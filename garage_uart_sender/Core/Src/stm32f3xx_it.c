/* stm32f3xx_it.c */
#include "main.h"
#include "stm32f3xx_it.h"

extern UART_HandleTypeDef huart1;

/* NMI_Handler */
void NMI_Handler(void)
{

   while (1)
  {
  }

}

/* HardFault_Handler */
void HardFault_Handler(void)
{

  while (1)
  {

  }
}

/* MemManage_Handler */
void MemManage_Handler(void)
{

  while (1)
  {

  }
}

/* BusFault_Handler */
void BusFault_Handler(void)
{

  while (1)
  {

  }
}

/* UsageFault_Handler */
void UsageFault_Handler(void)
{

  while (1)
  {

  }
}

/* SVC_Handler */
void SVC_Handler(void)
{

}

/* DebugMon_Handler */
void DebugMon_Handler(void)
{

}

/* PendSV_Handler */
void PendSV_Handler(void)
{

}

/* SysTick_Handler */
void SysTick_Handler(void)
{

  HAL_IncTick();

}

/* USART1_IRQHandler */
void USART1_IRQHandler(void)
{

  HAL_UART_IRQHandler(&huart1);

}
