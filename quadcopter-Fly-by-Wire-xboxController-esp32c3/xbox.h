#ifndef __XBOX_H__
#define __XBOX_H__

#include "Arduino.h"

#include "XboxSeriesXControllerESP32_asukiaaa.hpp"
#include "Preferences.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/FreeRTOSConfig.h"


#include "nrf24.h"
#include "oled.h"
#include "nrf24l01p.h"


// Mac 68:6c:e6:42:57:89

#define XBOX_CONNECTED                ((uint8_t) 1)
#define XBOX_UNCONNECT                ((uint8_t) 0)

#define XBOX_ModeSelectPin            10

#define XBOX_ModeSetting              ((uint8_t) 0)
#define XBOX_ModeNormal               ((uint8_t) 1)

#define XBOX_Max_Maclist              20
#define XBOX_Max_Macsize              18

#define XBOX_Serial_Timeout           50

#define XBOX_DEBUG                    XBOX_ModeNormal

/*
typedef struct
{
  bool btnA;
  bool btnB;
  bool btnX;
  bool btnY;
  bool btnShare;    // 上传/分享
  bool btnStart;    // 设置菜单
  bool btnSelect;   // 复制
  bool btnXbox;     // 主按键
  bool btnLB; // side top button
  bool btnRB;	// side top button
  bool btnLS; // button on joy stick
  bool btnRS;	// button on joy stick
  bool btnDirUp;
  bool btnDirLeft;
  bool btnDirRight;
  bool btnDirDown;
  uint16_t joyLHori = 0xffff / 2;
  uint16_t joyLVert = 0xffff / 2;
  uint16_t joyRHori = 0xffff / 2;
  uint16_t joyRVert = 0xffff / 2;
  uint16_t trigLT, trigRT;
} XBOX_DATAPACK;
*/
extern String MAC_Address_Selected;
extern uint8_t xbox_boot_mode;
extern uint8_t XBOX_isConnect;
extern uint8_t data_exchange_lock;


void xbox_setup(void);

void xbox_boot(void);
void xbox_setting(void);

void xbox_operation(void);

void xbox_setting_showlist(uint8_t mac_num);
void xbox_setting_ref(void);

void xbox_get_SerialCommand(String *cmd_str, char *cmd_key);
uint8_t xbox_check_mac_list_status(void);
void xbox_get_All_parameters(String cmd_string, char cmd_key, String *param1, String *param2, String *param3);
bool xbox_get_mac_valid(String mac);
uint8_t xbox_get_mac_total(void);


#endif
