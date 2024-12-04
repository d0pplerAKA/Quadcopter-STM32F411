#include "gl9306.h"

uint8_t GL9306_TEMP_DATA_PACK[GL9306_BufferSize];
uint8_t GL9306_DATA_PACK[GL9306_BufferSize];

GL9306_DISPLACEMENT_t gl9306_displacement;

int16_t GL9306_X_FIFO[GL9306_BufferSize];
int16_t GL9306_Y_FIFO[GL9306_BufferSize];



void GL9306_Init(void)
{
    gl9306_displacement.d_x = 0;
    gl9306_displacement.d_y = 0;

    gl9306_displacement.envir_quality = 0;

    gl9306_displacement.d_h = 0.0f;
    gl9306_displacement.speed_x = 0.0f;
    gl9306_displacement.speed_y = 0.0f;
    gl9306_displacement.sum_flow_x = 0.0f;
    gl9306_displacement.sum_flow_y = 0.0f;

    gl9306_displacement.dt = 1.0f / (float) GL9306_Sampling_Time;

    printf("GL Init.\n");
}

void GL9306_toString(void)
{
    printf("--------------------------------------------------\n");
    for(uint8_t i = 0; i < GL9306_BufferSize; i++)
        printf("%x ", GL9306_DATA_PACK[i]);
    printf("\nX Displacement:         %d\n", gl9306_displacement.d_x);
    printf("Y Displacement:         %d\n", gl9306_displacement.d_y);
    printf("Environment Quality:    %d\n", gl9306_displacement.envir_quality);
    printf("--------------------------------------------------\n");
}

void GL_RefreshData(void)
{
    uint8_t crc = GL9306_DATA_PACK[6];

    if(GL9306_Pack_cmp(GL9306_DATA_PACK[3], GL9306_DATA_PACK[2], GL9306_DATA_PACK[5], GL9306_DATA_PACK[4], crc) == 0)
    {
        if(gl9306_displacement.d_h >= 155.0f && gl9306_displacement.d_h <= 2500.0f)
        {
            int16_t temp_dx = (((int16_t) GL9306_DATA_PACK[3]) << 8) | ((int16_t) GL9306_DATA_PACK[2]);
            int16_t temp_dy = (((int16_t) GL9306_DATA_PACK[5]) << 8) | ((int16_t) GL9306_DATA_PACK[4]);

            if(temp_dx >= 16 || temp_dx <= -16) temp_dx = 0;
            if(temp_dy >= 16 || temp_dy <= -16) temp_dy = 0;


            int16_t temp_x_fifo_sum = 0;
            int16_t temp_y_fifo_sum = 0;

            for(uint8_t i = GL9306_BufferSize - 1; i > 0; i--)
            {
                GL9306_X_FIFO[i] = GL9306_X_FIFO[i - 1];
                GL9306_Y_FIFO[i] = GL9306_Y_FIFO[i - 1];

                temp_x_fifo_sum += GL9306_X_FIFO[i];
                temp_y_fifo_sum += GL9306_Y_FIFO[i];
            }

            GL9306_X_FIFO[0] = temp_dx;
            GL9306_Y_FIFO[0] = temp_dy;

            gl9306_displacement.d_x = temp_x_fifo_sum / GL9306_BufferSize;
            gl9306_displacement.d_y = temp_y_fifo_sum / GL9306_BufferSize;
            gl9306_displacement.envir_quality = GL9306_DATA_PACK[7];
            
            gl9306_displacement.speed_x = (((float) gl9306_displacement.d_x / 100.0f * gl9306_displacement.d_h) / gl9306_displacement.dt);
            gl9306_displacement.speed_y = (((float) gl9306_displacement.d_y / 100.0f * gl9306_displacement.d_h) / gl9306_displacement.dt);

            gl9306_displacement.sum_flow_x += gl9306_displacement.speed_x * gl9306_displacement.dt;
            gl9306_displacement.sum_flow_y += gl9306_displacement.speed_y * gl9306_displacement.dt;
        }
        else
        {
            gl9306_displacement.d_x = 0;
            gl9306_displacement.d_y = 0;

            gl9306_displacement.speed_x = 0;
            gl9306_displacement.speed_y = 0;
            gl9306_displacement.sum_flow_x = 0;
            gl9306_displacement.sum_flow_y = 0;

            gl9306_displacement.envir_quality = GL9306_DATA_PACK[7];
        }
    }
    else
    {
        printf("Error!\n");
        gl9306_displacement.d_x = 0;
        gl9306_displacement.d_y = 0;

        gl9306_displacement.speed_x = 0;
        gl9306_displacement.speed_y = 0;
        gl9306_displacement.sum_flow_x = 0;
        gl9306_displacement.sum_flow_y = 0;

        gl9306_displacement.envir_quality = GL9306_DATA_PACK[7];
    }

}

int16_t GL_X(void)
{
    return gl9306_displacement.d_x;
}

int16_t GL_Y(void)
{
    return gl9306_displacement.d_y;
}

uint8_t GL_ENV(void)
{
    return gl9306_displacement.envir_quality;
}

float GL_SpeedX(void)
{
    return gl9306_displacement.speed_x;
}

float GL_SpeedY(void)
{
    return gl9306_displacement.speed_y;
}

float GL_DisX(void)
{
    return gl9306_displacement.sum_flow_x;
}

float GL_DisY(void)
{
    return gl9306_displacement.sum_flow_y;
}

uint8_t GL9306_Pack_cmp(uint8_t dx_h, uint8_t dx_l, uint8_t dy_h, uint8_t dy_l, uint8_t crc)
{
    uint8_t temp_crc;

    temp_crc = (uint8_t) (dx_h + dx_l + dy_h + dy_l);

    if(temp_crc == crc) return GL9306_Valid;
    else                return GL9306_Invalid;
}

