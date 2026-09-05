#ifndef VALIDATE_H_
#define VALIDATE_H_

#include "shared/dual_slot_header.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  FW_STATUS_VALID = 0,
  FW_STATUS_BAD_MAGIC,
  FW_STATUS_BAD_SIZE,
  FW_STATUS_BAD_CRC,
  FW_STATUS_BAD_VECTOR_TABLE,
} fw_status_t;

void FW_ReadHeader(uint32_t slot_base, fw_header_t *out_header);
fw_status_t FW_Validate(uint32_t slot_base, bool print_log);

#endif
