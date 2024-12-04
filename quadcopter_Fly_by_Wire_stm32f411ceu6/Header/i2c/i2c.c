#include "i2c.h"

void I2Cs_Init(void)
{
    I2C1_Init();
    I2C2_Init();
    I2C3_Init();
}

uint8_t I2C_WriteByte(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t byte_write)
{
    uint16_t counter;
	
    counter = I2C_Timeout;
    while(I2C_GetFlagStatus(I2C_BASE, I2C_FLAG_BUSY))
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_BUSY;
        }

        counter--;
    }
    I2C_GenerateSTART(I2C_BASE, ENABLE);
	
    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C_BASE, sl_addr, I2C_Direction_Transmitter);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV6;
        }
        
        counter--;
    }
    I2C_SendData(I2C_BASE, reg_addr);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV8;
        }
        
        counter--;
    }
    I2C_SendData(I2C_BASE, byte_write);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV8_2;
        }
        
        counter--;
    }
    I2C_GenerateSTOP(I2C_BASE, ENABLE);

    return NO_ERROR;
}

uint8_t I2C_WriteBytes(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t *byte_buffer_write, uint32_t byte_num)
{
    uint16_t counter;
	
    counter = I2C_Timeout;
    while(I2C_GetFlagStatus(I2C_BASE, I2C_FLAG_BUSY))
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_BUSY;
        }

        counter--;
    }
    I2C_GenerateSTART(I2C_BASE, ENABLE);
	
    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C_BASE, sl_addr, I2C_Direction_Transmitter);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV6;
        }
        
        counter--;
    }
    I2C_SendData(I2C_BASE, reg_addr);

    for(uint32_t i = 0; i < byte_num; i++)
    {
        counter = I2C_Timeout;
        while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS)
        {
            if(counter == 0)
            {
                I2C_GenerateSTOP(I2C_BASE, ENABLE);
                return ERROR_EV8;
            }
            
            counter--;
        }
        I2C_SendData(I2C_BASE, *byte_buffer_write);

        byte_buffer_write++;
    }

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV8_2;
        }
        
        counter--;
    }
    I2C_GenerateSTOP(I2C_BASE, ENABLE);

    return NO_ERROR;
}

uint8_t I2C_ReadByte(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t *byte_receive)
{
    uint16_t counter;
    *byte_receive = 0x00;

    counter = I2C_Timeout;
    while(I2C_GetFlagStatus(I2C_BASE, I2C_FLAG_BUSY))
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_BUSY;
        }
        
        counter--;
    }
    I2C_GenerateSTART(I2C_BASE, ENABLE);            

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C_BASE, sl_addr, I2C_Direction_Transmitter);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV6;
        }
        
        counter--;
    }
    I2C_SendData(I2C_BASE, reg_addr);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV8;
        }
        
        counter--;
    }
    I2C_GenerateSTART(I2C_BASE, ENABLE);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C_BASE, sl_addr, I2C_Direction_Receiver);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV6;
        }
        
        counter--;
    }
    I2C_AcknowledgeConfig(I2C_BASE, DISABLE);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV7;
        }
        
        counter--;
    }

    *byte_receive = I2C_ReceiveData(I2C_BASE);

    I2C_GenerateSTOP(I2C_BASE, ENABLE);

    I2C_AcknowledgeConfig(I2C_BASE, ENABLE);

    return NO_ERROR;
}

uint8_t I2C_ReadBytes(I2C_TypeDef *I2C_BASE, uint8_t sl_addr, uint8_t reg_addr, uint8_t *byte_buffer_receive, uint32_t byte_num)
{
    uint16_t counter;

    for(uint32_t i = 0; i < byte_num; i++)
        byte_buffer_receive[i] = 0x00;

    counter = I2C_Timeout;
    while(I2C_GetFlagStatus(I2C_BASE, I2C_FLAG_BUSY))
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_BUSY;
        }

        counter--;
    }
    I2C_GenerateSTART(I2C_BASE, ENABLE);


    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C_BASE, sl_addr, I2C_Direction_Transmitter);


    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV6;
        }

        counter--;
    }
    I2C_SendData(I2C_BASE, reg_addr);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV8;
        }

        counter--;
    }
    I2C_GenerateSTART(I2C_BASE, ENABLE);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV5;
        }
        
        counter--;
    }
    I2C_Send7bitAddress(I2C_BASE, sl_addr, I2C_Direction_Receiver);

    counter = I2C_Timeout;
    while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS)
    {
        if(counter == 0)
        {
            I2C_GenerateSTOP(I2C_BASE, ENABLE);
            return ERROR_EV6;
        }

        counter--;
    }

    while(byte_num)
    {
        if(byte_num == 1)
            I2C_AcknowledgeConfig(I2C_BASE, DISABLE);
        
        counter = I2C_Timeout;
        while(I2C_CheckEvent(I2C_BASE, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS)
        {
            if(counter == 0)
            {
                for(uint32_t i = 0; i < byte_num; i++)
                    byte_buffer_receive[i] = 0x00;

                I2C_GenerateSTOP(I2C_BASE, ENABLE);
                return ERROR_EV7;
            }

            counter--;
        }
        *byte_buffer_receive = I2C_ReceiveData(I2C_BASE);

        byte_buffer_receive++;

        byte_num--;
    }

    I2C_GenerateSTOP(I2C_BASE, ENABLE);

    I2C_AcknowledgeConfig(I2C_BASE, ENABLE);

    return NO_ERROR;
}
