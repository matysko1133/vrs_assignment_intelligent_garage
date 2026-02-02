/* stm32f3xx_hal_msp.c */
#include "main.h"

/* HAL_MspInit */
void HAL_MspInit(void)
{

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

}
