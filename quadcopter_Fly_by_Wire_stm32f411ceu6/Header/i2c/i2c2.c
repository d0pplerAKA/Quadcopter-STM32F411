#include "i2c.h"

void I2C2_Init(void)    // MS5611 / ESP32S3
{
    GPIO_InitTypeDef GPIO_S;
    I2C_InitTypeDef I2C_InitStruct;  

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    GPIO_S.GPIO_Mode = GPIO_Mode_AF;
    GPIO_S.GPIO_OType = GPIO_OType_OD;
    GPIO_S.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_S.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_S.GPIO_Pin = GPIO_PIN_SCL2 | GPIO_PIN_SDA2;

    GPIO_Init(GPIOB, &GPIO_S);
    GPIO_SetBits(GPIOB, GPIO_PIN_SCL2);
    GPIO_SetBits(GPIOB, GPIO_PIN_SDA2);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);        // SCL GPIO_Pin_10      GPIOB
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF9_I2C2);        // SDA GPIO_Pin_9       GPIOB

    I2C_DeInit(I2C2);
    delay_ms(5);

    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_ClockSpeed = 400000;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C2, &I2C_InitStruct);

    I2C_AcknowledgeConfig(I2C2, ENABLE);
    I2C_Cmd(I2C2, ENABLE);
}



uint8_t I2C2_WriteOnlyReg(uint8_t sl_addr, uint8_t byte_write)  //MS5611
{
    uint16_t counter;
	
	counter = I2C_Timeout;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_BUSY;
        }
        
        counter--;
    }
    I2C_GenerateSTART(I2C2, ENABLE);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C2, sl_addr, I2C_Direction_Transmitter);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV6;
        }
        
        counter--;
    }
    
    I2C_SendData(I2C2, byte_write);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV8;
        }
        
        counter--;
    }
    I2C_GenerateSTOP(I2C2, ENABLE);

    return NO_ERROR;
}


uint8_t I2C2_ReadBytes_Direct(uint8_t sl_addr, uint8_t *byte_buffer_receive)    // MS5611
{
    uint16_t counter;
	
	counter = I2C_Timeout;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_BUSY;
        }
        
        counter--;
    }
    I2C_GenerateSTART(I2C2, ENABLE);


    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C2, sl_addr, I2C_Direction_Receiver);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV7;
        }
        
        counter--;
    }
    *byte_buffer_receive = I2C_ReceiveData(I2C2);
    byte_buffer_receive++;

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV7;
        }
        
        counter--;
    }
    *byte_buffer_receive = I2C_ReceiveData(I2C2);
    byte_buffer_receive++;

    I2C_AcknowledgeConfig(I2C2, DISABLE);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV7;
        }
        
        counter--;
    }
    *byte_buffer_receive = I2C_ReceiveData(I2C2);

    I2C_GenerateSTOP(I2C2, ENABLE);

    I2C_AcknowledgeConfig(I2C2, ENABLE);

    return NO_ERROR;
}

uint8_t I2C2_ReadByte_Arduino(uint8_t sl_addr, uint8_t *byte_read)
{
    uint16_t counter;

    counter = I2C_Timeout;
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_BUSY;
        }
        
        counter--;
    }
    I2C_GenerateSTART(I2C2, ENABLE);


    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C2, sl_addr, I2C_Direction_Receiver);

    I2C_AcknowledgeConfig(I2C2, DISABLE);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C2, ENABLE);
            I2C_AcknowledgeConfig(I2C2, ENABLE);
            return ERROR_EV7;
        }
        
        counter--;
    }

    *byte_read = I2C_ReceiveData(I2C2);

    I2C_GenerateSTOP(I2C2, ENABLE);

    I2C_AcknowledgeConfig(I2C2, ENABLE);

    return NO_ERROR;
}


uint8_t I2C_RequestFrom_EspSoc(uint8_t reg_addr, uint8_t *byte_read)
{
    uint8_t err;

    err = I2C_WriteByte(I2C_PORT_MS_ESP, I2C_Esp32Soc_Address, I2C_Esp32Soc_Read_Operation, reg_addr);
    
    if(err != NO_ERROR)
        return ERROR;

    err = I2C2_ReadByte_Arduino(I2C_Esp32Soc_Address, byte_read);

    if(err != NO_ERROR)
        return ERROR;
    
    return NO_ERROR;
}

uint8_t I2C_SendTo_EspSoc(uint8_t reg_addr, uint8_t byte_write)
{
    uint8_t err;

    err = I2C_WriteByte(I2C_PORT_MS_ESP, I2C_Esp32Soc_Address, reg_addr, byte_write);

    if(err != NO_ERROR) return ERROR;
    else return NO_ERROR;
}
