#include "boot.h"

#include "em_device.h"
#include "flash.h"
#include "gpio.h"
#include "jump.h"
#include "shared/dual_slot_header.h"
#include "uart.h"
#include "validate.h"
#include "xmodem.h"

static void safe_mode_loop(void) {
  printf("\r\n[BTL] SAFE MODE: both slots invalid. "
         "Reset and hold BTN0 > 3s to enter Update Mode.\r\n");
  while (true) {
    LED_Pattern_Run(LED_PATTERN_SAFE_MODE_ERROR);
  }
}

static void run_update_mode(void) {
  fw_header_t hdr_a, hdr_b;
  bool valid_a = (FW_Validate(SLOT_A_BASE, false) == FW_STATUS_VALID);
  bool valid_b = (FW_Validate(SLOT_B_BASE, false) == FW_STATUS_VALID);
  FW_ReadHeader(SLOT_A_BASE, &hdr_a);
  FW_ReadHeader(SLOT_B_BASE, &hdr_b);

  uint32_t target_base;
  char target_name;

  if (!valid_a && valid_b) {
    target_base = SLOT_A_BASE;
    target_name = 'A';
  } else if (valid_a && !valid_b) {
    target_base = SLOT_B_BASE;
    target_name = 'B';
  } else if (!valid_a && !valid_b) {
    target_base = SLOT_A_BASE;
    target_name = 'A';
  } else if (hdr_a.version < hdr_b.version) {
    target_base = SLOT_A_BASE;
    target_name = 'A';
  } else if (hdr_b.version < hdr_a.version) {
    target_base = SLOT_B_BASE;
    target_name = 'B';
  } else {
    target_base = SLOT_B_BASE;
    target_name = 'B';
  }

  printf("\r\n[BTL] UPDATE MODE\r\n");
  printf("[BTL] Target Update: SLOT %c (Address: 0x%08lX)\r\n", target_name, target_base);
  printf("[BTL] Erasing Slot %c...\r\n", target_name);

  if (!FLASH_EraseSlot(target_base)) {
    printf("[BTL] ERROR: Flash Erase Failed!\r\n");
    return; // Thoát khỏi hàm, Reset hệ thống sẽ tự vào Default Mode
  }
  printf("[BTL] Erased! Ready to receive file via XMODEM-CRC...\r\n");

  uint32_t received = 0;
  xmodem_result_t res = XMODEM_Receive(target_base, SLOT_SIZE, &received);

  if (res != XMODEM_OK) {
    printf("[BTL] ERROR: File transfer failed.\r\n");
    return;
  }
  printf("[BTL] Transfer complete (%lu bytes). Validating...\r\n", received);

  fw_status_t status = FW_Validate(target_base, true);
  if (status == FW_STATUS_VALID) {
    printf("[BTL] New firmware VALID! Resetting system...\r\n");
    SYSTICK_DelayMs(200); // Chờ UART đẩy hết dữ liệu
    NVIC_SystemReset();   // Reset sạch -> Default Mode sẽ tự chọn bản mới nhất
  } else {
    printf("[BTL] ERROR: New firmware is INVALID.\r\n");
  }
}

void BOOT_Run(void) {
  FLASH_DriverInit();
  GPIO_BTN0_Init();
  GPIO_LED0_Init();
  SYSTICK_Init();
  UART_Init(115200);
  printf("\r\n\r\n---------------------------------------\r\n");
  printf("--- BGM220 DUAL-SLOT BOOTLOADER ---\r\n");
  printf("---------------------------------------\r\n");

  printf("\r\n[BTL] === SYSTEM STATUS REPORT ===\r\n");
  bool valid_a = (FW_Validate(SLOT_A_BASE, true) == FW_STATUS_VALID);
  bool valid_b = (FW_Validate(SLOT_B_BASE, true) == FW_STATUS_VALID);
  printf("[BTL] ================================\r\n\r\n");

  fw_header_t hdr_a, hdr_b;
  FW_ReadHeader(SLOT_A_BASE, &hdr_a);
  FW_ReadHeader(SLOT_B_BASE, &hdr_b);

  boot_mode_t mode = BUTTON_DetectHoldMode();

  if (mode == BOOT_MODE_DEFAULT) {
    printf("[BTL] Boot Mode Detected: DEFAULT\r\n");
  } else if (mode == BOOT_MODE_ROLLBACK) {
    printf("[BTL] Boot Mode Detected: ROLLBACK\r\n");
  } else {
    printf("[BTL] Boot Mode Detected: UPDATE\r\n");
  }

  if (mode == BOOT_MODE_UPDATE) {
    LED_Pattern_Run(LED_PATTERN_UPDATE_STEADY);
    run_update_mode();
    while (true) {
      LED_Pattern_Run(LED_PATTERN_UPDATE_STEADY);
      run_update_mode();
    }
  }

  uint32_t chosen_base = 0;
  bool have_choice = false;

  if (valid_a && valid_b) {
    printf("[BTL] Comparing: Slot A (v%lu) vs Slot B (v%lu)\r\n", hdr_a.version, hdr_b.version);
    if (mode == BOOT_MODE_DEFAULT) {
      // Chọn version cao hơn; nếu bằng nhau -> Ưu tiên Slot A.
      chosen_base = (hdr_a.version >= hdr_b.version) ? SLOT_A_BASE : SLOT_B_BASE;
    } else { // BOOT_MODE_ROLLBACK
      // Chọn version thấp hơn; nếu bằng nhau -> Ưu tiên Slot B.
      if (hdr_a.version < hdr_b.version) {
        chosen_base = SLOT_A_BASE;
      } else if (hdr_b.version < hdr_a.version) {
        chosen_base = SLOT_B_BASE;
      } else {
        // Version bằng nhau -> Ưu tiên Slot B để Rollback
        chosen_base = SLOT_B_BASE;
      }
    }
    have_choice = true;
  } else if (valid_a && !valid_b) {
    printf("[BTL] Slot A is valid, Slot B is invalid. Selecting Slot A.\r\n");
    chosen_base = SLOT_A_BASE;
    have_choice = true;
  } else if (!valid_a && valid_b) {
    printf("[BTL] Slot B is valid, Slot A is invalid. Selecting Slot B.\r\n");
    chosen_base = SLOT_B_BASE;
    have_choice = true;
  } else {
    printf("[BTL] Both slots are invalid.\r\n");
  }

  if (have_choice) {
    printf("[BTL] Decision: Booting from Slot %c!\r\n", (chosen_base == SLOT_A_BASE) ? 'A' : 'B');
    if (FW_Validate(chosen_base, false) == FW_STATUS_VALID) {
      printf("[BTL] Jumping to Application...\r\n");
      SYSTICK_DelayMs(50);
      LED_Pattern_Run(mode == BOOT_MODE_ROLLBACK ? LED_PATTERN_ROLLBACK_FAST : LED_PATTERN_DEFAULT_BLINK_ONCE);
      fw_header_t hdr;
      FW_ReadHeader(chosen_base, &hdr);
      JUMP_ToApplication(hdr.vector_addr);
      // Không bao giờ quay lại.
    }
  }

  // Thử boot Slot còn lại để cứu hộ.
  uint32_t other_base = (chosen_base == SLOT_A_BASE) ? SLOT_B_BASE : SLOT_A_BASE;
  printf("[BTL] WARNING: Chosen slot failed validation! Trying Fallback to Slot %c...\r\n",
         (other_base == SLOT_A_BASE) ? 'A' : 'B');
  if (have_choice && FW_Validate(other_base, false) == FW_STATUS_VALID) {
    fw_header_t hdr;
    FW_ReadHeader(other_base, &hdr);
    printf("[BTL] Fallback successful. Jumping to Application...\r\n");
    SYSTICK_DelayMs(50);
    LED_Pattern_Run(LED_PATTERN_DEFAULT_BLINK_ONCE);
    JUMP_ToApplication(hdr.vector_addr);
  }

  // Cả hai Slot đều hỏng -> Vào Safe Mode.
  safe_mode_loop();
}
