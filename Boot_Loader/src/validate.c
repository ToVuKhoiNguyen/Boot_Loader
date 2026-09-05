#include "validate.h"
#include "flash.h"
#include "uart.h"
#include <string.h>

// Kiểm tra toàn vẹn dữ liệu bằng thuật toán CRC32
static uint32_t crc32_table[256];
static bool table_ready = false;

static void CRC32_BuildTable(void) {
  for (uint32_t i = 0; i < 256; i++) {
    uint32_t c = i;
    for (int k = 0; k < 8; k++) {
      c = (c & 1U) ? (0xEDB88320UL ^ (c >> 1)) : (c >> 1);
    }
    crc32_table[i] = c;
  }
  table_ready = true;
}

static uint32_t CRC32_Continue(uint32_t crc, const uint8_t *data, uint32_t len) {
  if (!table_ready) {
    CRC32_BuildTable();
  }
  for (uint32_t i = 0; i < len; i++) {
    crc = crc32_table[(crc ^ data[i]) & 0xFFU] ^ (crc >> 8);
  }
  return crc;
}

static uint32_t CRC32_Finalize(uint32_t partial_crc) { return partial_crc ^ 0xFFFFFFFFUL; }

#define SRAM_LOW 0x20000000UL
#define SRAM_HIGH (0x20000000UL + 0x00008000UL) // 32 KB RAM

void FW_ReadHeader(uint32_t slot_base, fw_header_t *out_header) {
  memcpy(out_header, (const void *)slot_base, sizeof(fw_header_t));
}

fw_status_t FW_Validate(uint32_t slot_base, bool print_log) {
  fw_header_t hdr;
  FW_ReadHeader(slot_base, &hdr);

  char slot_name = (slot_base == 0x10000) ? 'A' : 'B';

  if (print_log) {
    printf("\r\n[BTL] --- Checking SLOT %c (0x%08lX) ---\r\n", slot_name, slot_base);
    if (hdr.magic == FW_HEADER_MAGIC) {
      printf("[BTL] Header reports Version: v%lu\r\n", hdr.version);
    } else {
      printf("[BTL] Empty or corrupt (No valid header found).\r\n");
    }
  }

  if (hdr.magic != FW_HEADER_MAGIC) {
    if (print_log) printf("[BTL] -> ERROR: Bad Magic Number (Got: 0x%08lX)\r\n", hdr.magic);
    return FW_STATUS_BAD_MAGIC;
  }

  if (hdr.img_size < FW_HEADER_BLOCK_SIZE || hdr.img_size > SLOT_SIZE) {
    if (print_log) printf("[BTL] -> ERROR: Bad Image Size (Got: %lu bytes)\r\n", hdr.img_size);
    return FW_STATUS_BAD_SIZE;
  }

  // Gọi CRC đọc hết magic và size, đổi CRC thành 0 rồi đọc tiếp đến hết
  uint32_t zero = 0;
  uint32_t crc_state;

  crc_state = CRC32_Continue(0xFFFFFFFFUL, (const uint8_t *)slot_base, 8);
  crc_state = CRC32_Continue(crc_state, (const uint8_t *)&zero, 4);
  crc_state = CRC32_Continue(crc_state, (const uint8_t *)(slot_base + 12), hdr.img_size - 12);
  crc_state = CRC32_Finalize(crc_state);

  if (crc_state != hdr.img_crc32) {
    if (print_log) printf("[BTL] -> ERROR: Bad CRC32 (Expected: 0x%08lX, Calc: 0x%08lX)\r\n", hdr.img_crc32, crc_state);
    return FW_STATUS_BAD_CRC;
  }

  if (hdr.vector_addr != slot_base + FW_HEADER_BLOCK_SIZE) {
    if (print_log) printf("[BTL] -> ERROR: Bad Vector Address (Got: 0x%08lX)\r\n", hdr.vector_addr);
    return FW_STATUS_BAD_VECTOR_TABLE;
  }

  uint32_t msp = *(const uint32_t *)(hdr.vector_addr + 0);
  uint32_t reset = *(const uint32_t *)(hdr.vector_addr + 4);

  if (msp < SRAM_LOW || msp >= SRAM_HIGH) {
    if (print_log) printf("[BTL] -> ERROR: MSP outside SRAM (Got: 0x%08lX)\r\n", msp);
    return FW_STATUS_BAD_VECTOR_TABLE;
  }
  if (reset < slot_base || reset >= (slot_base + SLOT_SIZE)) {
    if (print_log) printf("[BTL] -> ERROR: Reset Handler outside Slot (Got: 0x%08lX)\r\n", reset);
    return FW_STATUS_BAD_VECTOR_TABLE;
  }

  if (print_log) printf("[BTL] -> VALID! Ready for use.\r\n");
  return FW_STATUS_VALID;
}
