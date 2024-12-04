#include "hc_sr04.h"


uint32_t HC_SR04_Flag = 0x0;


void HC_SR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Trig;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Echo;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    Trig_L;
}

void HC_SR04_Distance(float *distance_cm)
{
    Trig_H;
    delay_i2c(15);
    Trig_L;
	
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Echo) != 1); //ECHO 引脚为低的时候为空闲状态，为高时跳出循环，即收到返回信号
    TIM_Cmd(TIM_HC_SR04, ENABLE);
	
	
    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Echo) != 0); //ECHO 引脚变为低时返回信号结束
    TIM_Cmd(TIM_HC_SR04, DISABLE);

    uint32_t counter = HC_SR04_Flag;
    HC_SR04_Flag = 0x0;

    if(counter != 0)
        *distance_cm = (float) counter * 0.000346f * 0.5f * 100.0f;
    else
        *distance_cm = -1.0f;
}

float HC_SR04_Distance_Return(void)
{
    float temp_dis;
    HC_SR04_Distance(&temp_dis);

    return temp_dis;
}
