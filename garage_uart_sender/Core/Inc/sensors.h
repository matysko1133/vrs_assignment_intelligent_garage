/* sensors.h */
#ifndef SENSORS_H
#define SENSORS_H

#include "main.h"

#define IN_TRIG_PORT   GPIOA
#define IN_TRIG_PIN    GPIO_PIN_3
#define IN_ECHO_PORT   GPIOB
#define IN_ECHO_PIN    GPIO_PIN_3

#define OUT_TRIG_PORT  GPIOB
#define OUT_TRIG_PIN   GPIO_PIN_5
#define OUT_ECHO_PORT  GPIOB
#define OUT_ECHO_PIN   GPIO_PIN_4

#define DISTANCE_THRESHOLD 3

typedef enum {
    CAR_NONE,
    CAR_INSIDE,
    CAR_OUTSIDE,
    CAR_BOTH
} CarPosition;

void Sensor_Init(void);
void Sensor_Update(void);
CarPosition Sensor_GetCarPosition(void);

#endif
