#include "timer/timer.h"

uint64_t sys_tickstamp = 0x0;

uint32_t xbox_sampling_counter = 0x0;

float motor_startup = 0.00f;


void SysTick_Init(void)
{
    SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
}

void Timer_Cmd(void)
{
    TIM_Cmd(TIM_GY95T, ENABLE);

    //TIM_Cmd(TIM_MS5611_ESP32S3, ENABLE);

    TIM_Cmd(TIM_ADC, ENABLE);

    TIM_Cmd(TIM_XBOX, ENABLE);
    
    

    
    DMA_Cmd(DMA1_Stream5, ENABLE);

    USART_Cmd(USART2, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TC);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
}

void TIM_BLDC_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef TIM_OCStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Prescaler = 99;
    TIM_TimeBaseStruct.TIM_Period = 19999;
    TIM_TimeBaseInit(TIM_BLDC, &TIM_TimeBaseStruct);

    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_Pulse = 0;
    TIM_OC1Init(TIM_BLDC, &TIM_OCStruct);
    TIM_OC2Init(TIM_BLDC, &TIM_OCStruct);
    TIM_OC3Init(TIM_BLDC, &TIM_OCStruct);
    TIM_OC4Init(TIM_BLDC, &TIM_OCStruct);

    TIM_OC1PreloadConfig(TIM_BLDC, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM_BLDC, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM_BLDC, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM_BLDC, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM_BLDC, ENABLE);
}

void TIM_ToString_Init(void)
{
    //SysTick_Init();

    //TIM_MS5611_Init();
    TIM_GY95T_Init();
    
    TIM_ADC_Voltage_Init();
    TIM_XBOX_Init();
}

void TIM_MS5611_Init(void)                              // sampling freq: 20 times per sec (1 / 20 = 0.05) 50ms
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = 2499;
    TIM_TimeBaseStruct.TIM_Prescaler = 1999;
    TIM_TimeBaseInit(TIM_MS5611_ESP32S3, &TIM_TimeBaseStruct);

    TIM_ClearITPendingBit(TIM_MS5611_ESP32S3, TIM_IT_Update);
    TIM_ITConfig(TIM_MS5611_ESP32S3, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM_MS5611_ESP32S3, DISABLE);
}

void TIM_GY95T_Init(void)                               // sampling freq: 200 times per sec (1 / 200 = 0.005s) 5ms
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = 249;
    TIM_TimeBaseStruct.TIM_Prescaler = 1999;
    TIM_TimeBaseInit(TIM_GY95T, &TIM_TimeBaseStruct);

    TIM_ClearITPendingBit(TIM_GY95T, TIM_IT_Update);
    TIM_ITConfig(TIM_GY95T, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM_GY95T, DISABLE);
}

void TIM_ADC_Voltage_Init(void)                         // sampling freq: 2 times per sec (1 / 2 = 0.5s) 500ms
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = 4999;
    TIM_TimeBaseStruct.TIM_Prescaler = 9999;
    TIM_TimeBaseInit(TIM_ADC, &TIM_TimeBaseStruct);

    TIM_ClearITPendingBit(TIM_ADC, TIM_IT_Update);
    TIM_ITConfig(TIM_ADC, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM_ADC, DISABLE);
}

void TIM_XBOX_Init(void)                                // sampling freq: 40 times per sec (1 / 40 = 0.025s) 25ms
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period = 4999;
    TIM_TimeBaseStruct.TIM_Prescaler = 499;
    TIM_TimeBaseInit(TIM_XBOX, &TIM_TimeBaseStruct);

    TIM_ClearITPendingBit(TIM_XBOX, TIM_IT_Update);
    TIM_ITConfig(TIM_XBOX, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM_XBOX, DISABLE);

    nrf_joy_stick.JoyLeftV = 0.0f;
    nrf_joy_stick.JoyRightH = 0.0f;
    nrf_joy_stick.JoyRightV = 0.0f;
    nrf_joy_stick.TrigLeft = 0.0f;
    nrf_joy_stick.TrigRight = 0.0f;
}

/*
void TIM4_IRQHandler(void)                                      // TIM 4 Handler
{
    if(TIM_GetITStatus(TIM_MS5611_ESP32S3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM_MS5611_ESP32S3, TIM_IT_Update);
        
        MS_RefreshData();
        
        //printf("cmd MS\n");
    }
}
*/

void TIM5_IRQHandler(void)                                      //TIM 5 Handler
{
    if(TIM_GetITStatus(TIM_GY95T, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM_GY95T, TIM_IT_Update);

        GY_RefreshData();


        //FWB_PID_onLoop(&pid_yaw, Drone_offset.offset_yaw * FWB_deg2rad, GY_Yaw() * FWB_deg2rad, 0.0125f, 0);


        /*
            Drone_Motor.value_L1 +=
            Drone_Motor.value_L2 +=
            Drone_Motor.value_R1 +=
            Drone_Motor.value_R2 +=
        */


        /*
        if(package_is_control_message && nrf_send_raw.is_control_message && xbox_recovery_flag == 0)
        {
            
            FWB_LPF_onLoop(&lpf_roll, GY_Gyro_x() * FWB_deg2rad);
            FWB_LPF_onLoop(&lpf_pitch, GY_Gyro_x() * FWB_deg2rad);
            FWB_LPF_onLoop(&lpf_yaw, GY_Gyro_z());
            //FWB_LPF2_onLoop(&lpf2_roll, GY_Gyro_x() * FWB_deg2rad);

            float temp_lpf_roll = 0;
            float temp_lpf_pitch = 0;

            for(uint8_t i = FWB_LFP_BUFFER - 1; i > 0; i--)
            {
                LPF_FIFO_ROLL[i] = LPF_FIFO_ROLL[i - 1];
                LPF_FIFO_PITCH[i] = LPF_FIFO_PITCH[i - 1];

                temp_lpf_roll += LPF_FIFO_ROLL[i];
                temp_lpf_pitch += LPF_FIFO_PITCH[i];
            }

            LPF_FIFO_ROLL[0] = lpf_roll.output;
            LPF_FIFO_PITCH[0] = lpf_pitch.output;

            temp_lpf_roll += lpf_roll.output;
            temp_lpf_pitch += lpf_pitch.output;


            FWB_PID_onLoop(&pid_roll_degree, drone_attitude.desired_roll, GY_Roll(), 0.3f, 0);
            FWB_PID_onLoop(&pid_roll_gyro, pid_roll_degree.output, temp_lpf_roll / (float) FWB_LFP_BUFFER, 0.038f, 1);

            FWB_PID_onLoop(&pid_pitch_degree, drone_attitude.desired_pitch, GY_Pitch(), 0.3f, 0);
            FWB_PID_onLoop(&pid_pitch_gyro, pid_pitch_degree.output, temp_lpf_pitch / (float) FWB_LFP_BUFFER, 0.038f, 1);

            FWB_PID_onLoop(&pid_yaw, drone_attitude.desired_yaw, lpf_yaw.output, 0.04f, 0);

            Drone_throttle_factor(nrf_joy_stick.JoyLeftV, &throttle_val);
            Drone_Yaw_adjusting(nrf_joy_stick.TrigLeft, nrf_joy_stick.TrigRight, &drone_attitude.desired_yaw);

            FWB_PID_onLoop(&pid_flow_x, 0.0f, (float) gl9306_displacement.d_x, 0.5f, 1);
            FWB_PID_onLoop(&pid_flow_y, 0.0f, (float) gl9306_displacement.d_y, 0.5f, 1);

            if(motor_startup >= 1.0f)       motor_startup = 1.0f;
            else                            motor_startup += 0.0015f;

            Drone_Motor.value_L1 = BLDC_BaseSpeed + throttle_val;
            Drone_Motor.value_L2 = BLDC_BaseSpeed + throttle_val;
            Drone_Motor.value_R1 = BLDC_BaseSpeed + throttle_val;
            Drone_Motor.value_R2 = BLDC_BaseSpeed + throttle_val;

            Drone_Motor.value_L1 += pid_roll_gyro.output;
            Drone_Motor.value_L2 += pid_roll_gyro.output;
            Drone_Motor.value_R1 += -pid_roll_gyro.output;
            Drone_Motor.value_R2 += -pid_roll_gyro.output;
            
            Drone_Motor.value_L1 += -pid_pitch_gyro.output;
            Drone_Motor.value_L2 += pid_pitch_gyro.output;
            Drone_Motor.value_R1 += -pid_pitch_gyro.output;
            Drone_Motor.value_R2 += pid_pitch_gyro.output;

            if(pid_yaw.output <= 1.0f && pid_yaw.output >= -1.0f)
                pid_yaw.output = 0.0f;

            Drone_Motor.value_L1 += pid_yaw.output;
            Drone_Motor.value_L2 += -pid_yaw.output;
            Drone_Motor.value_R1 += -pid_yaw.output;
            Drone_Motor.value_R2 += pid_yaw.output;

            if((nrf_joy_stick.JoyRightH <= 0.065f && nrf_joy_stick.JoyRightH >= -0.065f) && TOF_Height() > 155.0f && TOF_Height() < 2500.0f)   // flow x
            {
                if(pid_flow_x.output >= 35.0f)
                    pid_flow_x.output = 35.0f;
                
                if(pid_flow_x.output <= -35.0f)
                    pid_flow_x.output = -35.0f;

                Drone_Motor.value_L1 += -pid_flow_x.output;
                Drone_Motor.value_L2 += -pid_flow_x.output;
                Drone_Motor.value_R1 += pid_flow_x.output;
                Drone_Motor.value_R2 += pid_flow_x.output;
            }

            if((nrf_joy_stick.JoyRightV <= 0.065f && nrf_joy_stick.JoyRightV >= -0.065f) && TOF_Height() > 155.0f && TOF_Height() < 2500.0f) // flow y
            {
                if(pid_flow_y.output >= 35.0f)
                    pid_flow_y.output = 35.0f;
                
                if(pid_flow_y.output <= -35.0f)
                    pid_flow_y.output = -35.0f;
                
                Drone_Motor.value_L1 += -pid_flow_y.output;
                Drone_Motor.value_L2 += pid_flow_y.output;
                Drone_Motor.value_R1 += -pid_flow_y.output;
                Drone_Motor.value_R2 += pid_flow_y.output;
            }

            Drone_Motor.value_L1 *= motor_startup;
            Drone_Motor.value_L2 *= motor_startup;
            Drone_Motor.value_R1 *= motor_startup;
            Drone_Motor.value_R2 *= motor_startup;

            BLDC_SetSpeed(&Drone_Motor);

            if(GY_Roll() > 43.0f || GY_Pitch() > 43.0f || GY_Roll() < -43.0f || GY_Pitch() < -43.0f)
            {
                Drone_Motor.value_L1 = 0;
                Drone_Motor.value_L2 = 0;
                Drone_Motor.value_R1 = 0;
                Drone_Motor.value_R2 = 0;

                BLDC_SetSpeed(&Drone_Motor);
                TIM_Cmd(TIM_GY95T, DISABLE);
            }
        }
        else
        {
            motor_startup = 0.0f;

            Drone_Motor.value_L1 = 0.0f;
            Drone_Motor.value_L2 = 0.0f;
            Drone_Motor.value_R1 = 0.0f;
            Drone_Motor.value_R2 = 0.0f;

            BLDC_SetSpeed(&Drone_Motor);
        }
        */

        Drone_Motor.value_L1 = 0.0f;
        Drone_Motor.value_L2 = 0.0f;
        Drone_Motor.value_R1 = 0.0f;
        Drone_Motor.value_R2 = 0.0f;

        BLDC_SetSpeed(&Drone_Motor);


        //printf("ROLL=%.2f,PITCH=%.2f,YAW=%.2f\n", GY_Roll(), GY_Pitch(), GY_Yaw());

        //printf("Gy_X=%.2f,Gy_Y=%.2f,Gy_Z=%.2f\n", GY_Gyro_x(), GY_Gyro_y(), GY_Gyro_x());

        //printf("cmd GY\n");
    }
}


void TIM1_BRK_TIM9_IRQHandler(void)                             //TIM 9 Handler
{
    if(TIM_GetITStatus(TIM_ADC, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM_ADC, TIM_IT_Update);

        ADC_SoftwareStartConv(ADC1);
    }
}

void TIM1_UP_TIM10_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM_XBOX, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM_XBOX, TIM_IT_Update);

        XBOX_IRQHandler();

        //printf("cmd Xbox\n");
    }
}

uint8_t xbox_sampling_cmp(void)
{
    return ((xbox_sampling_counter % 6) == 0);
}










/*
void TIM_TOF400C_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    NVIC_InitTypeDef NVIC_Struct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Prescaler = 9999;
    TIM_TimeBaseStruct.TIM_Period = 9999;
    TIM_TimeBaseInit(TIM_TOF400C, &TIM_TimeBaseStruct);

    TIM_ITConfig(TIM_TOF400C, TIM_IT_Update, ENABLE);

    NVIC_Struct.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_Struct);

    TIM_Cmd(TIM_TOF400C, DISABLE);
}
*/



