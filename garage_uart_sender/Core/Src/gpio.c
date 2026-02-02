/* gpio.c */
#include "gpio.h"

/* MX_GPIO_Init */
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, LED_GREEN_Pin|LED_RED_Pin|TRIG_IN_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(TRIG_OUT_GPIO_Port, TRIG_OUT_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = LED_GREEN_Pin|LED_RED_Pin|TRIG_IN_Pin|SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = MAN_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(MAN_BTN_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ECHO_IN_Pin|ECHO_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = TRIG_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TRIG_OUT_GPIO_Port, &GPIO_InitStruct);

}
