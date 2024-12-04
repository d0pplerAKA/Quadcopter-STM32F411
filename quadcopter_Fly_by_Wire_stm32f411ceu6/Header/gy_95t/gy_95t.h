#ifndef __GY_95T_H__
#define __GY_95T_H__

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#include "i2c/i2c.h"
#include "timer/timer.h"
#include "delay/delay.h"

#include "uart/uart.h"
#include "ms5611/ms5611.h"

#include "post/post.h"


__packed typedef struct
{
    /* data */
    int16_t raw_accel_x;
    int16_t raw_accel_y;
    int16_t raw_accel_z;
    int16_t raw_gyro_x;
    int16_t raw_gyro_y;
    int16_t raw_gyro_z;
    int16_t raw_roll;
    int16_t raw_pitch;
    int16_t raw_yaw;
    uint8_t raw_mfc; //magnetic field calibration
    int16_t raw_temp;

} GY_RAW;

typedef struct 
{
    /* data */
    float mpu_accel_x;
    float mpu_accel_y;
    float mpu_accel_z;
    float mpu_gyro_x;
    float mpu_gyro_y;
    float mpu_gyro_z;
    float mpu_roll;
    float mpu_pitch;
    float mpu_yaw;
    float mpu_temp;

    uint8_t mpu_mfc;                     // Magnetic field calibration
} GY_95T;

#define GY95T_ADDR                      0x52
#define GY95T_ACC_X_L                   0x08
#define GY95T_ROLL_L                    0x14
#define GY95T_INFO                      0x2B

#define GY95T_UPDATE_RATE               0x02
#define GY95T_OUTPUT_MODE               0x03
#define GY95T_OUTPUT_FORMAT             0x04
#define GY95T_SAVE_SETTING              0x05
#define GY95T_CALIBRATION               0x06
//#define GY95T_MODE_SET                  0x07

#define GY95T_UPDATE_RATE_SET           0x03        //200Hz

#define GY95T_OUTPUT_MODE_IRQ           0x00        //持续输出
#define GY95T_OUTPUT_MODE_DEFAULT       0x01        //轮询输出

#define GY96T_OUTPUT_FORMAT_HEX         0x00        //16进制输出格式

/*** 0x05 ***/
#define GY95T_SETTING_SAVE              0x55        //保存设置
#define GY95T_SETTING_FACT_RESET        0xAA        //恢复出厂设置
#define GY95T_SETTING_AUTO_CALIB        0x57        //自动校准
#define GY95T_SETTING_START_CALIB       0x58        //磁场校准开始
#define GY95T_SETTING_STOP_CALIB        0x59        //磁场校准结束
#define GY95T_SETTING_SAVE_CALIB        0x5A        //保存磁场校准数据



#define GY95T_FIFO_SIZE                 6


uint8_t GY_Init(void);
uint8_t GY_RefreshData(void);
float GY_Accel_x(void);
float GY_Accel_y(void);
float GY_Accel_z(void);
float GY_Gyro_x(void);
float GY_Gyro_y(void);
float GY_Gyro_z(void);
float GY_Roll(void);
float GY_Pitch(void);
float GY_Yaw(void);
uint8_t GY_MFC(void);
float GY_Temp(void);

void GY_toString(void);


#endif
