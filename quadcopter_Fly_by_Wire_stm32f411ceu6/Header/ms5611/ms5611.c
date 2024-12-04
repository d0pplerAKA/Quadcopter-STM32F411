#include "ms5611.h"


MS5611_PROM ms5611_prom;
MS5611_Data ms5611_data;

float temp_ms_height = 0.0f;
uint16_t ms_counter = 0;

float ms5611_data_Height_FIFO[MS5611_Sampling_FIFO_Size];

float ms5611_temp_data_temperature = 0.0f;
float ms5611_temp_data_pressure = 0.0f;
float ms5611_temp_data_height = 0.0f;


uint8_t MS_Init(void)
{
    MS_Reset();	
	
    uint8_t err = MS_ReadPROM();

    uint16_t prom[8];
    memset(prom, 0, sizeof(uint16_t) * 8);

    prom[0] = ms5611_prom.prom_Reserve;
    prom[1] = ms5611_prom.prom_C1;
    prom[2] = ms5611_prom.prom_C2;
    prom[3] = ms5611_prom.prom_C3;
    prom[4] = ms5611_prom.prom_C4;
    prom[5] = ms5611_prom.prom_C5;
    prom[6] = ms5611_prom.prom_C6;
    prom[7] = ms5611_prom.prom_CRC;

    uint8_t crc_val = MS5611_CRC(prom);

    if(crc_val != I2C_Success)
        printf("    Value of CRC MS: %d\n", crc_val);
    
    memset(ms5611_data_Height_FIFO, 0, sizeof(float) * MS5611_Sampling_FIFO_Size);

    if(err != I2C_Success)
        return I2C_Fail;
    else
        return I2C_Success;
}

void MS_Reset(void)
{
    I2C2_WriteOnlyReg(MS5611_ADDR, MS5611_Reset);
    delay_ms(100);
}

uint8_t MS_ReadPROM(void)
{
    uint8_t err;
    uint8_t temp_prom[2];

    memset(&ms5611_prom, 0, sizeof(uint16_t) * 8);

    //C0
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_C0, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_Reserve = (uint16_t) ((((uint16_t) temp_prom[0]) << 8) | (uint16_t) temp_prom[1]);


    //C1
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_C1, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_C1 = (uint16_t) ((((uint16_t) temp_prom[0] << 8)) | (uint16_t) temp_prom[1]);


    //C2
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_C2, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_C2 = (uint16_t) ((((uint16_t) temp_prom[0] << 8)) | (uint16_t) temp_prom[1]);

    
    //C3
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_C3, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_C3 = (uint16_t) ((((uint16_t) temp_prom[0] << 8)) | (uint16_t) temp_prom[1]);
    

    //C4
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_C4, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_C4 = (uint16_t) ((((uint16_t) temp_prom[0] << 8)) | (uint16_t) temp_prom[1]);
    
    
    //C5
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_C5, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_C5 = (uint16_t) ((((uint16_t) temp_prom[0] << 8)) | (uint16_t) temp_prom[1]);
    

    //C6
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_C6, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_C6 = (uint16_t) ((((uint16_t) temp_prom[0] << 8)) | (uint16_t) temp_prom[1]);


    //C7 (CRC)
    err = I2C_ReadBytes(I2C_PORT_MS_ESP, MS5611_ADDR, MS5611_Read_PROM_CRC, temp_prom, 2);
    if(err != I2C_Success)
        return I2C_Fail;
    else
        ms5611_prom.prom_CRC = (uint16_t) ((((uint16_t) temp_prom[0] << 8)) | (uint16_t) temp_prom[1]);


    return I2C_Success;
}

uint8_t MS_Convert(uint8_t order, uint8_t ms_delay, uint32_t *result)
{
    uint8_t err;
    uint8_t temp[3];

    *result = 0x0;

    err = I2C2_WriteOnlyReg(MS5611_ADDR, order);
    if(err != I2C_Success)
        return err;

    delay_ms(ms_delay);

    err = I2C2_WriteOnlyReg(MS5611_ADDR, MS5611_Read_ADC);
    if(err != I2C_Success)
        return err;
    
    err = I2C2_ReadBytes_Direct(MS5611_ADDR, temp);
    if(err != I2C_Success)
        return err;
    
    *result = ((temp[0] << 16) | (temp[1] << 8) | temp[2]);

    return I2C_Success;
}

uint8_t MS_Temp(void)
{
    uint8_t err;
    err = MS_Convert(MS5611_D2_OSR, 10, &ms5611_data.data_D2_temp);
    if(err != I2C_Success)
    {
        printf("error %d\n", err);
        
        I2C2_Init();
        if(MS_Init() != 0)
        {
            printf("MS Fatal Exception. Flight might crushed. \n");

            // Blackbox functions implement here
        }

        // Pressure sensor broke.
        // Quadcopter should be never access in to this if condition...

        return I2C_Fail;
    }

    return I2C_Success;
}

uint8_t MS_Pres(void)
{
    uint8_t err;
    err = MS_Convert(MS5611_D1_OSR, 10, &ms5611_data.data_D1_pres);
    if(err != I2C_Success)
    {
        printf("error %d\n", err);
        
        I2C2_Init();
        if(MS_Init() != 0)
        {
            printf("MS Fatal Exception. Flight might crushed. \n");

            // Blackbox functions implement here
        }

        // Pressure sensor broke.
        // Quadcopter should be never access in to this if condition...

        return I2C_Fail;
    }
    
    return I2C_Success;
}

uint8_t MS_RefreshData(void)
{
    uint8_t err;

    err = MS_Temp();
    if(err != I2C_Success)
        return I2C_Fail;

    err = MS_Pres();
    if(err != I2C_Success)
        return I2C_Fail;

    ms5611_data.data_dT = ms5611_data.data_D2_temp - ((uint32_t) ms5611_prom.prom_C5 << 8);

    ms5611_data.data_OFF = ((uint32_t) ms5611_prom.prom_C2 << 16) + ((int64_t) ms5611_data.data_dT * ms5611_prom.prom_C4) / 128.0f;
    ms5611_data.data_SENS = ((uint32_t) ms5611_prom.prom_C1 << 15) + ((int64_t) ms5611_data.data_dT * ms5611_prom.prom_C3) / 256.0f;

    ms5611_data.data_Temperature = MS_Temperature_int32_t();

    ms5611_data.data_Pressure = ((int64_t) (ms5611_data.data_D1_pres * ms5611_data.data_SENS) / 
                                    (int64_t) 0x200000 - ms5611_data.data_OFF) / 
                                        (int64_t) 0x8000;

    ms5611_temp_data_temperature = ms5611_data.data_Temperature_inFloat;
    ms5611_temp_data_pressure = ms5611_data.data_Pressure_inFloat;

    ms5611_data.data_Temperature_inFloat = ms5611_data.data_Temperature / 100.0f;
    ms5611_data.data_Pressure_inFloat = ms5611_data.data_Pressure / 1000.0f;

    ms5611_data.data_Pressure_inFloat = MS5611_firstOrder_Filtering(ms5611_data.data_Pressure_inFloat,
                                                                        MS5611_FirstOrder_PressCoff,
                                                                            ms5611_temp_data_pressure);    
    
    float temp_fifo = 0.0f;
    
    for(uint8_t i = MS5611_Sampling_FIFO_Size - 1; i > 0; i--)
    {
        ms5611_data_Height_FIFO[i] = ms5611_data_Height_FIFO[i - 1];

        temp_fifo += ms5611_data_Height_FIFO[i];
    }

    ms5611_data_Height_FIFO[0] = (MS_Height_Hypsometric() + MS_Height_Barometric()) / 2.0f;

    float min_val = ms5611_data_Height_FIFO[0];
    float max_val = min_val;

    for(uint8_t i = 0; i < MS5611_Sampling_FIFO_Size; i++)
    {
        if(ms5611_data_Height_FIFO[i] > max_val)
            max_val = ms5611_data_Height_FIFO[i];
        
        if(ms5611_data_Height_FIFO[i] < min_val)
            min_val = ms5611_data_Height_FIFO[i];
    }

    temp_fifo -= (max_val + min_val);

    ms5611_temp_data_height = temp_fifo / (MS5611_Sampling_FIFO_Size - 2.0f);

    
    if(ms_counter < 36)
    {
        ms_counter++;

    }
    else if(ms_counter >= 36 && ms_counter < 72)
    {
        ms_counter++;

        temp_ms_height += ms5611_temp_data_height; 
    }
    else if(ms_counter == 72)
    {   
        ms_counter++;

        temp_ms_height /= 36.0f;
    }
    else
    {
        ms5611_data.data_height = (ms5611_temp_data_height - temp_ms_height + 0.55f) * 10.0f;

        ms5611_data.data_height = MS5611_firstOrder_Filtering(ms5611_temp_data_height - temp_ms_height, 
                                                                MS5611_FirstOrder_HeightCoff, 
                                                                    ms5611_data.data_height) * 2.0f - 3.8f;
    }

    return I2C_Success;
}

void MS_toString(void)
{
    printf("--------------------------------------------------\n");
    printf("- Reserve:      %d\n", ms5611_prom.prom_Reserve);
    printf("- C1:           %d\n", ms5611_prom.prom_C1);
    printf("- C2:           %d\n", ms5611_prom.prom_C2);
    printf("- C3:           %d\n", ms5611_prom.prom_C3);
    printf("- C4:           %d\n", ms5611_prom.prom_C4);
    printf("- C5:           %d\n", ms5611_prom.prom_C5);
    printf("- C6:           %d\n", ms5611_prom.prom_C6);
    printf("- CRC:          %d\n", ms5611_prom.prom_CRC);
    printf("- Temperature:  %.4f °C\n", MS_Temperature());
    printf("- Pressure raw: %.4f kPa\n", MS_Pressure());
    printf("- Height Hypso: %.4f m\n", MS_Height_Hypsometric());
    printf("- Height Barom: %.4f m\n", MS_Height_Barometric());
    printf("--------------------------------------------------\n");
}


// 北京市 朝阳区平均海拔为34米


int32_t MS_Temperature_int32_t(void)
{
    return 2000 + ((int64_t) ms5611_data.data_dT * ms5611_prom.prom_C6) / 8388608.0f;
}

float MS_Temperature(void)
{
    return ms5611_data.data_Temperature_inFloat;
}

float Fusion_Temp(void)
{
    return (float) (MS_Temperature() * 0.85f + GY_Temp() * 0.15f);
}

float MS_Pressure(void)
{
    return (float) ms5611_data.data_Pressure_inFloat;
}

float MS_Height(void)
{
    //return (float) ms5611_data.data_height + Drone_offset.offset_height_MS;

    return ms5611_data.data_height;
}

float MS_Height_Hypsometric(void)
{
    return (float) ((powf((MS5611_Standard_Bar / MS_Pressure()), MS5611_Powf_exponent1) - 1.0f) 
                        * (MS_Temperature() + MS5611_Kelvin_Temp)) / 0.0065f;
}

float MS_Height_Barometric(void)
{
    return (float) 44330.0f * (1.0f - powf((MS_Pressure() / MS5611_Standard_Bar), MS5611_Powf_exponent2));
}


float MS5611_firstOrder_Filtering(float x_data, float coff, float y_data)
{
    return x_data * coff + (1.0f - coff) * y_data;
}


uint8_t MS5611_CRC(uint16_t *prom)
{
    int32_t i, j;
    uint32_t res = 0;
    uint8_t zero = 1;
    uint8_t crc = prom[7] & 0xF;
    prom[7] &= 0xFF00;
 
    // if eeprom is all zeros, we're probably fucked - BUT this will return valid CRC lol
    for(i = 0; i < 8; i++) 
	{
        if (prom[i] != 0)
            zero = 0;
    }
 
    if(zero)
        return I2C_Fail;
 
    for(i = 0; i < 16; i++)
    {
        if (i & 1)
            res ^= ((prom[i >> 1]) & 0x00FF);
        else
            res ^= (prom[i >> 1] >> 8);
        for (j = 8; j > 0; j--) {
            if (res & 0x8000)
                res ^= 0x1800;
            res <<= 1;
        }
    }
 
    prom[7] |= crc;
    if (crc == ((res >> 12) & 0xF))
        return (I2C_Success);
    
	return (I2C_Fail);
}

    /*
    if(MS_Temperature() < 20.0f)
    {
        ms5611_data.data_Temperature2 = (ms5611_data.data_dT * ms5611_data.data_dT) / 0x80000000;
        ms5611_data.data_OFF2 = (5 * (ms5611_data.data_Temperature - 2000) * (ms5611_data.data_Temperature - 2000)) / 2;
        ms5611_data.data_SENS2 = (5 * (ms5611_data.data_Temperature - 2000) * (ms5611_data.data_Temperature - 2000)) / 4;


        if(MS_Temperature() < -15.0f)
        {
            ms5611_data.data_OFF2 += (7 * (ms5611_data.data_Temperature + 1500) * (ms5611_data.data_Temperature + 1500));
            ms5611_data.data_SENS2 += (11 * (ms5611_data.data_Temperature + 1500) * (ms5611_data.data_Temperature + 1500)) / 2;
        }
    }
    else
    {
        ms5611_data.data_Temperature2 = 0;
        ms5611_data.data_OFF2 = 0;
        ms5611_data.data_SENS2 = 0;
    }

    ms5611_data.data_Temperature -= ms5611_data.data_Temperature2;
    ms5611_data.data_OFF -= ms5611_data.data_OFF2;
    ms5611_data.data_SENS -= ms5611_data.data_SENS2;
    */

