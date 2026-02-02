/* main.c */
#include "main.h"

#include "disp.h"
#include "led.h"

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

static uint8_t uart_rx_byte_g = 0;
static uint8_t parse_state_g = 0;
static uint8_t pkt_ui_state_g = 0;
static uint8_t pkt_seq_g = 0;

static volatile led_state_t ui_state_latest_g = CLOSED;
static volatile uint8_t ui_state_dirty_g = 0;
static uint32_t last_rx_ms_g = 0;
static uint8_t link_lost_shown_g = 0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

/* parse_byte */
static void parse_byte(uint8_t b)
{
  switch (parse_state_g)
  {
    case 0:
      if (b == 0xAA) parse_state_g = 1;
      break;
    case 1:
      if (b == 0x55) parse_state_g = 2;
      else parse_state_g = 0;
      break;
    case 2:
      pkt_ui_state_g = b;
      parse_state_g = 3;
      break;
    case 3:
      pkt_seq_g = b;
      parse_state_g = 4;
      break;
    case 4:
    {
      uint8_t chk = (uint8_t)(0xAA ^ 0x55 ^ pkt_ui_state_g ^ pkt_seq_g);
      if (b == chk)
      {

        if (pkt_ui_state_g <= (uint8_t)MOVEMENT_IN_PROGRESS)
        {
          ui_state_latest_g = (led_state_t)pkt_ui_state_g;
          ui_state_dirty_g = 1;
          last_rx_ms_g = HAL_GetTick();
          link_lost_shown_g = 0;
        }
      }
      parse_state_g = 0;
      break;
    }
    default:
      parse_state_g = 0;
      break;
  }
}

/* uart_start_rx */
static void uart_start_rx(void)
{

  (void)HAL_UART_Receive_IT(&huart1, &uart_rx_byte_g, 1);
}

/* HAL_UART_RxCpltCallback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    parse_byte(uart_rx_byte_g);
    uart_start_rx();
  }
}

/* HAL_UART_ErrorCallback */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {

    (void)HAL_UART_AbortReceive_IT(&huart1);
    parse_state_g = 0;
    (void)HAL_UART_Receive_IT(&huart1, &uart_rx_byte_g, 1);
  }
}

/* main */
int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  if (dispInit(&hi2c1) != HAL_OK)
  {

  }
  dispPrint2("WAIT UART", "");

  uart_start_rx();
  last_rx_ms_g = HAL_GetTick();

  while (1)
  {

    if (ui_state_dirty_g)
    {
      ui_state_dirty_g = 0;
      dispSetState(ui_state_latest_g);
    }

    if (!link_lost_shown_g && (HAL_GetTick() - last_rx_ms_g) > 2000)
    {
      dispPrint2("LINK LOST", "");
      link_lost_shown_g = 1;
    }

    {
      static uint32_t last_rearm_ms = 0;
      if ((HAL_GetTick() - last_rearm_ms) > 250U)
      {
        last_rearm_ms = HAL_GetTick();
        if ((HAL_GetTick() - last_rx_ms_g) > 500U)
        {
          (void)HAL_UART_AbortReceive_IT(&huart1);
          parse_state_g = 0;
          (void)HAL_UART_Receive_IT(&huart1, &uart_rx_byte_g, 1);
        }
      }
    }

    HAL_Delay(20);
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* MX_I2C1_Init */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

/* MX_USART1_UART_Init */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* MX_GPIO_Init */
static void MX_GPIO_Init(void)
{

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
