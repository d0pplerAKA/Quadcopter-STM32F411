#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f4xx.h"

#include "nrf24l01p/nrf24l01p.h"

#include "uart/uart.h"
#include "delay/delay.h"


#define GPIO_Pin_NSS1               GPIO_Pin_4	//PA
#define GPIO_Pin_SCK1               GPIO_Pin_3	//PB
#define GPIO_Pin_MISO1              GPIO_Pin_4	//PB
#define GPIO_Pin_MOSI1              GPIO_Pin_5	//PB

#define GPIO_Pin_CE                 GPIO_Pin_14 //PC 14 test
#define GPIO_Pin_IRQ                GPIO_Pin_15 //PA


#define PACK_Type_Control           (uint8_t) 0xCE
#define PACK_Type_Setting           (uint8_t) 0x32


extern uint8_t package_is_control_message;
extern uint8_t package_new_setting;


// Mounting below APB2 Peripheral bus with a clock of (100MHz / 1) -> 100MHz
void SPI1_Init(void);

void EXTI15_10_IRQHandler(void);

void NRF_Writebyte(uint8_t command, uint8_t reg_data);
void NRF_Writebytes(uint8_t command, uint8_t *reg_data, uint8_t data_len);
void NRF_Readbyte(uint8_t command, uint8_t *data_p, uint8_t dummy_byte);
void NRF_Readbytes(uint8_t command, uint8_t *data_p, uint8_t data_len, uint8_t dummy_byte);

uint8_t nrf_spi_transfer(uint8_t t_data);


#endif

