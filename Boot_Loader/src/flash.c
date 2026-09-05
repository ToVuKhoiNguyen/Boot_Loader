#include "flash.h"
#include "em_msc.h"

void FLASH_DriverInit(void) { MSC_Init(); }

bool FLASH_EraseSlot(uint32_t slot_base) {
  const uint32_t num_pages = SLOT_SIZE / FLASH_PAGE_SIZE_BTL; // 8 pages

  for (uint32_t i = 0; i < num_pages; i++) {
    uint32_t page_addr = slot_base + (i * FLASH_PAGE_SIZE_BTL);
    // Hàm xóa 1 page
    if (MSC_ErasePage((uint32_t *)page_addr) != mscReturnOk) {
      return false;
    }
  }
  return true;
}

bool FLASH_Write(uint32_t addr, const void *data, uint32_t len) {
  // Đảm bảo địa chỉ và độ dài là word-aligned chia hết cho 4
  if ((addr % 4) != 0 || (len % 4) != 0) {
    return false;
  }
  // Hàm ghi word
  return MSC_WriteWord((uint32_t *)addr, data, len) == mscReturnOk;
}
