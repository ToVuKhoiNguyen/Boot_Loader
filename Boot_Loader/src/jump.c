/***************************************************************************//**
 * @file btl_jump.c
 * @brief CPU preparation and jump to application firmware.
 * @details Follows the standard technique used by Silicon Labs' Gecko
 *          Bootloader internally: set SCB->VTOR, then load MSP/PC via
 *          inline assembly.
 ******************************************************************************/
#include "jump.h"
#include "em_device.h"
#include "em_usart.h"
#include "em_cmu.h"

/***************************************************************************//**
 * @brief De-initialize peripherals used by the bootloader.
 * @details Per requirement Muc 8: reset UART/Timer/SysTick to default state
 *          before leaving the bootloader.
 ******************************************************************************/
static void deinit_peripherals(void)
{
  // Dừng SysTick
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL  = 0;

  // Đưa USART1 về trạng thái mặc định (chỉ khi Clock đã được bật bởi UART_Init).
  // Cố gắng truy cập thanh ghi USART1 khi chưa bật Clock sẽ gây lỗi HardFault
  // trên dòng chip Series 2 (như BGM220). Ở chế độ Default/Rollback, UART_Init
  // không được gọi, nên bắt buộc phải có đoạn code bảo vệ này.
  CMU_ClockEnable(cmuClock_USART1, true);   // Bật tạm Clock (an toàn kể cả khi đã bật sẵn)
  USART_Reset(USART1);
  CMU_ClockEnable(cmuClock_USART1, false);
}

/***************************************************************************//**
 * @brief Perform the actual jump via pure C function pointers and CMSIS.
 * @param[in] vector_addr  Address of the application's vector table.
 ******************************************************************************/
static void __attribute__((noreturn)) jump_to_app_c(uint32_t vector_addr)
{
  uint32_t initial_msp = *(volatile uint32_t *)vector_addr;
  uint32_t reset_handler_addr = *(volatile uint32_t *)(vector_addr + 4);
  void (*app_reset_handler)(void) = (void (*)(void))reset_handler_addr;

  __set_MSP(initial_msp);
  __set_PSP(initial_msp);
  
  app_reset_handler();

  while (1) { } // Không bao giờ chạm tới đây
}

void JUMP_ToApplication(uint32_t vector_addr)
{
  __disable_irq();

  // Xóa toàn bộ ngắt đang chờ hoặc đang bật trong NVIC.
  for (int i = 0; i < (EXT_IRQ_COUNT + 31) / 32; i++) {
    NVIC->ICER[i] = 0xFFFFFFFFUL;
    NVIC->ICPR[i] = 0xFFFFFFFFUL;
  }

  deinit_peripherals();

  __DSB();
  __ISB();

  SCB->VTOR = vector_addr;   // Địa chỉ vector_addr đã được căn lề 512 Bytes từ đầu

  __enable_irq();            // Application sẽ tự cấu hình ngắt của riêng nó

  jump_to_app_c(vector_addr);
}
