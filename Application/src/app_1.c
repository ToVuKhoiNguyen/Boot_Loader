#include "app_run.h"
#include "em_cmu.h"
#include "em_gpio.h"

#define EXT_LED_PORT gpioPortA
#define EXT_LED_PIN 4

void APP_Run(void) {
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(gpioPortA, 4U, gpioModePushPull, 0);

  // Delay mềm một chút để Clock ổn định
  for (volatile uint32_t d = 0; d < 200000U; d++) {
  }

  while (1) {
    GPIO_PinOutSet(gpioPortA, 4U);
    for (volatile uint32_t i = 0; i < 500000U; i++) {
    }
    GPIO_PinOutClear(gpioPortA, 4U);
    for (volatile uint32_t i = 0; i < 500000U; i++) {
    }
  }
}
