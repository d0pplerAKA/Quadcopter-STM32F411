#ifndef __GL9306_H__
#define __GL9306_H__


#include "stm32f4xx.h"
#include "stdio.h"


#define GL9306_RX_GPIO                  GPIO_Pin_3
#define GL9306_BaudRate                 19200
#define GL9306_BufferSize               9
#define GL9306_Sampling_Time            25

#define GL9306_Valid                    0
#define GL9306_Invalid                  1


typedef struct
{
    int16_t d_x;
    int16_t d_y;

    uint8_t envir_quality;

    float dt;

    float d_h;

    float speed_x;
    float speed_y;

    float sum_flow_x;
    float sum_flow_y;
    /* data */
} GL9306_DISPLACEMENT_t;

extern GL9306_DISPLACEMENT_t gl9306_displacement;

extern uint8_t GL9306_TEMP_DATA_PACK[GL9306_BufferSize];
extern uint8_t GL9306_DATA_PACK[GL9306_BufferSize];

void GL9306_Init(void);
void GL_RefreshData(void);
void GL9306_toString(void);

int16_t GL_X(void);
int16_t GL_Y(void);
uint8_t GL_ENV(void);

float GL_SpeedX(void);
float GL_SpeedY(void);
float GL_DisX(void);
float GL_DisY(void);


uint8_t GL9306_Pack_cmp(uint8_t dx_h, uint8_t dx_l, uint8_t dy_h, uint8_t dy_l, uint8_t crc);


#endif
