#include "bl_motor.h"


BLDC_Motor_t Drone_Motor;
volatile float throttle_val = 0.0f;
volatile float yaw_throttle = 0.0f;


void BLDC_Init(void)
{
    GPIO_InitTypeDef GPIO_S;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | 
                            RCC_AHB1Periph_GPIOB, 
                            ENABLE);
    
    GPIO_S.GPIO_Mode = GPIO_Mode_AF;
    GPIO_S.GPIO_OType = GPIO_OType_PP;
    GPIO_S.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_S.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_S.GPIO_Pin = GPIO_BL_MOTOR_L1 | GPIO_BL_MOTOR_R1;

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
    GPIO_Init(GPIOA, &GPIO_S);

    GPIO_S.GPIO_Pin = GPIO_BL_MOTOR_L2 | GPIO_BL_MOTOR_R2;

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
    GPIO_Init(GPIOB, &GPIO_S);
}

void BLDC_L1_SetCompare(uint32_t value)
{
    if(value > BLDC_Max_Speed)
        TIM_SetCompare1(TIM_BLDC, BLDC_Max_Speed);
    else if(value < BLDC_Min_Speed)
        TIM_SetCompare1(TIM_BLDC, BLDC_Min_Speed);
    else
        TIM_SetCompare1(TIM_BLDC, value);
}

void BLDC_R1_SetCompare(uint32_t value)
{
    if(value > BLDC_Max_Speed)
        TIM_SetCompare2(TIM_BLDC, BLDC_Max_Speed);
    else if(value < BLDC_Min_Speed)
        TIM_SetCompare2(TIM_BLDC, BLDC_Min_Speed);
    else
        TIM_SetCompare2(TIM_BLDC, value);
}

void BLDC_L2_SetCompare(uint32_t value)
{
    if(value > BLDC_Max_Speed)
        TIM_SetCompare3(TIM_BLDC, BLDC_Max_Speed);
    else if(value < BLDC_Min_Speed)
        TIM_SetCompare3(TIM_BLDC, BLDC_Min_Speed);
    else
        TIM_SetCompare3(TIM_BLDC, value);
}

void BLDC_R2_SetCompare(uint32_t value)
{
    if(value > BLDC_Max_Speed)
        TIM_SetCompare4(TIM_BLDC, BLDC_Max_Speed);
    else if(value < BLDC_Min_Speed)
        TIM_SetCompare4(TIM_BLDC, BLDC_Min_Speed);
    else
        TIM_SetCompare4(TIM_BLDC, value);
}

void BLDC_SetSpeed(BLDC_Motor_t * motor_value)
{
    BLDC_L1_SetCompare(BLDC_CompareMap(motor_value->value_L1));
    BLDC_R1_SetCompare(BLDC_CompareMap(motor_value->value_R1));
    BLDC_L2_SetCompare(BLDC_CompareMap(motor_value->value_L2));
    BLDC_R2_SetCompare(BLDC_CompareMap(motor_value->value_R2));
}

void BLDC_Unlock(void)
{
    TIM_Cmd(TIM_BLDC, ENABLE);
	delay_ms(200);

    BLDC_L1_SetCompare(BLDC_Max_Speed);
	BLDC_R1_SetCompare(BLDC_Max_Speed);
	BLDC_L2_SetCompare(BLDC_Max_Speed);
	BLDC_R2_SetCompare(BLDC_Max_Speed);
	delay_ms(1500);
	
	BLDC_L1_SetCompare(BLDC_Min_Speed);
	BLDC_R1_SetCompare(BLDC_Min_Speed);
	BLDC_L2_SetCompare(BLDC_Min_Speed);
	BLDC_R2_SetCompare(BLDC_Min_Speed);
	delay_ms(100);
}

uint32_t BLDC_CompareMap(float value)
{
    float rtn_val = floatMap(value, 0.0f, 1000.0f, BLDC_Min_Speed, BLDC_Max_Speed) + 0.5f;

    return (uint32_t) rtn_val;
}

float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
{
    if(x > in_max)
        x = in_max;

    if(x < in_min)
        x = in_min;

    return (float) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

