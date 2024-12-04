#include "gy_95t.h"


GY_RAW gy_raw;
GY_95T gy_95t;

int16_t GY_ROLL_FIFO[GY95T_FIFO_SIZE];
int16_t GY_Pitch_FIFO[GY95T_FIFO_SIZE];


uint8_t GY_Init(void)
{
    memset(GY_ROLL_FIFO, 0, GY95T_FIFO_SIZE * sizeof(int16_t));
    memset(GY_Pitch_FIFO, 0, GY95T_FIFO_SIZE * sizeof(int16_t));

    uint8_t error_type;

    error_type = I2C_WriteByte(I2C_PORT_GY, GY95T_ADDR, GY95T_UPDATE_RATE, GY95T_UPDATE_RATE_SET);
	if(error_type != I2C_Success)
    {
        return I2C_Fail;
    }
    delay_ms(10);

    error_type = I2C_WriteByte(I2C_PORT_GY, GY95T_ADDR, GY95T_OUTPUT_MODE, GY95T_OUTPUT_MODE_DEFAULT);
    if(error_type != I2C_Success)
    {
        return I2C_Fail;
    }
    delay_ms(10);
		
    return I2C_Success;
}

uint8_t GY_RefreshData(void)
{   
    uint8_t error_type;
    uint8_t raw_data[21];

    error_type = I2C_ReadBytes(I2C_PORT_GY, GY95T_ADDR, GY95T_ACC_X_L, raw_data, 21);
    if(error_type != 0)
    {
        printf("error %d\n", error_type);

        I2C1_Init();
        if(GY_Init() != 0)
        {
            printf("GY Fatal Exception. Flight might crushed. \n");

            // Blackbox functions implement here

            return I2C_Fail;
        }

        // Flameout in flight
        // Quadcopter should be never access in to this if condition...
    }

    memcpy(&gy_raw, raw_data, 21);

    int16_t temp_roll = 0, temp_pitch = 0;

    for(uint8_t i = GY95T_FIFO_SIZE - 1; i > 0; i--)
    {
        GY_ROLL_FIFO[i] = GY_ROLL_FIFO[i - 1];
        GY_Pitch_FIFO[i] = GY_Pitch_FIFO[i - 1];

        temp_roll += GY_ROLL_FIFO[i];
        temp_pitch += GY_Pitch_FIFO[i];
    }

    GY_ROLL_FIFO[0] = gy_raw.raw_roll;
    GY_Pitch_FIFO[0] = gy_raw.raw_pitch;

    temp_roll += gy_raw.raw_roll;
    temp_pitch += gy_raw.raw_pitch;

    temp_roll = (temp_roll) / (float) (GY95T_FIFO_SIZE);
    temp_pitch = (temp_pitch) / (float) (GY95T_FIFO_SIZE);

    FWB_LPF_onLoop(&lpf_roll_degree, temp_roll / 100.0f);
    FWB_LPF_onLoop(&lpf_pitch_degree, temp_pitch / 100.0f);

    gy_95t.mpu_roll = lpf_roll_degree.output;
    gy_95t.mpu_pitch = lpf_pitch_degree.output;
    
    return I2C_Success;
}

float GY_Accel_x(void)
{
    gy_95t.mpu_accel_x = (float) gy_raw.raw_accel_x / 100.0f;

    return gy_95t.mpu_accel_x;
}

float GY_Accel_y(void)
{
    gy_95t.mpu_accel_y = (float) gy_raw.raw_accel_y / 100.0f;

    return gy_95t.mpu_accel_y;
}

float GY_Accel_z(void)
{
    gy_95t.mpu_accel_z = (float) gy_raw.raw_accel_z / 100.0f;

    return gy_95t.mpu_accel_z;
}

float GY_Gyro_x(void)
{
    gy_95t.mpu_gyro_x = (float) gy_raw.raw_gyro_x / 100.0f;

    return gy_95t.mpu_gyro_x;
}

float GY_Gyro_y(void)
{
    gy_95t.mpu_gyro_y = (float) gy_raw.raw_gyro_y / 100.0f;

    return gy_95t.mpu_gyro_y;
}

float GY_Gyro_z(void)
{
    gy_95t.mpu_gyro_z = (float) gy_raw.raw_gyro_z / 100.0f;

    return gy_95t.mpu_gyro_z;
}

float GY_Roll(void)
{
    //gy_95t.mpu_roll = (float) gy_raw.raw_roll / 100.0f;

    return gy_95t.mpu_roll - Drone_offset.offset_roll;
}

float GY_Pitch(void)
{
    //gy_95t.mpu_pitch = (float) gy_raw.raw_pitch / 100.0f;

    return gy_95t.mpu_pitch - Drone_offset.offset_pitch;
}

float GY_Yaw(void)
{
    gy_95t.mpu_yaw = (float) gy_raw.raw_yaw / 100.0f;

    return gy_95t.mpu_yaw;
}

uint8_t GY_MFC(void)
{
    gy_95t.mpu_mfc = gy_raw.raw_mfc;

    return gy_95t.mpu_mfc;
}

float GY_Temp(void)
{
    gy_95t.mpu_temp = (float) gy_raw.raw_temp / 100.0f;

    return gy_95t.mpu_temp;
}


void GY_toString(void)
{
    printf("--------------------------------------------------\n");
    printf("- Accel X:  %.2f\n", GY_Accel_x());
    printf("- Accel Y:  %.2f\n", GY_Accel_y());
    printf("- Accel Z:  %.2f\n", GY_Accel_z());
    printf("- Gyro X:   %.2f\n", GY_Gyro_x());
    printf("- Gyro Y:   %.2f\n", GY_Gyro_y());
    printf("- Gyro Z:   %.2f\n", GY_Gyro_z());
    printf("- Roll:     %.2f\n", GY_Roll());
    printf("- Pitch:    %.2f\n", GY_Pitch());
    printf("- Yaw:      %.2f\n", GY_Yaw());
    printf("- MFC:      %d\n", GY_MFC());
    printf("- Temp:     %.2f\n", GY_Temp());
    printf("--------------------------------------------------\n");
}
