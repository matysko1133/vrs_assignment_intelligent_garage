/* stm32f3xx_hal_conf.h */
#ifndef __STM32F3xx_HAL_CONF_H
#define __STM32F3xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#define HAL_MODULE_ENABLED

#define HAL_UART_MODULE_ENABLED

#define HAL_GPIO_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED

#define HAL_DMA_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED

#if !defined  (HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)8000000)
#endif

#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    ((uint32_t)100)
#endif

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)8000000)
#endif

#if !defined  (HSI_STARTUP_TIMEOUT)
 #define HSI_STARTUP_TIMEOUT   ((uint32_t)5000)
#endif

#if !defined  (LSI_VALUE)
 #define LSI_VALUE  ((uint32_t)40000)
#endif

#if !defined  (LSE_VALUE)
 #define LSE_VALUE  ((uint32_t)32768)
#endif

#if !defined  (LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT    ((uint32_t)5000)
#endif

#if !defined  (EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE    ((uint32_t)8000000)
#endif

#define  VDD_VALUE                   ((uint32_t)3300)
#define  TICK_INT_PRIORITY            ((uint32_t)15)
#define  USE_RTOS                     0
#define  PREFETCH_ENABLE              1
#define  INSTRUCTION_CACHE_ENABLE     0
#define  DATA_CACHE_ENABLE            0
#define USE_SPI_CRC                     0U

#define  USE_HAL_ADC_REGISTER_CALLBACKS         0U
#define  USE_HAL_CAN_REGISTER_CALLBACKS         0U
#define  USE_HAL_COMP_REGISTER_CALLBACKS        0U
#define  USE_HAL_CEC_REGISTER_CALLBACKS         0U
#define  USE_HAL_DAC_REGISTER_CALLBACKS         0U
#define  USE_HAL_SRAM_REGISTER_CALLBACKS        0U
#define  USE_HAL_SMBUS_REGISTER_CALLBACKS       0U
#define  USE_HAL_SDADC_REGISTER_CALLBACKS       0U
#define  USE_HAL_NAND_REGISTER_CALLBACKS        0U
#define  USE_HAL_NOR_REGISTER_CALLBACKS         0U
#define  USE_HAL_PCCARD_REGISTER_CALLBACKS      0U
#define  USE_HAL_HRTIM_REGISTER_CALLBACKS       0U
#define  USE_HAL_I2C_REGISTER_CALLBACKS         0U
#define  USE_HAL_UART_REGISTER_CALLBACKS        0U
#define  USE_HAL_USART_REGISTER_CALLBACKS       0U
#define  USE_HAL_IRDA_REGISTER_CALLBACKS        0U
#define  USE_HAL_SMARTCARD_REGISTER_CALLBACKS   0U
#define  USE_HAL_WWDG_REGISTER_CALLBACKS        0U
#define  USE_HAL_OPAMP_REGISTER_CALLBACKS       0U
#define  USE_HAL_RTC_REGISTER_CALLBACKS         0U
#define  USE_HAL_SPI_REGISTER_CALLBACKS         0U
#define  USE_HAL_I2S_REGISTER_CALLBACKS         0U
#define  USE_HAL_TIM_REGISTER_CALLBACKS         0U
#define  USE_HAL_TSC_REGISTER_CALLBACKS         0U
#define  USE_HAL_PCD_REGISTER_CALLBACKS         0U

#ifdef HAL_RCC_MODULE_ENABLED
 #include "stm32f3xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
 #include "stm32f3xx_hal_gpio.h"
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
  #include "stm32f3xx_hal_exti.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f3xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
 #include "stm32f3xx_hal_cortex.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
 #include "stm32f3xx_hal_adc.h"
#endif

#ifdef HAL_CAN_MODULE_ENABLED
 #include "stm32f3xx_hal_can.h"
#endif

#ifdef HAL_CAN_LEGACY_MODULE_ENABLED
  #include "stm32f3xx_hal_can_legacy.h"
#endif

#ifdef HAL_CEC_MODULE_ENABLED
 #include "stm32f3xx_hal_cec.h"
#endif

#ifdef HAL_COMP_MODULE_ENABLED
 #include "stm32f3xx_hal_comp.h"
#endif

#ifdef HAL_CRC_MODULE_ENABLED
 #include "stm32f3xx_hal_crc.h"
#endif

#ifdef HAL_DAC_MODULE_ENABLED
 #include "stm32f3xx_hal_dac.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
 #include "stm32f3xx_hal_flash.h"
#endif

#ifdef HAL_SRAM_MODULE_ENABLED
  #include "stm32f3xx_hal_sram.h"
#endif

#ifdef HAL_NOR_MODULE_ENABLED
  #include "stm32f3xx_hal_nor.h"
#endif

#ifdef HAL_NAND_MODULE_ENABLED
  #include "stm32f3xx_hal_nand.h"
#endif

#ifdef HAL_PCCARD_MODULE_ENABLED
  #include "stm32f3xx_hal_pccard.h"
#endif

#ifdef HAL_HRTIM_MODULE_ENABLED
 #include "stm32f3xx_hal_hrtim.h"
#endif

#ifdef HAL_I2C_MODULE_ENABLED
 #include "stm32f3xx_hal_i2c.h"
#endif

#ifdef HAL_I2S_MODULE_ENABLED
 #include "stm32f3xx_hal_i2s.h"
#endif

#ifdef HAL_IRDA_MODULE_ENABLED
 #include "stm32f3xx_hal_irda.h"
#endif

#ifdef HAL_IWDG_MODULE_ENABLED
 #include "stm32f3xx_hal_iwdg.h"
#endif

#ifdef HAL_OPAMP_MODULE_ENABLED
 #include "stm32f3xx_hal_opamp.h"
#endif

#ifdef HAL_PCD_MODULE_ENABLED
 #include "stm32f3xx_hal_pcd.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
 #include "stm32f3xx_hal_pwr.h"
#endif

#ifdef HAL_RTC_MODULE_ENABLED
 #include "stm32f3xx_hal_rtc.h"
#endif

#ifdef HAL_SDADC_MODULE_ENABLED
 #include "stm32f3xx_hal_sdadc.h"
#endif

#ifdef HAL_SMARTCARD_MODULE_ENABLED
 #include "stm32f3xx_hal_smartcard.h"
#endif

#ifdef HAL_SMBUS_MODULE_ENABLED
 #include "stm32f3xx_hal_smbus.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
 #include "stm32f3xx_hal_spi.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
 #include "stm32f3xx_hal_tim.h"
#endif

#ifdef HAL_TSC_MODULE_ENABLED
 #include "stm32f3xx_hal_tsc.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
 #include "stm32f3xx_hal_uart.h"
#endif

#ifdef HAL_USART_MODULE_ENABLED
 #include "stm32f3xx_hal_usart.h"
#endif

#ifdef HAL_WWDG_MODULE_ENABLED
 #include "stm32f3xx_hal_wwdg.h"
#endif

#ifdef  USE_FULL_ASSERT

  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))

  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif
