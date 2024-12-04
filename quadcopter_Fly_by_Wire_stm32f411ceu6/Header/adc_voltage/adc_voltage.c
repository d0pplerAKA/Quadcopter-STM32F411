#include "adc_voltage.h"


//nrf_receive_raw.volt = 11.6;

uint8_t adc_sample_time;

float temp_adc_value[ADC_Sample_Term];


void ADC_Voltage_Init(void)
{
    adc_sample_time = 0;
    ADC_GPIO_Init();
    ADC_Parameter_Init();
}

void ADC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    //GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_ADC;
    GPIO_Init(GPIO_Port_ADC, &GPIO_InitStruct);
}

void ADC_Parameter_Init(void)
{
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);

    

    ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_15Cycles;
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInit(&ADC_CommonInitStruct);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    //ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_NbrOfConversion = 1;
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_480Cycles);

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    ADC_Cmd(ADC1, ENABLE);
    //ADC_TempSensorVrefintCmd(ENABLE);
    //ADC_VBATCmd(ENABLE);
}


void ADC_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
    {
        temp_adc_value[adc_sample_time] = (float) ADC_GetConversionValue(ADC1) * (float) 3.3f / (float) 4096.0f;
        
        adc_sample_time++;

        if(adc_sample_time == ADC_Sample_Term)
        {
            adc_sample_time = 0;

            float adc_value = 0;

            for(uint8_t i = 0; i < ADC_Sample_Term; i++)
                adc_value += temp_adc_value[i];

            adc_value /= (float) ADC_Sample_Term;

            adc_value = adc_value * 15.0f / 3.3f;

            nrf_receive_raw.volt = adc_value;

            uint8_t temp_i2c = (uint8_t) floatMap(adc_value, 11.1f, 12.6f, 0.0f, 100.0f);

            I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_Battery, temp_i2c);

            //printf("ADC: %d %.3f\n", temp_i2c, adc_value);
        }
    }

    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}
