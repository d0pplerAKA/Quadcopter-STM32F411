#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f4xx.h"

//#include "hc_sr04/hc_sr04.h"
#include "xbox/xbox.h"
#include "gy_95t/gy_95t.h"
#include "ms5611/ms5611.h"

#include "fwb_stm32f4/fwb.h"

#define OS_TICKS_PER_SEC                1000

#define TIM_Servo                       TIM2
#define TIM_BLDC                        TIM3

#define TIM_MS5611_ESP32S3              TIM4    // 100Mhz devided by 2 APB1
#define TIM_GY95T                       TIM5    // 100Mhz devided by 1 APB2

#define TIM_ADC                         TIM9

#define TIM_XBOX                        TIM10



extern uint64_t sys_tickstamp;

extern uint32_t xbox_sampling_counter;

extern float motor_startup;


void SysTick_Init(void);

void TIM_BLDC_Init(void);

void TIM_ToString_Init(void);
void TIM_MS5611_Init(void);
void TIM_GY95T_Init(void);
void TIM_ADC_Voltage_Init(void);
void TIM_XBOX_Init(void);

uint8_t xbox_sampling_cmp(void);

void Timer_Cmd(void);


#endif
