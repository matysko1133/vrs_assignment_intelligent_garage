/* motor.h */
#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"
#include <stdbool.h>

typedef enum {
    CMD_IDLE,
    CMD_OPEN,
    CMD_CLOSE,
    CMD_STOP
} GarageCommand;

typedef enum {
    STATE_CLOSED,
    STATE_OPENING,
    STATE_OPEN,
    STATE_CLOSING
} GarageState;

void Motor_Init(void);
void Motor_Process(GarageCommand inputCmd);
GarageState Motor_GetState(void);

uint8_t Motor_LastMoveWasManual(void);

#endif
