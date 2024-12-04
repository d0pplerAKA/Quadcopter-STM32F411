#ifndef __HC_SR04_H__
#define __HC_SR04_H__

#include "stm32f4xx.h"

#include "delay/delay.h"
#include "timer/timer.h"
#include "uart/uart1.h"



#define GPIO_Trig       GPIO_Pin_8
#define GPIO_Echo       GPIO_Pin_9

#define Trig_H          GPIO_SetBits(GPIOB, GPIO_Trig)
#define Trig_L          GPIO_ResetBits(GPIOB, GPIO_Trig)


extern uint32_t HC_SR04_Flag;


void HC_SR04_Init(void);
void HC_SR04_Distance(float *distance_cm);
float HC_SR04_Distance_Return(void);


#endif
