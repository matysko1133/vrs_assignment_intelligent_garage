/* led.h */
#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
  OPEN = 0,
  CLOSED,
  MOVEMENT_IN_PROGRESS
} led_state_t;

void ledInit(void);

void ledSetState(led_state_t state);

led_state_t ledGetState(void);

void ledProcess(void);

void ledSetBlinkPeriodMs(uint32_t period_ms);

#ifdef __cplusplus
}
#endif

#endif
