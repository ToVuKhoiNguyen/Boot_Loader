#ifndef XMODEM_H_
#define XMODEM_H_

#include <stdint.h>

typedef enum {
  XMODEM_OK = 0,
  XMODEM_ERR_TIMEOUT,
  XMODEM_ERR_TOO_LARGE,
  XMODEM_ERR_CANCELLED,
  XMODEM_ERR_TOO_MANY_RETRIES,
} xmodem_result_t;

xmodem_result_t XMODEM_Receive(uint32_t dest_addr, uint32_t max_len, uint32_t *out_received);

#endif
