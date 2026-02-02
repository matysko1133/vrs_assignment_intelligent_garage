/* main.h */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"

void Error_Handler(void);

#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOA
#define TRIG_IN_Pin GPIO_PIN_3
#define TRIG_IN_GPIO_Port GPIOA
#define SD_CS_Pin GPIO_PIN_4
#define SD_CS_GPIO_Port GPIOA
#define SD_SCK_Pin GPIO_PIN_5
#define SD_SCK_GPIO_Port GPIOA
#define SD_MISO_Pin GPIO_PIN_6
#define SD_MISO_GPIO_Port GPIOA
#define SD_MOSI_Pin GPIO_PIN_7
#define SD_MOSI_GPIO_Port GPIOA
#define SERVO_M_Pin GPIO_PIN_8
#define SERVO_M_GPIO_Port GPIOA
#define MAN_BTN_Pin GPIO_PIN_11
#define MAN_BTN_GPIO_Port GPIOA
#define ECHO_IN_Pin GPIO_PIN_3
#define ECHO_IN_GPIO_Port GPIOB
#define ECHO_OUT_Pin GPIO_PIN_4
#define ECHO_OUT_GPIO_Port GPIOB
#define TRIG_OUT_Pin GPIO_PIN_5
#define TRIG_OUT_GPIO_Port GPIOB
#define LCD_SCL_Pin GPIO_PIN_6
#define LCD_SCL_GPIO_Port GPIOB
#define LCD_SDA_Pin GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif
