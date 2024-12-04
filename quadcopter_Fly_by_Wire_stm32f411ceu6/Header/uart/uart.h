#ifndef __UART1_H__
#define __UART1_H__

#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"

#include "gl9306/gl9306.h"


void UART1_Init(uint32_t baudrate);
void USART1_IRQhandler(void);

void UART2_Init(uint32_t baudrate);
void UART2_DMA_Init(void);           // [Channel 4] [Stream 5]
void USART2_IRQHandler(void);


#endif
