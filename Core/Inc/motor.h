#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"
#include <stdbool.h>

typedef enum {
    CMD_IDLE,   // 0: Udrziavaj stav / Nic nerob
    CMD_OPEN,   // 1: Prikaz: Otvorit
    CMD_CLOSE,  // 2: Prikaz: Zatvorit
    CMD_STOP    // 3: Prikaz: Stop
} GarageCommand;

typedef enum {
    STATE_CLOSED,   // Garaz je zatvorena
    STATE_OPENING,  // Prave sa otvara
    STATE_OPEN,     // Garaz je otvorena
    STATE_CLOSING   // Prave sa zatvara
} GarageState;

void Motor_Init(void);                      // Nastavenie PWM
void Motor_Process(GarageCommand inputCmd); // Hlavna slucka (logika + pohyb)
GarageState Motor_GetState(void);           // Ak by main chcel vediet stav

#endif
