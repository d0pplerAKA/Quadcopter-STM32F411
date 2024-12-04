#include "post.h"

char rcc_rst_type[7][8] = {"BORRST", "PINRST", "PORRST", "SFTRST", 
                        "IWDGRST", "WWDGRST", "LPWRRST"};

POST_OFFSET_t Drone_offset;

// POST -> Power on self-test
uint8_t Drone_POST(void)
{
    FlagStatus rcc_status;

    uint8_t RCC_CSR = 0x00;

    for(uint8_t i = 0x79; i < 0x80; i++)
    {
        rcc_status = RCC_GetFlagStatus(i);

        RCC_CSR |= rcc_status;
        RCC_CSR <<= 1;

        delay_ms(5);
    }

    RCC_ClearFlag();

    return RCC_CSR;
}

void Drone_Init(uint8_t post_state)
{
    Drone_Motor.value_L1 = 0;
    Drone_Motor.value_L2 = 0;
    Drone_Motor.value_R1 = 0;
    Drone_Motor.value_R2 = 0;

    Drone_offset.offset_roll = 0;
    Drone_offset.offset_pitch = 0;
    Drone_offset.offset_yaw = 0;

    if(post_state == Drone_PWR_RESET)
    {
        printf("Hardware reset! Load BLDC. \n");
		
        BLDC_Init();
        TIM_BLDC_Init();
        BLDC_Unlock();

        BLDC_SetSpeed(&Drone_Motor);
        printf("Brushless Motor Init. Set speed to 0. \n");

        I2Cs_Init();
		printf("I2Cs init.\n");

        TIM_ToString_Init();
        printf("Data collection irq init\n");
		
        ADC_Voltage_Init();

		Drone_Sensor_Init();

        delay_ms(500);

        while(GPIO_ReadInputDataBit(GPIOC, Drone_POST_Pin) == SET)
        {
            printf("Waiting for ESP32 Soc Ready.\n");
            delay_ms(500);
        }
        
        printf("ESP32S3 Soc Ready. \n");

        uint8_t temp_channel = 207;

        while(temp_channel == 207)
        {
            Drone_Get_Channel(&temp_channel);
            delay_ms(50);
            Drone_Get_Channel(&temp_channel);
            delay_ms(50);
        }

        printf("Channel Got: %d\n", temp_channel);
        
        SPI1_Init();
        printf("SPI init.\n");

        NRF24_Init(NRF_PRX);
        NRF24_Set_RF_Channel(temp_channel);
        printf("NRF24 init.\n");
        
		printf("Init over, setup done!\n\n\n");	
        NRF24_PowerUp();
        delay_ms(50);
        NRF_CE_HIGH();

        printf("HARDWARE RESET\n");
    }
    else
    {
        printf("Software reset! BLDC is still in the OPEN STATE!\n");

        BLDC_Init();
        TIM_BLDC_Init();

        BLDC_SetSpeed(&Drone_Motor);
        printf("Brushless Motor Init. Set speed to 0. \n");

        I2Cs_Init();
		printf("I2Cs init.\n");

        TIM_ToString_Init();
        printf("Data collection irq init\n");
				
		ADC_Voltage_Init();

        Drone_Sensor_Init();

        delay_ms(500);

        while(GPIO_ReadInputDataBit(GPIOC, Drone_POST_Pin) == SET)
        {
            printf("Waiting for ESP32 Soc Ready.\n");
            delay_ms(500);
        }
        
        printf("ESP32S3 Soc Ready. \n");

        uint8_t temp_channel = 207;

        while(temp_channel == 207)
        {
            Drone_Get_Channel(&temp_channel);
            delay_ms(50);
            Drone_Get_Channel(&temp_channel);
            delay_ms(50);
        }

        printf("Channel Got: %d\n", temp_channel);

        SPI1_Init();
        printf("SPI init.\n");

        NRF24_Init(NRF_PRX);
        NRF24_Set_RF_Channel(temp_channel);
        printf("NRF24 init.\n");

		printf("Init over, setup done!\n\n\n");	
        NRF24_PowerUp();
        delay_ms(50);
        NRF_CE_HIGH();
    
        printf("SOFTWARE RESET\n");
    }
}

void Drone_Sensor_Init(void)
{
    Drone_POST_GPIO_Init();
    Drone_Altimeter_Init();
    Drone_Baroceptor_Init();
    Drone_IMU_Init();
    GL9306_Init();

    Drone_data_calibration(&Drone_offset);
}

void Drone_Altimeter_Init(void)
{
    TOF_Init();
	printf("TOF init.\n");
}

void Drone_Baroceptor_Init(void)
{
    uint8_t counter = 100;
    while(MS_Init() != I2C_Success)
	{
        if(counter == 0)
        {
            while(1)
            {
                printf("MS Init Failed!\n");
                LED_blink();
                delay_ms(1000);
            }
        }

		I2C2_Init();
		printf("Ms reloading\n");
		
		delay_ms(50);

        counter--;
	}
	printf("MS init\n");
}

void Drone_IMU_Init(void)
{
    uint8_t counter = 100;
	while(GY_Init() != I2C_Success)
	{
        if(counter == 0)
        {
            while(1)
            {
                printf("GY Init Failed!\n");
                LED_blink();
                delay_ms(1000);
            }
        }

		I2C1_Init();
		printf("GY reloading\n");
		
		delay_ms(50);

        counter--;
	}
	printf("GY init\n");
}

void Drone_POST_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_Struct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_Struct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Struct.GPIO_OType = GPIO_OType_PP;
    GPIO_Struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Struct.GPIO_Pin = Drone_POST_Pin;
    GPIO_Init(GPIOC, &GPIO_Struct);
}

void Drone_Get_Channel(uint8_t *channel)
{
    I2C_RequestFrom_EspSoc(I2C_Esp32Soc_Register_NRF_Channel, channel);
}


void Drone_data_calibration(POST_OFFSET_t *drone_offset)
{
    float temp_offset_roll, temp_offset_pitch;

    temp_offset_roll = 0;
    temp_offset_pitch = 0;

    drone_offset->offset_roll = 0;
    drone_offset->offset_pitch = 0;
    //drone_offset->offset_yaw = 0;
    
    printf("Pre sampling... \n");
    for(uint8_t i = 0; i < MS5611_Sampling_FIFO_Size * 4; i++)
    {
        delay_ms(10);
        MS_RefreshData();
        GY_RefreshData();
    }
    printf("Pre sampling over... \n");
    
    VL53L1X_StartRanging(0);

    for(uint8_t i = 0; i < Drone_Calibration_Samples; i++)
    {
        GY_RefreshData();

        temp_offset_roll += GY_Roll();
        temp_offset_pitch += GY_Pitch();

        //drone_offset->offset_yaw += GY_Yaw();

        delay_ms(3);
    }

    VL53L1X_StopRanging(0);
    VL53L1X_ClearInterrupt(0);

    temp_offset_roll /= (float) Drone_Calibration_Samples;
    temp_offset_pitch /= (float) Drone_Calibration_Samples;

    drone_offset->offset_roll = temp_offset_roll;
    drone_offset->offset_pitch = temp_offset_pitch;
    //drone_offset->offset_yaw /= (float) Drone_Calibration_Samples;
    
    Drone_Offsets_toString();

    TOF_EXTILine_Cmd(ENABLE);
}

void Drone_Offsets_toString(void)
{
    printf("\n");
    printf("    <Offset> \n");
    printf("        Roll        :   %.2f\n", Drone_offset.offset_roll);
    printf("        Pitch       :   %.2f\n", Drone_offset.offset_pitch);
    //printf("        Yaw         :   %.2f\n", Drone_offset.offset_yaw);
}


void Drone_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_Struct;

    // GY-95T
    NVIC_Struct.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_Struct);

    // MS5611
    /*
    NVIC_Struct.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_Struct);
    */

    // TOF400C IRQ
    TOF_EXTILine_Cmd(DISABLE);
    /*
    NVIC_Struct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_Struct);*/

    // NRF IRQ
    NVIC_Struct.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_Struct);

    // XBOX
    NVIC_Struct.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_Struct);

    // ADC
    NVIC_Struct.NVIC_IRQChannel = ADC_IRQn;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);

    // TIM ADC
    NVIC_Struct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_Struct);

    // UART
    NVIC_Struct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);

    // GL9306
    //NVIC_Struct.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    //NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
    //NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
    //NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_Struct);

    // UART2
    NVIC_Struct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
}

void Drone_NVIC_State(uint8_t state)
{
    NVIC_InitTypeDef NVIC_Struct;

    if(state == ENABLE)
    {
        // GY-95T
        NVIC_Struct.NVIC_IRQChannel = TIM5_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
        NVIC_Init(&NVIC_Struct);

        // MS5611
        /*
        NVIC_Struct.NVIC_IRQChannel = TIM4_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
        NVIC_Init(&NVIC_Struct);
        */
   

        // TOF400C IRQ
        TOF_EXTILine_Cmd(DISABLE);
        /*
        NVIC_Struct.NVIC_IRQChannel = EXTI1_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
        NVIC_Init(&NVIC_Struct);*/

        // NRF IRQ
        NVIC_Struct.NVIC_IRQChannel = EXTI15_10_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
        NVIC_Init(&NVIC_Struct);

        // XBOX
        NVIC_Struct.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
        NVIC_Init(&NVIC_Struct);

        // ADC
        NVIC_Struct.NVIC_IRQChannel = ADC_IRQn;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_Struct);

        // TIM ADC
        NVIC_Struct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
        NVIC_Init(&NVIC_Struct);

        // UART
        NVIC_Struct.NVIC_IRQChannel = USART1_IRQn;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_Struct);

        // GL9306
        //NVIC_Struct.NVIC_IRQChannel = DMA1_Stream5_IRQn;
        //NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
        //NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
        //NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        //NVIC_Init(&NVIC_Struct);

        // UART2
        NVIC_Struct.NVIC_IRQChannel = USART2_IRQn;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_Struct);
    }
    else
    {
        // GY-95T
        NVIC_Struct.NVIC_IRQChannel = TIM5_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
        NVIC_Init(&NVIC_Struct);

        // MS5611
        /*
        NVIC_Struct.NVIC_IRQChannel = TIM4_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
        NVIC_Init(&NVIC_Struct);
        */
   
        // TOF400C IRQ
        TOF_EXTILine_Cmd(DISABLE);
        /*
        NVIC_Struct.NVIC_IRQChannel = EXTI1_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
        NVIC_Init(&NVIC_Struct);*/

        // NRF IRQ
        NVIC_Struct.NVIC_IRQChannel = EXTI15_10_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
        NVIC_Init(&NVIC_Struct);

        // XBOX
        NVIC_Struct.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
        NVIC_Init(&NVIC_Struct);

        // ADC
        NVIC_Struct.NVIC_IRQChannel = ADC_IRQn;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
        NVIC_Struct.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_Struct);

        // TIM ADC
        NVIC_Struct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
        NVIC_Struct.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
        NVIC_Init(&NVIC_Struct);

        // UART
        NVIC_Struct.NVIC_IRQChannel = USART1_IRQn;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
        NVIC_Struct.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_Struct);

        // GL9306
        //NVIC_Struct.NVIC_IRQChannel = DMA1_Stream5_IRQn;
        //NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
        //NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
        //NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        //NVIC_Init(&NVIC_Struct);

        // UART2
        NVIC_Struct.NVIC_IRQChannel = USART2_IRQn;
        NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_Struct.NVIC_IRQChannelSubPriority = 3;
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_Struct);
    }
}

