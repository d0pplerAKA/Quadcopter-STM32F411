#ifndef __POST_H__
#define __POST_H__

#include "stm32f4xx.h"

#include "uart/uart.h"
#include "delay/delay.h"

#include "timer/timer.h"

#include "bl_motor/bl_motor.h"

#include "i2c/i2c.h"
#include "gy_95t/gy_95t.h"
#include "ms5611/ms5611.h"
#include "tof400c/tof400c.h"

#include "adc_voltage/adc_voltage.h"

#include "nrf24l01p/nrf24l01p.h"
#include "xbox/xbox.h"

#include "gl9306/gl9306.h"


#define Drone_PWR_RESET                     0xE0

#define Drone_Calibration_Samples           65

#define Drone_POST_Pin                      GPIO_Pin_15


typedef struct
{
    float offset_roll;
    float offset_pitch;

    float offset_yaw;
    /* data */
}POST_OFFSET_t;


extern POST_OFFSET_t Drone_offset;


uint8_t Drone_POST(void);
void Drone_Init(uint8_t post_state);
void Drone_Sensor_Init(void);
void Drone_Altimeter_Init(void);
void Drone_Baroceptor_Init(void);
void Drone_IMU_Init(void);
void Drone_POST_GPIO_Init(void);
void Drone_Get_Channel(uint8_t *channel);
void Drone_data_calibration(POST_OFFSET_t *drone_offset);
void Drone_Offsets_toString(void);


void Drone_NVIC_Init(void);
void Drone_NVIC_State(uint8_t state);


#endif
