#include "motor.h"

#define SERVO_STOP  1500
#define SERVO_CW    1000  // Otvaranie
#define SERVO_CCW   2000  // Zatvaranie
#define MOVE_TIME_MS 250

// Tlacidlo
#define BTN_PORT    GPIOA
#define BTN_PIN     GPIO_PIN_11

extern TIM_HandleTypeDef htim1;
static volatile GarageState currentState = STATE_CLOSED;


static void Raw_Move_Open(void) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_CW);
    HAL_Delay(MOVE_TIME_MS);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_STOP);
}

static void Raw_Move_Close(void) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_CCW);
    HAL_Delay(MOVE_TIME_MS);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_STOP);
}



void Motor_Init(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_STOP);
    currentState = STATE_CLOSED;
}

void Motor_Process(GarageCommand inputCmd) {

    // KONTROLA TLACIDLA
    if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET) {
        HAL_Delay(50); // Debounce
        if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET) {

            if (currentState == STATE_CLOSED || currentState == STATE_CLOSING) {
                // Manualne otvorenie
                currentState = STATE_OPENING;
                Raw_Move_Open();
                currentState = STATE_OPEN;
            } else {
                // Manualne zatvorenie
                currentState = STATE_CLOSING;
                Raw_Move_Close();
                currentState = STATE_CLOSED;
            }

            while (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET);

            return;
        }
    }


    switch (inputCmd) {
        case CMD_OPEN:
            if (currentState == STATE_CLOSED) {
                currentState = STATE_OPENING;
                Raw_Move_Open();
                currentState = STATE_OPEN;
            }
            break;

        case CMD_CLOSE:
            if (currentState == STATE_OPEN) {
                currentState = STATE_CLOSING;
                Raw_Move_Close();
                currentState = STATE_CLOSED;
            }
            break;

        case CMD_IDLE:
        default:

            break;
    }
}
