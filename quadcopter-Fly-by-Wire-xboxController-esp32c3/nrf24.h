#ifndef __NRF24_H__
#define __NRF24_H__

#include "Arduino.h"
#include "SPI.h"
#include "Preferences.h"

#include "nrf24l01p.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "xbox.h"
#include "oled.h"

#define NRF_MISO                        3
#define NRF_MOSI                        4
#define NRF_SCLK                        5
#define NRF_NSS                         6

#define NRF_CE                          7
#define NRF_IRQ                         10

#define SPI_Speed                       8000000
#define NRF_Speed                       75

#define NRF_IRQ_Install()               attachInterrupt(digitalPinToInterrupt(NRF_IRQ), nrf_irq_operation, FALLING)
#define NRF_IRQ_Uninstall()             detachInterrupt(digitalPinToInterrupt(NRF_IRQ))

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

    uint16_t joyLHori = 0xffff / 2;     // 2 bytes
    uint16_t joyLVert = 0xffff / 2;     // 2 bytes
    uint16_t joyRHori = 0xffff / 2;     // 2 bytes
    uint16_t joyRVert = 0xffff / 2;     // 2 bytes
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


extern Preferences NRF24_Channel_Pref;

extern NRF_RECEIVE_RAW nrf_receive_raw;
extern NRF_SEND_RAW nrf_send_raw;
extern NRF_MANUAL_RAW nrf_manual_raw;

extern TaskHandle_t nrf24_send_op_handle;
extern TaskHandle_t nrf24_recv_op_handle;



void NRF_SPI_setup(void);

void NRF_Writebyte(uint8_t command, uint8_t reg_data);
void NRF_Writebytes(uint8_t command, uint8_t *reg_data, uint8_t data_len);
void NRF_Readbyte(uint8_t command, uint8_t *data_p, uint8_t dummy_byte);
void NRF_Readbytes(uint8_t command, uint8_t *data_p, uint8_t data_len, uint8_t dummy_byte);

void nrf_send_operation(void *pt);
void nrf_recv_operation(void *pt);
void IRAM_ATTR nrf_irq_operation(void);


#endif
