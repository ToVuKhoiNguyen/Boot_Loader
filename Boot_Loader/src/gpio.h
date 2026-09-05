#ifndef GPIO_H_
#define GPIO_H_

#include <stdbool.h>
#include <stdint.h>

void GPIO_BTN0_Init(void);
bool GPIO_BTN0_IsPressed(void);
void GPIO_LED0_Init(void);
void GPIO_LED0_Set(bool on);

typedef enum {
  LED_PATTERN_OFF,
  LED_PATTERN_DEFAULT_BLINK_ONCE,
  LED_PATTERN_ROLLBACK_FAST,
  LED_PATTERN_UPDATE_STEADY,
  LED_PATTERN_SAFE_MODE_ERROR,
} led_pattern_t;

typedef enum {
  BOOT_MODE_DEFAULT = 0,
  BOOT_MODE_ROLLBACK,
  BOOT_MODE_UPDATE,
} boot_mode_t;

boot_mode_t BUTTON_DetectHoldMode(void);

void SYSTICK_Init(void);
uint32_t SYSTICK_GetMs(void);
void SYSTICK_DelayMs(uint32_t ms);

void LED_Pattern_Run(led_pattern_t pattern);



#endif
