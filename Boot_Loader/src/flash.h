#ifndef FLASH_H_
#define FLASH_H_

#include <stdbool.h>
#include <stdint.h>

#define SLOT_A_BASE 0x00010000UL
#define SLOT_B_BASE 0x00020000UL
#define SLOT_SIZE 0x00010000UL           // 64KB
#define FLASH_PAGE_SIZE_BTL 0x00002000UL // 8KB

void FLASH_DriverInit(void);
bool FLASH_EraseSlot(uint32_t slot_base);
bool FLASH_Write(uint32_t addr, const void *data, uint32_t len);

#endif
