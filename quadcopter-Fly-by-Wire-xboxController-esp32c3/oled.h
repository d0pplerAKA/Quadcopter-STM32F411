#ifndef __OLED_H__
#define __OLED_H__

#include <string.h>
#include <stdlib.h>

#include "Arduino.h"

#include "U8g2lib.h"
#ifdef U8X8_HAVE_HW_I2C
  #include "Wire.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "xbox.h"
#include "nrf24.h"

#define OLED_Setfont_6x10()                 u8g2.setFont(u8g2_font_6x10_tf)
#define OLED_Setfont_13x13()                u8g2.setFont(u8g2_font_wqy12_t_gb2312)
#define OLED_Setfont_13x16()                u8g2.setFont(u8g2_font_wqy13_t_gb2312)
#define OLED_Setfont_18x18()                u8g2.setFont(u8g2_font_wqy16_t_gb2312)

#define Volts_MAX                           (float ) (12.6f)
#define Volts_MIN                           (float ) (11.1f)

//#define OLED_Rolling_Block                  (uint8_t ) (12)

#define OLED_Sampling_Rate                  (uint32_t ) (71)
#define OLED_Sampling_Rate_Perc             (uint32_t ) (40)




extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;


typedef struct
{
    uint8_t oled_page_depth;
    uint8_t oled_index_H;
    uint8_t oled_index_V;
    uint8_t oled_index_V2;
    uint8_t oled_index_V3;

    uint8_t send_op;
    uint8_t is_control_message;
    uint8_t is_setting_message;

    uint8_t key_up;
    uint8_t key_down;
    uint8_t key_left;
    uint8_t key_right;

    uint8_t key_A;
    uint8_t key_B;

    uint8_t key_LB;
    uint8_t key_RB;

} OLED_Pref_t;


extern OLED_Pref_t oled_pref_struct;

extern uint8_t nrf24l01p_channel;

extern float pid_roll_Kp;
extern float pid_roll_Ki;
extern float pid_roll_Kd;

extern float pid_pitch_Kp;
extern float pid_pitch_Ki;
extern float pid_pitch_Kd;

extern float pid_yaw_Kp;
extern float pid_yaw_Ki;
extern float pid_yaw_Kd;


void oled_setup(void);

void oled_boot_operation(void *pt);
void oled_operation(void *pt);

void draw_setting_frame(String title, float min_value, float max_value, float step, float *current_value, uint8_t* channel_flag, uint8_t is_float);
void draw_control_system_frame(void);
void draw_menu_title(void);
void draw_menu_title_2nd(void);
void draw_menu_title_3rd(String str_title);
void draw_menu_text(void);
void draw_menu_text_2nd(void);
void draw_menu_text_3rd(String str_text);
void draw_rolling(uint8_t start_index);  // 0 12 24 36
uint8_t map_index(float volts);
void draw_battery(uint8_t index, uint8_t toggle_on);
void draw_ble(uint8_t toggle_on);
void draw_wifi(uint8_t toggle_on);
void draw_loss(float loss);
void draw_depth(void);
float floatMap(float x, float in_min, float in_max, float out_min, float out_max);


#endif
