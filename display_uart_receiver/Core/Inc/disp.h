/* disp.h */
#ifndef DISP_H
#define DISP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"
#include "led.h"

#define LCD_COLS 16
#define LCD_ROWS 2

HAL_StatusTypeDef dispInit(I2C_HandleTypeDef *hi2c);

void dispSetState(led_state_t state);

void dispPrint2(const char *line0, const char *line1);

uint8_t dispGetAddr7bit(void);

#ifdef __cplusplus
}
#endif

#endif
