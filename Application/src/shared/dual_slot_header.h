/***************************************************************************//**
 * @file dual_slot_header.h
 * @brief Shared firmware header structure for dual-slot bootloader.
 * @details This struct is written by the Python post-build tool and
 *          consumed by the bootloader's validation routine. Both sides
 *          MUST agree on field order/size (little-endian, packed).
 ******************************************************************************/
#ifndef DUAL_SLOT_HEADER_H_
#define DUAL_SLOT_HEADER_H_

#include <stdint.h>

#define FW_HEADER_MAGIC        0xDEADBEEFUL
/** Tổng dung lượng dự trữ cho phân vùng Header ở đầu mỗi Slot.
 *  MUST be a power of two >= 512 to satisfy Cortex-M33 SCB->VTOR alignment
 *  (EFR32BG22 has 64 external IRQs => 80 vector entries => 512-byte align). */
#define FW_HEADER_BLOCK_SIZE    0x200UL   // 512 bytes

#pragma pack(push, 1)
typedef struct {
  uint32_t magic;        ///< *< Bắt buộc bằng FW_HEADER_MAGIC (0xDEADBEEF)
  uint32_t img_size;      ///< *< Tổng dung lượng Image, tính CẢ kích thước Header
  uint32_t img_crc32;      /**< Mã CRC32 tính từ [slot_base, slot_base+img_size), với
                               these 4 bytes treated as 0x00000000 during calc */
  uint32_t vector_addr;    /**< Địa chỉ tuyệt đối của Vector Table của Application.
                               By construction == slot_base + FW_HEADER_BLOCK_SIZE */
  uint32_t version;      ///< *< Bộ đếm phiên bản build tăng dần
} fw_header_t;
#pragma pack(pop)

#endif // DUAL_SLOT_HEADER_H_
