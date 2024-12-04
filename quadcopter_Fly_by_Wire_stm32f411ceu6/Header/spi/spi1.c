#include "spi.h"


uint8_t package_is_control_message = 0;
uint8_t package_new_setting = 0;


void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_S;
    SPI_InitTypeDef SPI_S;
    EXTI_InitTypeDef EXTI_S;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    

    //RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
    //RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);

    /*** CS ***/
    GPIO_S.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_S.GPIO_OType = GPIO_OType_PP;
    GPIO_S.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_S.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_S.GPIO_Pin = GPIO_Pin_NSS1;
    GPIO_Init(GPIOA, &GPIO_S);
    NRF_CS_HIGH();
	
	//SPI_S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    //SPI_S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_S.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_S.SPI_CPOL = SPI_CPOL_Low;
    SPI_S.SPI_CRCPolynomial = 7;
    SPI_S.SPI_DataSize = SPI_DataSize_8b;
    SPI_S.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_S.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_S.SPI_Mode = SPI_Mode_Master;
    SPI_S.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI1, &SPI_S);
    SPI_Cmd(SPI1, ENABLE);
    //SPI_SSOutputCmd(SPI1, ENABLE);

    GPIO_S.GPIO_Mode = GPIO_Mode_AF;
    GPIO_S.GPIO_OType = GPIO_OType_PP;
    GPIO_S.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_S.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_S.GPIO_Pin = GPIO_Pin_SCK1 | GPIO_Pin_MISO1 | GPIO_Pin_MOSI1;
    GPIO_Init(GPIOB, &GPIO_S);

    GPIO_SetBits(GPIOB, GPIO_Pin_MOSI1 | GPIO_Pin_MISO1 | GPIO_Pin_SCK1);

    //GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

    

    /*** CE ***/
    GPIO_S.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_S.GPIO_OType = GPIO_OType_PP;
    GPIO_S.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_S.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_S.GPIO_Pin = GPIO_Pin_CE;
    GPIO_Init(GPIOC, &GPIO_S);
    GPIO_ResetBits(GPIOC, GPIO_Pin_CE);

    /*** IRQ ***/
    GPIO_S.GPIO_Mode = GPIO_Mode_IN;
    GPIO_S.GPIO_OType = GPIO_OType_OD;
    GPIO_S.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_S.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_S.GPIO_Pin = GPIO_Pin_IRQ;
    GPIO_Init(GPIOA, &GPIO_S);

    EXTI_S.EXTI_Line = EXTI_Line15;
    EXTI_S.EXTI_LineCmd = ENABLE;
    EXTI_S.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_S.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_S);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
}

void NRF_Writebyte(uint8_t command, uint8_t reg_data)
{
    NRF_CS_LOW();

    nrf_spi_transfer(command);

    nrf_spi_transfer(reg_data);

    NRF_CS_HIGH();
}

void NRF_Writebytes(uint8_t command, uint8_t *reg_data, uint8_t data_len)
{
    NRF_CS_LOW();

    nrf_spi_transfer(command);

    for(uint8_t i = 0; i < data_len; i++)
    {
        nrf_spi_transfer(*reg_data);

        reg_data++;
    }

    NRF_CS_HIGH();
}

void NRF_Readbyte(uint8_t command, uint8_t *data_p, uint8_t dummy_byte)
{
    NRF_CS_LOW();

    nrf_spi_transfer(command);

    *data_p = nrf_spi_transfer(dummy_byte);

    NRF_CS_HIGH();
}

void NRF_Readbytes(uint8_t command, uint8_t *data_p, uint8_t data_len, uint8_t dummy_byte)
{
    NRF_CS_LOW();

    nrf_spi_transfer(command);

    for(uint8_t i = 0; i < data_len; i++)
    {
        *data_p = nrf_spi_transfer(dummy_byte);

        data_p++;
    }

    NRF_CS_HIGH();
} 


uint8_t nrf_spi_transfer(uint8_t t_data)
{
    uint8_t temp_rtn;
    
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    SPI_SendData(SPI1, t_data);

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    temp_rtn = SPI_ReceiveData(SPI1);

    return temp_rtn;
}


void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetFlagStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);

        NRF24_PRX_IRQHandler();
    }
}

/*


        //NRF_CE_LOW();

        //printf("In call back.\n");
        
        uint8_t irq_status;

        NRF24_Get_Status(&irq_status);

        printf("irq status: %d\n", irq_status);
        
        printf("FIFO: %d\n", NRF24_Get_FIFO_Status());


        irq_status = NRF24_Get_IRQ_bits();
        
        if(irq_status == NRF_IRQ_RX_FIFO_Ready)
        {
			
            //NRF_CE_LOW();

            NRF24_Unload_payload(nrf_temp_rx_ram);

            
            for(uint8_t i = 0; i < 32; i++)
            {
                printf("%d ", nrf_temp_rx_ram[i]);
            }
            printf("\n\n");

            //NRF_CE_HIGH();

            NRF24_Clear_IRQ_RX_Ready();
        }
        else
        {
            printf("Strange Error!\n");
        }
        

        for(uint8_t i = 5; i < 32 + 5; i++)
        {
            nrf_temp_tx_ram[i - 5] = pid * i / 2 + 32;
        }

        NRF24_Write_RX_ACK_payload(0, nrf_temp_tx_ram);
*/
