#ifndef UART_H_
#define UART_H_

#include <stdbool.h>
#include <stdint.h>

void UART_Init(uint32_t baudrate);
void UART_SendByte(uint8_t b);
void UART_SendString(const char *s);
int UART_ReceiveByteTimeout(uint32_t timeout_ms);

void BTL_Printf(const char *fmt, ...);
#define printf BTL_Printf

#endif
