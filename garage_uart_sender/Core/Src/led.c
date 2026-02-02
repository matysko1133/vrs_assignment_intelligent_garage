/* led.c */
#include "led.h"

#include "main.h"
#include "stm32f3xx_hal.h"

static led_state_t s_state = CLOSED;

static uint32_t s_blink_period_ms = 250U;
static uint32_t s_last_toggle_ms  = 0U;
static uint8_t  s_blink_phase_on  = 0U;

/* write_leds */
static void write_leds(GPIO_PinState green, GPIO_PinState red)
{
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, green);
  HAL_GPIO_WritePin(LED_RED_GPIO_Port,   LED_RED_Pin,   red);
}

/* apply_state_immediate */
static void apply_state_immediate(void)
{
  switch (s_state)
  {
    case OPEN:
      write_leds(GPIO_PIN_SET, GPIO_PIN_RESET);
      break;

    case CLOSED:
      write_leds(GPIO_PIN_RESET, GPIO_PIN_SET);
      break;

    case MOVEMENT_IN_PROGRESS:

      if (s_blink_phase_on)
        write_leds(GPIO_PIN_SET, GPIO_PIN_SET);
      else
        write_leds(GPIO_PIN_RESET, GPIO_PIN_RESET);
      break;

    default:

      write_leds(GPIO_PIN_RESET, GPIO_PIN_RESET);
      break;
  }
}

/* ledInit */
void ledInit(void)
{
  s_last_toggle_ms = HAL_GetTick();
  s_blink_phase_on = 0U;
  apply_state_immediate();
}

/* ledSetState */
void ledSetState(led_state_t state)
{
  s_state = state;

  s_last_toggle_ms = HAL_GetTick();
  s_blink_phase_on = 0U;

  apply_state_immediate();
}

/* ledGetState */
led_state_t ledGetState(void)
{
  return s_state;
}

/* ledProcess */
void ledProcess(void)
{
  if (s_state != MOVEMENT_IN_PROGRESS)
  {

    apply_state_immediate();
    return;
  }

  const uint32_t now = HAL_GetTick();
  if ((now - s_last_toggle_ms) >= s_blink_period_ms)
  {
    s_last_toggle_ms = now;
    s_blink_phase_on = (uint8_t)!s_blink_phase_on;
    apply_state_immediate();
  }
}

/* ledSetBlinkPeriodMs */
void ledSetBlinkPeriodMs(uint32_t period_ms)
{
  if (period_ms == 0U) period_ms = 1U;
  s_blink_period_ms = period_ms;
}
