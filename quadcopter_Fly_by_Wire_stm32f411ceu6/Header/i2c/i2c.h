#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f4xx.h"

#include <stdio.h>

#include "delay/delay.h"


/*
*   
*   PS -> GND
*   IO -> INT
*/

#include "stm32f4xx.h"

#include "uart/uart.h"

// Mounting below APB1 Peripheral bus with a clock of (100MHz / 2) -> 50MHz
#define I2C_PORT_GY                         I2C1
#define I2C_PORT_MS_ESP                     I2C2
#define I2C_PORT_TOF                        I2C3


#define GPIO_PIN_SCL1                       GPIO_Pin_6          // PB
#define GPIO_PIN_SDA1                       GPIO_Pin_7          // PB

#define GPIO_PIN_SCL2                       GPIO_Pin_10         // PB10
#define GPIO_PIN_SDA2                       GPIO_Pin_9          // PB9

#define GPIO_PIN_SCL3                       GPIO_Pin_8          // PA8
#define GPIO_PIN_SDA3                       GPIO_Pin_8          // PB8


#define I2C_Timeout                         (uint16_t ) 500

#define NO_ERROR                            0
#define ERROR_EV5                           5
#define ERROR_EV6                           6
#define ERROR_EV7                           7
#define ERROR_EV8                           8
#define ERROR_EV8_2                         82
#define ERROR_BUSY                          9

#define I2C_Fail                            0xCE
#define I2C_Success                         0x00


#define I2C_Esp32Soc_Address                I2C_Fail

#define I2C_Esp32Soc_SoftReset              (uint8_t) 0x32
#define I2C_Esp32Soc_Read_Operation         (uint8_t) 0x35

#define I2C_Esp32Soc_Register_NRF_Channel   (uint8_t) 0x02
#define I2C_Esp32Soc_Register_WS2812_Mode   (uint8_t) 0x03
#define I2C_Esp32Soc_Register_Battery       (uint8_t) 0x04

#define I2C_Esp32Soc_WS2812_Mode0           (uint8_t) 0x00
#define I2C_Esp32Soc_WS2812_Mode1           (uint8_t) 0x01
#define I2C_Esp32Soc_WS2812_Mode2           (uint8_t) 0x02
#define I2C_Esp32Soc_WS2812_Mode3           (uint8_t) 0x03
#define I2C_Esp32Soc_WS2812_Mode4           (uint8_t) 0x04
#define I2C_Esp32Soc_WS2812_Mode5           (uint8_t) 0x05
#define I2C_Esp32Soc_WS2812_Mode6           (uint8_t) 0x06
#define I2C_Esp32Soc_WS2812_Mode7           (uint8_t) 0x07






// I2C1 - GY95T
void I2C1_Init(void);

// I2C2 - MS5611 / ESP32S3
void I2C2_Init(void);
uint8_t I2C2_WriteOnlyReg(uint8_t sl_addr, uint8_t byte_write);
uint8_t I2C2_ReadBytes_Direct(uint8_t sl_addr, uint8_t *byte_buffer_receive);
uint8_t I2C2_ReadByte_Arduino(uint8_t sl_addr, uint8_t *byte_read);
uint8_t I2C_RequestFrom_EspSoc(uint8_t reg_addr, uint8_t *byte_read);
uint8_t I2C_SendTo_EspSoc(uint8_t reg_addr, uint8_t byte_write);

// I2C3 - TOF400C
void I2C3_Init(void);
uint8_t I2C3_TOF_WriteByte(uint8_t sl_addr, uint16_t reg_addr, uint8_t byte_write);
uint8_t I2C3_TOF_ReadByte(uint8_t sl_addr, uint16_t reg_addr, uint8_t *byte_receive);
uint8_t I2C3_TOF_WriteBytes(uint8_t sl_addr, uint16_t reg_addr, uint8_t *byte_buffer_write, uint32_t byte_num);
uint8_t I2C3_TOF_ReadBytes(uint8_t sl_addr, uint16_t reg_addr, uint8_t *byte_buffer_receive, uint32_t byte_num);

// I2C1 I2C2 I2C3
void I2Cs_Init(void);

uint8_t I2C_WriteByte(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t byte_write);
uint8_t I2C_WriteBytes(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t *byte_buffer_write, uint32_t byte_num);
uint8_t I2C_ReadByte(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t *byte_receive);
uint8_t I2C_ReadBytes(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t *byte_buffer_receive, uint32_t byte_num);


#endif
