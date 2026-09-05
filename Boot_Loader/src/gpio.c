#include "gpio.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"

#define UPDATE_MODE_THRESHOLD_MS 3000U

#define BTN0_PORT gpioPortC
#define BTN0_PIN 7U
#define LED0_PORT gpioPortA
#define LED0_PIN 4U

// Systick
static volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) { ms_ticks++; }

void SYSTICK_Init(void) { SysTick_Config(SystemCoreClock / 1000U); }

uint32_t SYSTICK_GetMs(void) { return ms_ticks; }

void SYSTICK_DelayMs(uint32_t ms) {
  uint32_t start = ms_ticks;
  while ((ms_ticks - start) < ms) {
  }
}

void GPIO_BTN0_Init(void) {
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BTN0_PORT, BTN0_PIN, gpioModeInputPull, 1);
}

bool GPIO_BTN0_IsPressed(void) { return GPIO_PinInGet(BTN0_PORT, BTN0_PIN) == 0U; }

void GPIO_LED0_Init(void) {
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);
}

void GPIO_LED0_Set(bool on) {
  if (on) {
    GPIO_PinOutSet(LED0_PORT, LED0_PIN);
  } else {
    GPIO_PinOutClear(LED0_PORT, LED0_PIN);
  }
}

void LED_Pattern_Run(led_pattern_t pattern) {
  switch (pattern) {
  case LED_PATTERN_DEFAULT_BLINK_ONCE:
    GPIO_LED0_Set(true);
    SYSTICK_DelayMs(500);
    GPIO_LED0_Set(false);
    break;

  case LED_PATTERN_ROLLBACK_FAST:
    GPIO_LED0_Set(true);
    SYSTICK_DelayMs(200);
    GPIO_LED0_Set(false);
    SYSTICK_DelayMs(200);
    break;

  case LED_PATTERN_UPDATE_STEADY:
    GPIO_LED0_Set(true);
    SYSTICK_DelayMs(500);
    GPIO_LED0_Set(false);
    SYSTICK_DelayMs(500);
    break;

  case LED_PATTERN_SAFE_MODE_ERROR:
    for (int i = 0; i < 2; i++) {
      GPIO_LED0_Set(true);
      SYSTICK_DelayMs(100);
      GPIO_LED0_Set(false);
      SYSTICK_DelayMs(100);
    }
    SYSTICK_DelayMs(500);
    break;

  case LED_PATTERN_OFF:
  default:
    GPIO_LED0_Set(false);
    break;
  }
}

boot_mode_t BUTTON_DetectHoldMode(void) {
  if (!GPIO_BTN0_IsPressed()) {
    return BOOT_MODE_DEFAULT;
  }

  uint32_t press_start = SYSTICK_GetMs();

  while (GPIO_BTN0_IsPressed()) {
    uint32_t held = SYSTICK_GetMs() - press_start;
    if (held >= UPDATE_MODE_THRESHOLD_MS) {
      return BOOT_MODE_UPDATE;
    }
    LED_Pattern_Run(LED_PATTERN_ROLLBACK_FAST);
  }
  return BOOT_MODE_ROLLBACK;
}
