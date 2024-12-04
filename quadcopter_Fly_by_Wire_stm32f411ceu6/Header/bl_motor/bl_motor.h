#ifndef __BL_MOTOR_H__
#define __BL_MOTOR_H__

#include "stm32f4xx.h"
#include "delay/delay.h"
#include "timer/timer.h"


#define GPIO_BL_MOTOR_L1                GPIO_Pin_6      //PA
#define GPIO_BL_MOTOR_R1                GPIO_Pin_7      //PA
#define GPIO_BL_MOTOR_L2                GPIO_Pin_0      //PB
#define GPIO_BL_MOTOR_R2                GPIO_Pin_1      //PB

/*
    Sequence:

    6   PA  BLDC0   U3  Compare1
    7   PA  BLDC1   U4  Compare2
    0   PB  BLDC2   U5  Compare3
    1   PB  BLDC3   U6  Compare4
*/


#define BLDC_Max_Speed                  (uint32_t) 2005
#define BLDC_Min_Speed                  (uint32_t) 995

#define BLDC_BaseSpeed                  115



typedef struct
{
    float value_L1;
    float value_L2;

    float value_R1;
    float value_R2;
    /* data */
} BLDC_Motor_t;


extern BLDC_Motor_t Drone_Motor;
extern volatile float throttle_val;



void BLDC_Init(void);
void BLDC_L1_SetCompare(uint32_t value);
void BLDC_R1_SetCompare(uint32_t value);
void BLDC_L2_SetCompare(uint32_t value);
void BLDC_R2_SetCompare(uint32_t value);

void BLDC_SetSpeed(BLDC_Motor_t * motor_value);

void BLDC_Unlock(void);

uint32_t BLDC_CompareMap(float value);
float floatMap(float x, float in_min, float in_max, float out_min, float out_max);

#endif
