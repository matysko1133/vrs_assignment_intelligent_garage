#include "sensors.h"
#include "tim.h"

// globalne premenne
uint32_t dist_in  = 0;
uint32_t dist_out = 0;
CarPosition car_pos = CAR_NONE;

/* funkcia pre casovac */
static void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}

uint32_t HCSR04_Read_Distance(GPIO_TypeDef *TRIG_PORT, uint16_t TRIG_PIN,
                             GPIO_TypeDef *ECHO_PORT, uint16_t ECHO_PIN)
{
    uint32_t time = 0;

    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_RESET);
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET)
    {
        time++;
        delay_us(1);
    }

    return time / 58;
}

void Sensor_Init(void)
{
    HAL_TIM_Base_Start(&htim2);
    car_pos = CAR_NONE;
}

void Sensor_Update(void)
{
    dist_in = HCSR04_Read_Distance(IN_TRIG_PORT, IN_TRIG_PIN, IN_ECHO_PORT, IN_ECHO_PIN);
    HAL_Delay(20);
    dist_out = HCSR04_Read_Distance(OUT_TRIG_PORT, OUT_TRIG_PIN, OUT_ECHO_PORT, OUT_ECHO_PIN);

    uint8_t inside = (dist_in > 0 && dist_in < DISTANCE_THRESHOLD);
    uint8_t outside = (dist_out > 0 && dist_out < DISTANCE_THRESHOLD);

    if (inside && outside) {
        car_pos = CAR_BOTH;
    } else if (inside) {
        car_pos = CAR_INSIDE;
    } else if (outside) {
        car_pos = CAR_OUTSIDE;
    } else {
        car_pos = CAR_NONE;
    }
}

CarPosition Sensor_GetCarPosition(void)
{
    return car_pos;
}
