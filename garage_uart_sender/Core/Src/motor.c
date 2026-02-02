/* motor.c */
#include "motor.h"
#include "tim.h"

#define SERVO_STOP   1500
#define SERVO_CCW     1420
#define SERVO_CW    1900

#define MOVE_OPEN_TIME_MS  500U
#define MOVE_CLOSE_TIME_MS 550U

#define BTN_DEBOUNCE_MS 30U

#define BTN_PORT GPIOA
#define BTN_PIN  GPIO_PIN_11

extern TIM_HandleTypeDef htim1;

static volatile GarageState currentState = STATE_CLOSED;
static uint32_t move_start_ms = 0U;
static uint8_t  is_moving     = 0U;
static uint32_t move_duration_ms = MOVE_CLOSE_TIME_MS;

static volatile uint8_t pending_manual = 0U;
static volatile uint8_t last_move_manual = 0U;

static uint8_t  btn_last_raw    = 1U;
static uint8_t  btn_stable      = 1U;
static uint32_t btn_last_chg_ms = 0U;

/* servo_set */
static void servo_set(uint16_t pwm)
{
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
}

/* start_open */
static void start_open(void)
{

  last_move_manual = pending_manual;
  pending_manual = 0U;
  servo_set(SERVO_CW);
  move_start_ms = HAL_GetTick();
  move_duration_ms = MOVE_OPEN_TIME_MS;
  is_moving = 1U;
  currentState = STATE_OPENING;
}

/* start_close */
static void start_close(void)
{

  last_move_manual = pending_manual;
  pending_manual = 0U;
  servo_set(SERVO_CCW);
  move_start_ms = HAL_GetTick();
  move_duration_ms = MOVE_CLOSE_TIME_MS;
  is_moving = 1U;
  currentState = STATE_CLOSING;
}

/* finish_move */
static void finish_move(GarageState final_state)
{
  servo_set(SERVO_STOP);
  is_moving = 0U;
  currentState = final_state;
}

/* Motor_Init */
void Motor_Init(void)
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  servo_set(SERVO_STOP);
  currentState = STATE_CLOSED;
  is_moving = 0U;
  move_start_ms = HAL_GetTick();
  move_duration_ms = MOVE_CLOSE_TIME_MS;

  btn_last_raw = (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET) ? 0U : 1U;
  btn_stable = btn_last_raw;
  btn_last_chg_ms = HAL_GetTick();

  pending_manual = 0U;
  last_move_manual = 0U;
}

/* Motor_GetState */
GarageState Motor_GetState(void)
{
  return currentState;
}

/* Motor_LastMoveWasManual */
uint8_t Motor_LastMoveWasManual(void)
{
  return last_move_manual;
}

/* Motor_Process */
void Motor_Process(GarageCommand inputCmd)
{

  {
    const uint32_t now = HAL_GetTick();
    const uint8_t raw = (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET) ? 0U : 1U;

    if (raw != btn_last_raw)
    {
      btn_last_raw = raw;
      btn_last_chg_ms = now;
    }

    if ((now - btn_last_chg_ms) >= BTN_DEBOUNCE_MS && raw != btn_stable)
    {
      btn_stable = raw;

      if (btn_stable == 0U)
      {

        if (!is_moving)
        {
          if (currentState == STATE_CLOSED)
          {
            pending_manual = 1U;
            inputCmd = CMD_OPEN;
          }

          else if (currentState == STATE_OPEN)
          {
            pending_manual = 1U;
            inputCmd = CMD_CLOSE;
          }
        }
      }
    }
  }

  if (is_moving)
  {
    const uint32_t now = HAL_GetTick();
    if ((now - move_start_ms) >= move_duration_ms)
    {
      if (currentState == STATE_OPENING)
        finish_move(STATE_OPEN);
      else if (currentState == STATE_CLOSING)
        finish_move(STATE_CLOSED);
      else
        finish_move(currentState);
    }
    return;
  }

  switch (inputCmd)
  {
    case CMD_OPEN:
      if (currentState == STATE_CLOSED)
      {
        last_move_manual = pending_manual;
        pending_manual = 0U;
        start_open();
      }
      break;

    case CMD_CLOSE:
      if (currentState == STATE_OPEN)
      {
        last_move_manual = pending_manual;
        pending_manual = 0U;
        start_close();
      }
      break;

    case CMD_STOP:

      servo_set(SERVO_STOP);
      is_moving = 0U;
      break;

    case CMD_IDLE:
    default:
      break;
  }
}
