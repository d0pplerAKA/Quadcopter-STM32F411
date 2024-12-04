#include "tof400c.h"


VL53L1X_Result_t vl53l1x_result;
TOF400C_t tof400c;

float TOF_FIFO[TOF_FIFO_Size];

void TOF_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_Pin = TOF400C_XSHUT_Pin;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    TOF400C_OFF();

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_Pin = TOF400C_IRQ_Pin;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, TOF400C_IRQ_Pin);

    EXTI_InitTypeDef EXTI_InitStruct;

    EXTI_InitStruct.EXTI_Line = EXTI_Line1;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStruct);

    EXTI_ClearITPendingBit(EXTI_Line1);
    TOF_IRQ_Install;
}


void TOF_Init(void)
{
    tof400c.onLoop = 0;
    tof400c.height_distance_in_cm = 0.0f;

    TOF_GPIO_Init();
    delay_ms(5);

    TOF400C_OFF();
    delay_ms(25);
    TOF400C_ON();
    delay_ms(25);

    uint8_t status = 0, sensor_state = 0;

    uint16_t counter = I2C_Timeout;
    while(sensor_state == 0)
    {
        if(counter == 0 || status != 0)
        {
            while(1)
            {
                printf("Boot failed!\n");
                LED_blink();
                delay_ms(1000);
            }
        }
        
        status = VL53L1X_BootState(0, &sensor_state);
        delay_ms(10);

        counter--;
    }

    delay_ms(25);
    status = VL53L1X_SensorInit(0);
    if(status)
    {
        while(1)
        {
            printf("sensor init failed!\n");
            LED_blink();
            delay_ms(1000);
        }
    }

    delay_ms(25);
    status = VL53L1X_SetInterruptPolarity(0, 0);        // Active Low!!!
    if(status)
    {
       
        while(1)
        {
            printf("sensor set irq polarity failed!\n");
            LED_blink();
            delay_ms(1000);
        }
    }

    delay_ms(25);
    status = VL53L1X_SetROI(0, 4, 4);
    if(status)
    {
        while(1)
        {
            printf("sensor set ROI failed!\n");
            LED_blink();
            delay_ms(1000);
        }
    }

    delay_ms(25);
    status = VL53L1X_SetDistanceThreshold(0, 50, 3000, 3, 1);
    if(status)
    {
        while(1)
        {
            printf("sensor set Threshold failed!\n");
            LED_blink();
            delay_ms(1000);
        }
    }
    
    delay_ms(25);
    status = VL53L1X_SetDistanceMode(0, 2);
    if(status)
    {
        while(1)
        {
            printf("sensor set Ranging Mode failed!\n");
            LED_blink();
            delay_ms(1000);
        }
    }

    /*
    *   TB - TimingBudget -> Sampling time
    *   Time cost in sampling.
    */
    delay_ms(25);
    status = VL53L1X_SetTimingBudgetInMs(0, 50);
    if(status)
    {
        while(1)
        {
            printf("sensor set sampling time failed!\n");
            LED_blink();
            delay_ms(1000);
        }
    }

    /*
    *   IMP - intermeasurement period in milliseconds - Sampling interval (TB is included in IMP)
    *   IMP >= TB
    */
    delay_ms(25);
    status = VL53L1X_SetInterMeasurementInMs(0, 75);
    if(status)
    {
        while(1)
        {
            printf("sensor set Sampling interval failed!\n");
            LED_blink();
            delay_ms(1000);
        }
    }
}

void TOF_OnLoopTrigger(void)
{   
    tof400c.onLoop++;

    if(tof400c.onLoop % 2 == 1)
    {
        VL53L1X_ClearInterrupt(0);

        VL53L1X_StartRanging(0);
    }
    else
    {
        VL53L1X_StopRanging(0);

        VL53L1X_ClearInterrupt(0);
    }
}

void TOF_EXTILine_Cmd(uint8_t state)
{
    NVIC_InitTypeDef NVIC_Struct;

    NVIC_Struct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
    
    if(state == ENABLE)
        NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    else
        NVIC_Struct.NVIC_IRQChannelCmd = DISABLE;
    
    NVIC_Init(&NVIC_Struct);
}

float TOF_Height(void)
{
    return tof400c.height_distance_in_cm;
}

void EXTI1_IRQHandler(void)
{
    if(EXTI_GetFlagStatus(EXTI_Line1) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
        
        VL53L1X_GetResult(0, &vl53l1x_result);

        float temp_vlx = 0.0f;

        for(uint8_t i = TOF_FIFO_Size - 1; i > 0; i--)
        {
            TOF_FIFO[i] = TOF_FIFO[i - 1];

            temp_vlx += TOF_FIFO[i];
        }

        TOF_FIFO[0] = (float) vl53l1x_result.Distance;

        tof400c.height_distance_in_cm = MS5611_firstOrder_Filtering(temp_vlx / (float) TOF_FIFO_Size, 
                                                                    TOF_FirstOrder_Coff, 
                                                                    tof400c.height_distance_in_cm);

        gl9306_displacement.d_h = TOF_Height();

        //printf("TOF=%.2f\n", TOF_Height());
        
        VL53L1X_ClearInterrupt(0);
    }
}

