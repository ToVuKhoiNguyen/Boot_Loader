#include "uart.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "gpio.h"

#define VCOM_UART USART1
#define VCOM_TX_PORT gpioPortA
#define VCOM_TX_PIN 5U
#define VCOM_RX_PORT gpioPortA
#define VCOM_RX_PIN 6U
#define VCOM_ROUTE_LOC 1U

void UART_Init(uint32_t baudrate) {
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  GPIO_PinModeSet(VCOM_TX_PORT, VCOM_TX_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(VCOM_RX_PORT, VCOM_RX_PIN, gpioModeInput, 0);

  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  init.baudrate = baudrate;
  USART_InitAsync(VCOM_UART, &init);

  GPIO->USARTROUTE[VCOM_ROUTE_LOC].TXROUTE =
      (VCOM_TX_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT) | (VCOM_TX_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[VCOM_ROUTE_LOC].RXROUTE =
      (VCOM_RX_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT) | (VCOM_RX_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[VCOM_ROUTE_LOC].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RXPEN;
}

void UART_SendByte(uint8_t b) { USART_Tx(VCOM_UART, b); }

void UART_SendString(const char *s) {
  while (*s) {
    UART_SendByte((uint8_t)(*s++));
  }
}

// đợi 1 khoảng thời gian nếu không có dữ liệu trả về -1
// trong bootloader chỉ dùng để nhận boot mode
int UART_ReceiveByteTimeout(uint32_t timeout_ms) {
  uint32_t start = SYSTICK_GetMs();
  while ((SYSTICK_GetMs() - start) < timeout_ms) {
    if (VCOM_UART->STATUS & USART_STATUS_RXDATAV) {
      return (int)USART_Rx(VCOM_UART);
    }
  }
  return -1;
}

#include <stdarg.h>

// Hàm in siêu nhẹ tự viết để tránh bị HardFault từ thư viện stdio.h
void BTL_Printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  while (*fmt) {
    if (*fmt == '%') {
      fmt++;
      if (*fmt == 'c') {
        UART_SendByte((uint8_t)va_arg(args, int));
      } else if (*fmt == 's') {
        UART_SendString(va_arg(args, const char *));
      } else if (*fmt == 'l' && *(fmt + 1) == 'u') {
        fmt++;
        uint32_t val = va_arg(args, uint32_t);
        char buf[12];
        int i = 10;
        buf[11] = '\0';
        if (val == 0) {
          buf[i--] = '0';
        }
        while (val > 0) {
          buf[i--] = '0' + (val % 10);
          val /= 10;
        }
        UART_SendString(&buf[i + 1]);
      } else if (*fmt == '0' && *(fmt + 1) == '8' && *(fmt + 2) == 'l' && *(fmt + 3) == 'X') {
        fmt += 3;
        uint32_t val = va_arg(args, uint32_t);
        char buf[9];
        for (int i = 7; i >= 0; i--) {
          uint8_t nibble = val & 0xF;
          buf[i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
          val >>= 4;
        }
        buf[8] = '\0';
        UART_SendString(buf);
      }
    } else {
      UART_SendByte((uint8_t)*fmt);
    }
    fmt++;
  }
  va_end(args);
}
