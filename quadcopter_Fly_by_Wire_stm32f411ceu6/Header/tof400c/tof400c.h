#ifndef __TOF400C_H__
#define __TOF400C_H__

#include "stm32f4xx.h"
#include "i2c/i2c.h"
#include "uart/uart.h"
#include "led/led.h"

#include "vl53l1x/core/VL53L1X_api.h"

#include "post/post.h"

#include "ms5611/ms5611.h"

#include "gl9306/gl9306.h"


#define TOF400C_IRQ_Pin                     GPIO_Pin_1          // IRQ Active Low
#define TOF400C_XSHUT_Pin                   GPIO_Pin_2

#define TOF400C_ON()                        GPIO_SetBits(GPIOA, TOF400C_XSHUT_Pin)
#define TOF400C_OFF()                       GPIO_ResetBits(GPIOA, TOF400C_XSHUT_Pin)


#define TOF400C_I2C_Address                 0x52
#define TOF400C_Sensor_ID                   0xEACC

#define TOF_SUCCESS                         0x00
#define TOF_FAIL                            0x01


//detection mode
#define TOF_MODE_DEFAULT                    0	//default,see manul 5.3.1
#define TOF_MODE_HIGH_ACCURACY              1
#define TOF_MODE_LONG_RANGE                 2
#define TOF_MODE_HIGH_SPEED                 3

#define TOF_IRQ_Install                     SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1)

#define TOF_FIFO_Size                       6
#define TOF_FirstOrder_Coff                 (float) 0.975f


typedef struct
{
    uint8_t onLoop;
    float height_distance_in_cm;

} TOF400C_t;



void TOF_GPIO_Init(void);
void TOF_Init(void);
void TOF_OnLoopTrigger(void);
void TOF_EXTILine_Cmd(uint8_t state);
float TOF_Height(void);

#endif
