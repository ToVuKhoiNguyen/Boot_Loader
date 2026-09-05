#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_timer.h"

#define EXT_LED_PORT gpioPortA
#define EXT_LED_PIN 4U

#define PWM_PERIOD_STEPS 100U
#define BREATHING_STEP_MS 20U

static volatile uint32_t pwm_counter = 0;
static volatile uint32_t pwm_duty = 0;
static volatile uint32_t ms_counter = 0;

void TIMER0_IRQHandler(void) {
  TIMER_IntClear(TIMER0, TIMER_IF_OF);

  // PWM mềm bật LED khi counter < duty
  if (pwm_counter < pwm_duty) {
    GPIO_PinOutSet(EXT_LED_PORT, EXT_LED_PIN);
  } else {
    GPIO_PinOutClear(EXT_LED_PORT, EXT_LED_PIN);
  }

  pwm_counter++;
  if (pwm_counter >= PWM_PERIOD_STEPS) {
    pwm_counter = 0;
    ms_counter++;
  }
}

static void PWM_TimerInit(void) {
  CMU_ClockEnable(cmuClock_TIMER0, true);

TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
timerInit.enable = false; // Chưa cho chạy vội

TIMER_Init(TIMER0, &timerInit);

uint32_t top_value = (SystemCoreClock / (PWM_PERIOD_STEPS * 1000U)) - 1U;
if (top_value < 1U) {
  top_value = 1U;
}
TIMER_TopSet(TIMER0, top_value);

// Bật ngắt tràn
TIMER_IntEnable(TIMER0, TIMER_IEN_OF);
NVIC_ClearPendingIRQ(TIMER0_IRQn);
NVIC_EnableIRQ(TIMER0_IRQn);

// Bắt đầu chạy Timer
TIMER_Enable(TIMER0, true);
}

void APP_Run(void) {
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(EXT_LED_PORT, EXT_LED_PIN, gpioModePushPull, 0);

  PWM_TimerInit();

  // Hiệu ứng sáng tối
  uint32_t current_duty = 0;
  bool ramping_up = true;
  uint32_t last_step_ms = 0;

  while (1) {
    while ((ms_counter - last_step_ms) < BREATHING_STEP_MS) {
    }
    last_step_ms = ms_counter;

    if (ramping_up) {
      current_duty++;
      if (current_duty >= PWM_PERIOD_STEPS) {
        current_duty = PWM_PERIOD_STEPS;
        ramping_up = false;
      }
    } else {
      if (current_duty > 0) {
        current_duty--;
      }
      if (current_duty == 0) {
        ramping_up = true;
      }
    }

    __disable_irq();
    pwm_duty = current_duty;
    __enable_irq();
  }
}
