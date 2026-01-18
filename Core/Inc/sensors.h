/*
 * hcsr04.h
 *
 *  Created on: Jan 10, 2026
 *      Author: 1domi
 */

#ifndef SENSORS_H
#define SENSORS_H

#include "main.h"

/* piny senzorov */

// vnutorny senzor
#define IN_TRIG_PORT   GPIOA
#define IN_TRIG_PIN    GPIO_PIN_3
#define IN_ECHO_PORT   GPIOB
#define IN_ECHO_PIN    GPIO_PIN_3

// vonkajsi senzor
#define OUT_TRIG_PORT  GPIOB
#define OUT_TRIG_PIN   GPIO_PIN_5
#define OUT_ECHO_PORT  GPIOB
#define OUT_ECHO_PIN   GPIO_PIN_4

#define DISTANCE_THRESHOLD 40 // auto je v blizkosti senzora 40 cm

/* definicia stavov pre Carposition */
typedef enum {
    CAR_NONE,
    CAR_INSIDE,
    CAR_OUTSIDE,
    CAR_BOTH
} CarPosition;

// funkcie pre main.c
void Sensor_Init(void);
void Sensor_Update(void);
CarPosition Sensor_GetCarPosition(void);

#endif
