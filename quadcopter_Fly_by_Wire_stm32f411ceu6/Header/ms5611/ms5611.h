#ifndef __MS5611_H__
#define __MS5611_H__

#include "stm32f4xx.h"

#include <stdlib.h>
#include <math.h>

#include "i2c/i2c.h"
#include "timer/timer.h"
#include "delay/delay.h"

#include "uart/uart.h"
#include "gy_95t/gy_95t.h"

#include "post/post.h"

/*
*
*   在3000米范围内，高度每增加 9 米
*   气压大约减少 100 pa
*
*   即，高度增加 1 米，气压减小 11.11 pa
*   即，高度增加 10 厘米，气压减小 1.11 pa
*
*/

#define MS5611_ADDR                         0xEE

#define MS5611_Reset                        0x1E
#define MS5611_Read_ADC                     0x00

//#define MS5611_Read_D1                      0x40
//#define MS5611_Read_D2                      0x50

#define MS5611_D1_OSR_256                   0x40
#define MS5611_D1_OSR_512                   0x42
#define MS5611_D1_OSR_1024                  0x44
#define MS5611_D1_OSR_2048                  0x46
#define MS5611_D1_OSR_4096                  0x48

#define MS5611_D2_OSR_256                   0x50
#define MS5611_D2_OSR_512                   0x52
#define MS5611_D2_OSR_1024                  0x54
#define MS5611_D2_OSR_2048                  0x56
#define MS5611_D2_OSR_4096                  0x58

#define MS5611_Read_PROM_C0                 0xA0        //0 1010 000 0
#define MS5611_Read_PROM_C1                 0xA2        //1 1010 001 0
#define MS5611_Read_PROM_C2                 0xA4        //2 1010 010 0
#define MS5611_Read_PROM_C3                 0xA6        //3 1010 011 0
#define MS5611_Read_PROM_C4                 0xA8        //4 1010 100 0
#define MS5611_Read_PROM_C5                 0xAA        //5 1010 101 0
#define MS5611_Read_PROM_C6                 0xAC        //6 1010 110 0

#define MS5611_Read_PROM_CRC                0xAE        //7 1010 111 0

#define MS5611_D1_OSR                       MS5611_D1_OSR_4096
#define MS5611_D2_OSR                       MS5611_D2_OSR_4096



#define MS5611_Standard_Bar                 (float) 101.325f
#define MS5611_Kelvin_Temp                  (float) 273.15f
#define MS5611_Powf_exponent1               (float) 0.190223f
#define MS5611_Powf_exponent2               (float) 0.190295f

#define MS5611_Sampling_FIFO_Size           (uint8_t) 7

#define MS5611_FirstOrder_PressCoff         (float) 0.25f
#define MS5611_FirstOrder_HeightCoff        (float) 0.55f






/*
* 
*/


typedef struct
{
    uint16_t prom_Reserve;
    uint16_t prom_C1;
    uint16_t prom_C2;
    uint16_t prom_C3;
    uint16_t prom_C4;
    uint16_t prom_C5;
    uint16_t prom_C6;

    uint16_t prom_CRC;
} MS5611_PROM;

typedef struct
{
    int32_t data_dT;
    uint32_t data_D2_temp;
    int64_t data_Temperature;

    uint32_t data_D1_pres;
    int64_t data_OFF;
    int64_t data_SENS;
    int64_t data_Pressure;

    int64_t data_dT2;
    int64_t data_OFF2;
    int64_t data_SENS2;
    int64_t data_Temperature2;

    float data_Temperature_inFloat;
    float data_Pressure_inFloat;

    float data_height;
} MS5611_Data;

extern MS5611_Data ms5611_data;

extern float data_without_filtering;


uint8_t MS_Init(void);
void MS_Reset(void);
uint8_t MS_ReadPROM(void);
uint8_t MS_Convert(uint8_t order, uint8_t ms_delay, uint32_t *result);
uint8_t MS_Temp(void);
uint8_t MS_Pres(void);
uint8_t MS_RefreshData(void);

void MS_toString(void);
int32_t MS_Temperature_int32_t(void);
float MS_Temperature(void);
float Fusion_Temp(void);
float MS_Pressure(void);
float MS_Height(void);
float MS_Height_Hypsometric(void);
float MS_Height_Barometric(void);

float MS5611_firstOrder_Filtering(float x_data, float coff, float y_data);

uint8_t MS5611_CRC(uint16_t *prom);


#endif
