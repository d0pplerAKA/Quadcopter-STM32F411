#ifndef __ADC_VOLTAGE_H__
#define __ADC_VOLTAGE_H__

#include "stm32f4xx.h"

#include "uart/uart.h"
#include "xbox/xbox.h"

#include "i2c/i2c.h"
#include "bl_motor/bl_motor.h"


#define GPIO_Port_ADC                   GPIOA
#define GPIO_Pin_ADC                    GPIO_Pin_0

#define ADC_Sample_Term                 5


void ADC_Voltage_Init(void);

void ADC_GPIO_Init(void);
void ADC_Parameter_Init(void);


#endif
