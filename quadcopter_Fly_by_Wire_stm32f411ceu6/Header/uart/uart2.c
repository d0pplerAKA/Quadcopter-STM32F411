#include "uart.h"

uint32_t UART2_ReceiveSize = 0;

void UART2_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_Pin = GL9306_RX_GPIO;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStruct);

    //USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    //USART_ITConfig(USART2, USART_IT_TC, DISABLE);
    //USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
}

void UART2_DMA_Init(void)           // [Channel 4] [Stream 5]
{
    DMA_InitTypeDef DMA_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Stream5);

    while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);

    DMA_InitStruct.DMA_BufferSize = GL9306_BufferSize;
    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) GL9306_TEMP_DATA_PACK;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) (&(USART2->DR));
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA1_Stream5, &DMA_InitStruct);


    //DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
}

void USART2_IRQHandler(void)
{
    //uint8_t temp_rx;

    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        DMA_Cmd(DMA1_Stream5, DISABLE);

        //temp_rx = USART2->SR;
        //temp_rx = USART2->DR;

        USART2->SR;
        USART2->DR;

        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);

        uint8_t temp_rx_len = DMA_GetCurrDataCounter(DMA1_Stream5);

        if((temp_rx_len == GL9306_BufferSize) && 
            (GL9306_TEMP_DATA_PACK[0] == 0xFE) && 
            (GL9306_TEMP_DATA_PACK[1] == 0x04) &&
            (GL9306_TEMP_DATA_PACK[GL9306_BufferSize - 1] == 0xAA))
        {
            memcpy(GL9306_DATA_PACK, GL9306_TEMP_DATA_PACK, sizeof(uint8_t) * GL9306_BufferSize);
            GL_RefreshData();
            //printf("Valid pack.\n");
        }
        else
        {
            DMA_SetCurrDataCounter(DMA1_Stream5, GL9306_BufferSize);
            memset(GL9306_TEMP_DATA_PACK, 0, sizeof(uint8_t) * GL9306_BufferSize);
            printf("Invalid pack!\n");
        }

        DMA_Cmd(DMA1_Stream5, ENABLE);
    }
}

/*
void DMA1_Stream5_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5) != RESET)
    {
        DMA_Cmd(DMA1_Stream5, DISABLE);

        UART2_ReceiveSize = GL9306_BufferSize - DMA_GetCurrDataCounter(DMA1_Stream5);

        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | 
                                    DMA_FLAG_FEIF5 | 
                                    DMA_FLAG_DMEIF5 | 
                                    DMA_FLAG_TEIF5 | 
                                    DMA_FLAG_HTIF5);
        
        DMA_SetCurrDataCounter(DMA1_Stream5, GL9306_BufferSize);

        DMA_Cmd(DMA1_Stream5, ENABLE);
    }
}
*/
