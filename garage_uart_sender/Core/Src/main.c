/* main.c */
#include "main.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "sd_log.h"
#include "motor.h"
#include "sensors.h"

#include "time_sync.h"

#include "led.h"

static uint8_t ui_seq_g = 0;
static uint32_t ui_last_heartbeat_ms_g = 0;

void SystemClock_Config(void);

static led_state_t ui_from_motor_state(GarageState st);
static void UI_SendState(led_state_t st);

/* ui_from_motor_state */
static led_state_t ui_from_motor_state(GarageState st)
{
  switch (st)
  {
    case STATE_OPEN:
      return OPEN;
    case STATE_CLOSED:
      return CLOSED;
    case STATE_OPENING:
    case STATE_CLOSING:
    default:
      return MOVEMENT_IN_PROGRESS;
  }
}

/* UI_SendState */
static void UI_SendState(led_state_t st)
{

  uint8_t pkt[5];
  pkt[0] = 0xAA;
  pkt[1] = 0x55;
  pkt[2] = (uint8_t)st;
  pkt[3] = ui_seq_g++;
  pkt[4] = (uint8_t)(pkt[0] ^ pkt[1] ^ pkt[2] ^ pkt[3]);

  (void)HAL_UART_Transmit(&huart1, pkt, sizeof(pkt), 5);
}

/* main */
int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_FATFS_Init();
  MX_USART1_UART_Init();

  TimeSync_Init();

  Motor_Init();
  Sensor_Init();

  ledInit();

  {
    led_state_t ui0 = ui_from_motor_state(Motor_GetState());
    ledSetState(ui0);
    UI_SendState(ui0);
    ui_last_heartbeat_ms_g = HAL_GetTick();
  }

  SDLog_Init();

  uint32_t timer_close_start = 0;
  uint8_t timer_running = 0;
  uint8_t auto_arrived = 0;

  uint8_t timer_mode = 0;
  const uint32_t CLOSE_DELAY_NO_ENTRY_MS = 15000;
  const uint32_t CLOSE_DELAY_AFTER_ENTRY_MS = 2000;

  while (1)
  {
    TimeSync_Task();

    Sensor_Update();
    CarPosition pos = Sensor_GetCarPosition();

    static GarageState prev_gst = STATE_CLOSED;

    static CarPosition last_known_side = CAR_NONE;

    static uint8_t car_present = 0U;

    static uint8_t  exit_window = 0U;
    static uint32_t exit_empty_since_ms = 0U;

    static uint32_t closed_since_ms = 0U;

    #define AFTER_CLOSE_STABLE_MS   800U
    #define EXIT_EMPTY_CONFIRM_MS  2000U

    GarageState gst_now = Motor_GetState();
    const GarageState gst_prev = prev_gst;

    const uint8_t inside_raw = (pos == CAR_INSIDE || pos == CAR_BOTH) ? 1U : 0U;

    if (pos == CAR_OUTSIDE || pos == CAR_INSIDE)
      last_known_side = pos;

    if (gst_now != prev_gst)
    {

      if (gst_now == STATE_OPENING && prev_gst != STATE_OPENING)
      {
        if (Motor_LastMoveWasManual())
        {
          SDLog_Printf("BRANA MANUALNE OTVORENA");
        }
        else
        {

          CarPosition side = (pos == CAR_OUTSIDE || pos == CAR_INSIDE) ? pos : last_known_side;

          if (side == CAR_OUTSIDE)      SDLog_Printf("VJAZD DO GARAZE POVOLENY");
          else if (side == CAR_INSIDE)  SDLog_Printf("VYJAZD Z GARAZE POVOLENY");
          else                          SDLog_Printf("OTVORENIE GARAZE (SMER NEURCITY)");
        }
      }

      if (gst_now == STATE_CLOSING && prev_gst != STATE_CLOSING)
      {
        if (Motor_LastMoveWasManual()) SDLog_Printf("BRANA SA MANUALNE ZATVARA");
        else                           SDLog_Printf("BRANA SA ZATVARA");
      }

      if (gst_now == STATE_OPEN)      SDLog_Printf("BRANA OTVORENA");
      if (gst_now == STATE_CLOSED)    SDLog_Printf("BRANA ZATVORENA");

      prev_gst = gst_now;
    }

    if (gst_now == STATE_CLOSED)
    {

      if (gst_prev != STATE_CLOSED)
      {
        closed_since_ms = HAL_GetTick();
      }

      exit_window = 0U;
      exit_empty_since_ms = 0U;

      if (closed_since_ms != 0U && (HAL_GetTick() - closed_since_ms) >= AFTER_CLOSE_STABLE_MS)
      {
        if (inside_raw && !car_present)
        {
          car_present = 1U;
          SDLog_Printf("AUTO ZAPARKOVANE V GARAZI");
        }

      }
    }
    else
    {
      closed_since_ms = 0U;
    }

    if (gst_now == STATE_OPEN && gst_prev != STATE_OPEN)
    {
      if (Motor_LastMoveWasManual() && car_present)
      {
        exit_window = 1U;
        exit_empty_since_ms = 0U;

      }
    }

    if (exit_window)
    {
      const uint32_t now = HAL_GetTick();
      if (!inside_raw)
      {
        if (exit_empty_since_ms == 0U) exit_empty_since_ms = now;
        if ((now - exit_empty_since_ms) >= EXIT_EMPTY_CONFIRM_MS)
        {
          car_present = 0U;
          exit_window = 0U;
          exit_empty_since_ms = 0U;
          SDLog_Printf("AUTO ODISLO - GARAZ PRAZDNA");
        }
      }
      else
      {

        exit_empty_since_ms = 0U;
      }
    }

    GarageCommand cmd = CMD_IDLE;

    switch (pos)
    {
      case CAR_OUTSIDE:

        cmd = CMD_OPEN;
        timer_running = 0;
        timer_mode = 0;
        auto_arrived = 1;
        break;

      case CAR_INSIDE:

        if (auto_arrived == 1)
        {
          if (timer_running == 0 || timer_mode != 2)
          {
            timer_close_start = HAL_GetTick();
            timer_running = 1;
            timer_mode = 2;
            cmd = CMD_IDLE;
          }
          else
          {
            if ((HAL_GetTick() - timer_close_start) > CLOSE_DELAY_AFTER_ENTRY_MS)
            {
              cmd = CMD_CLOSE;
              auto_arrived = 0;
              timer_running = 0;
              timer_mode = 0;
            }
            else
            {
              cmd = CMD_IDLE;
            }
          }
        }
        else
        {
          cmd = CMD_IDLE;
        }
        break;

      case CAR_NONE:

        if (auto_arrived == 1)
        {
          if (timer_running == 0 || timer_mode != 1)
          {
            timer_close_start = HAL_GetTick();
            timer_running = 1;
            timer_mode = 1;
            cmd = CMD_IDLE;
          }
          else
          {
            if ((HAL_GetTick() - timer_close_start) > CLOSE_DELAY_NO_ENTRY_MS)
            {
              cmd = CMD_CLOSE;
              auto_arrived = 0;
              timer_running = 0;
              timer_mode = 0;
            }
            else
            {
              cmd = CMD_IDLE;
            }
          }
        }
        else
        {
          cmd = CMD_IDLE;
        }
        break;

      default:
        cmd = CMD_IDLE;
        break;
    }

    Motor_Process(cmd);

    {
      led_state_t ui = ui_from_motor_state(Motor_GetState());
      if (ui != ledGetState())
      {
        ledSetState(ui);
        UI_SendState(ui);
        ui_last_heartbeat_ms_g = HAL_GetTick();
      }
      ledProcess();
    }

    if ((HAL_GetTick() - ui_last_heartbeat_ms_g) > 500)
    {
      UI_SendState(ledGetState());
      ui_last_heartbeat_ms_g = HAL_GetTick();
    }

    {
      static uint32_t last_sd_ms = 0;
      if ((HAL_GetTick() - last_sd_ms) > 200U)
      {
        last_sd_ms = HAL_GetTick();
        GarageState st = Motor_GetState();
        uint8_t safe = (uint8_t)((st != STATE_OPENING) && (st != STATE_CLOSING));
        SDLog_Process(safe);
      }
    }

    HAL_Delay(50);
  }

}

/* SystemClock_Config */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* HAL_UART_RxCpltCallback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  TimeSync_UART_RxCpltCallback(huart);
}

/* Error_Handler */
void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }

}
#ifdef USE_FULL_ASSERT

/* assert_failed */
void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif
