/* time_sync.h */
#ifndef TIMESYNC_H
#define TIMESYNC_H

#include "main.h"
#include "stm32f3xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void TimeSync_Init(void);

void TimeSync_Task(void);

void TimeSync_UART_RxCpltCallback(UART_HandleTypeDef *huart);

uint8_t TimeSync_IsSynced(void);

#ifdef __cplusplus
}
#endif

#endif
