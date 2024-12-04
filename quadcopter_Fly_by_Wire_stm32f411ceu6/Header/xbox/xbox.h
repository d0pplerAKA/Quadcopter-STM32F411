#ifndef __XBOX_H__
#define __XBOX_H__

#include <stdint.h>

#include "i2c/i2c.h"
#include "timer/timer.h"


#define NRF_PID_Roll                    0
#define NRF_PID_Pitch                   1
#define NRF_PID_Yaw                     2



typedef struct
{
    int16_t roll;                       // 2 bytes
    int16_t pitch;                      // 2 bytes
    int16_t yaw;                        // 2 bytes

    int16_t temp;                       // 2 bytes
    float height;                       // 4 bytes
    float volt;                         // 4 bytes

    float acc_L1;                       // 4 bytes
    float acc_L2;                       // 4 bytes
    float acc_R1;                       // 4 bytes
    float acc_R2;                       // 4 bytes

} __attribute__((packed)) NRF_RECEIVE_RAW;                      // 32 bytes in total


typedef struct
{
    uint8_t is_control_message;
    // 1 bytes

    uint8_t btnA;
    uint8_t btnB;
    uint8_t btnX;
    uint8_t btnY;
    uint8_t btnShare;                   // 上传/分享
    uint8_t btnStart;                   // 设置菜单
    uint8_t btnSelect;                  // 复制
    uint8_t btnXbox;                    // 主按键
    uint8_t btnLB;                      // side top button
    uint8_t btnRB;                      // side top button
    uint8_t btnLS;                      // button on joy stick
    uint8_t btnRS;                      // button on joy stick
    uint8_t btnDirUp;
    uint8_t btnDirLeft;
    uint8_t btnDirRight;
    uint8_t btnDirDown;
    // 16 bytes

    uint16_t joyLHori;                  // 2 bytes
    uint16_t joyLVert;                  // 2 bytes
    uint16_t joyRHori;                  // 2 bytes
    uint16_t joyRVert;                  // 2 bytes
    uint16_t trigLT;                    // 2 bytes
    uint16_t trigRT;                    // 2 bytes
    // 12 bytes

    uint8_t note1;
    uint8_t note2;
    uint8_t note3;
    // 3 bytes

} __attribute__((packed)) NRF_SEND_RAW;                         // 32 bytes in total

typedef struct
{
    uint8_t is_setting_message;
    uint8_t pid_type;
    // 2 bytes

    float temp_kp;
    float temp_ki;
    float temp_kd;
    // 12 bytes

    uint32_t note0;
    uint32_t note1;
    uint32_t note2;
    uint32_t note3;
    uint16_t note4;
    // 18 bytes

} __attribute__((packed)) NRF_MANUAL_RAW;


typedef struct
{
    float JoyLeftV;

    float JoyRightH;
    float JoyRightV;

    float TrigLeft;
    float TrigRight;
    /* data */
} JOY_STICK_t;



extern uint8_t data_exchange_lock_receive;
extern uint8_t data_exchange_lock_send_ack;


extern uint8_t ws2812_mode;
extern uint8_t drone_hover_switch;


extern volatile uint8_t xbox_abort_flag;
extern volatile uint8_t xbox_recovery_flag;
extern volatile uint8_t xbox_upload_parameter_flag;


extern NRF_RECEIVE_RAW nrf_receive_raw;
extern NRF_SEND_RAW nrf_send_raw;
extern NRF_MANUAL_RAW nrf_manual_raw;

extern JOY_STICK_t nrf_joy_stick;


extern const uint8_t nrf_default_signal[32];




void XBOX_IRQHandler(void);

void XBOX_JoyStick_Convert(JOY_STICK_t * joy);

void XBOX_WS2812_Controll(uint8_t ws2812_mode);


#endif
