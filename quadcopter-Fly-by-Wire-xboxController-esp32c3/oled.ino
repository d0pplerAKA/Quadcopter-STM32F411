#include "oled.h"

OLED_Pref_t oled_pref_struct;

uint8_t nrf24l01p_channel;

float pid_roll_Kp;
float pid_roll_Ki;
float pid_roll_Kd;

float pid_pitch_Kp;
float pid_pitch_Ki;
float pid_pitch_Kd;

float pid_yaw_Kp;
float pid_yaw_Ki;
float pid_yaw_Kd;




//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 0, 1);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 0, 1);



uint8_t in_pid_setting = 0;



void oled_setup(void)
{
    memset(&oled_pref_struct, 0, sizeof(uint8_t) * 16);

    u8g2.setBusClock(400000);

    u8g2.begin();

    //OLED_Setfont_13x13();
    //OLED_Setfont_13x16();
    //OLED_Setfont_18x18();

    u8g2.setFontDirection(0);
}

void oled_boot_operation(void *pt)
{
    String temp_str = "";

    while(1)
    {
        temp_str = MAC_Address_Selected;

        u8g2.clearBuffer();

        u8g2.home();
        OLED_Setfont_18x18();
        u8g2.setCursor(0, 18);
        u8g2.print("[Boot Mode]");
        u8g2.setCursor(3, 36);
        u8g2.print("Check Serial.");
        u8g2.setCursor(3, 54);
        u8g2.print("Mac:");

        OLED_Setfont_13x13();
        u8g2.setCursor(38, 56);
        u8g2.print(temp_str);

        u8g2.sendBuffer();
        vTaskDelay(950 / portTICK_RATE_MS);


        u8g2.clearBuffer();

        u8g2.home();
        OLED_Setfont_18x18();
        u8g2.setCursor(0, 18);
        u8g2.print("[Boot Mode]");
        u8g2.setCursor(3, 36);
        u8g2.print("Check Serial.");
        u8g2.setCursor(3, 54);
        u8g2.print("Mac:");

        u8g2.sendBuffer();
        vTaskDelay(380 / portTICK_RATE_MS);
    }
}


void oled_operation(void *pt)
{
    uint8_t isConnected = 0;
    uint8_t nrf24_status = 0;
    uint8_t battery_status = 0;
    
    /*
    oled_pref_struct.oled_page_depth = 0;
    oled_pref_struct.oled_index_H = 0;
    oled_pref_struct.oled_index_V = 0;
    oled_pref_struct.send_op = 0;
    */

    uint8_t channel_changed = 0;

    uint8_t pid_roll_Kp_changed = 0;
    uint8_t pid_roll_Ki_changed = 0;
    uint8_t pid_roll_Kd_changed = 0;


    uint8_t pid_pitch_Kp_changed = 0;
    uint8_t pid_pitch_Ki_changed = 0;
    uint8_t pid_pitch_Kd_changed = 0;

    uint8_t pid_yaw_Kp_changed = 0;
    uint8_t pid_yaw_Ki_changed = 0;
    uint8_t pid_yaw_Kd_changed = 0;


    uint32_t time_stamp = 0;

    uint8_t pack_total = 0;
    uint8_t pack_lost = 0;

    float rate_loss = 0;

    while(1)
    {
        isConnected = XBOX_isConnect;
        nrf24_status = nrf_status;
        battery_status = map_index(nrf_receive_raw.volt);

        u8g2.clearBuffer();

        draw_menu_title();

        if(!isConnected)
        {
            uint8_t toggle = 0;
            uint32_t prec = time_stamp % OLED_Sampling_Rate;
            
            if(prec > OLED_Sampling_Rate_Perc)
                toggle = 1;

            draw_ble(toggle);
        } else draw_ble(1);

        if(nrf24_status != 96)
        {
            pack_lost++;

            nrf_receive_raw.volt = 0;
            nrf_status = 0;

            uint8_t toggle = 0;
            uint32_t prec = time_stamp % OLED_Sampling_Rate;
            
            if(prec > OLED_Sampling_Rate_Perc)
                toggle = 1;
            
            draw_wifi(toggle);
        } else draw_wifi(1);

        if(battery_status < 2)
        {
            uint8_t toggle = 0;
            uint32_t prec = time_stamp % OLED_Sampling_Rate;
            
            if(prec > OLED_Sampling_Rate_Perc)
                toggle = 1;
            
            draw_battery(battery_status, toggle);
        } else draw_battery(battery_status, 1);

        if(pack_total == 100)
        {
            rate_loss = (float) pack_lost / 100.0f;

            pack_total = 0;
            pack_lost = 0;
        }

        draw_loss(rate_loss);


        while(data_exchange_lock == 1);

        uint8_t k_A = oled_pref_struct.key_A;
        uint8_t k_B = oled_pref_struct.key_B;
        uint8_t k_Up = oled_pref_struct.key_up;
        uint8_t k_Left = oled_pref_struct.key_left;
        uint8_t k_Right = oled_pref_struct.key_right;
        uint8_t k_Down = oled_pref_struct.key_down;

        //draw_depth();

        switch(oled_pref_struct.oled_page_depth)                // 页面深度
        {
            case 0:                                                 //  主页 (页面深度 0)
            {
                draw_menu_text();                                       //  构建选项

                oled_pref_struct.send_op = 0;
                oled_pref_struct.is_control_message = 0;
                oled_pref_struct.is_setting_message = 0;                
                
                draw_rolling(oled_pref_struct.oled_index_V * 12);           //  框选图形

                if(k_Down == 1 && k_Up == 0)                                //  向下框选
                {    
                    if(oled_pref_struct.oled_index_V < 3)
                        oled_pref_struct.oled_index_V += 1;
                    
                    vTaskDelay(150 / portTICK_RATE_MS);
                }
                else if(k_Down == 0 && k_Up == 1)                           //  向上框选
                {
                    if(oled_pref_struct.oled_index_V > 0)
                        oled_pref_struct.oled_index_V -= 1;
                    
                    vTaskDelay(150 / portTICK_RATE_MS);
                }

                if(k_A == 1)                                                //  进入该页
                {
                    if(oled_pref_struct.oled_page_depth < 1)
                        oled_pref_struct.oled_page_depth += 1;
                    
                    vTaskDelay(150 / portTICK_RATE_MS);
                }

                break;
            }

            case 1:                                             // 页面深度1
            {
                if(k_B == 1 && oled_pref_struct.oled_index_V != 0 && oled_pref_struct.oled_index_V != 1 && oled_pref_struct.oled_index_V != 2) // 其它退出 页面深度 -1
                {
                    if(oled_pref_struct.oled_page_depth > 0)
                        oled_pref_struct.oled_page_depth -= 1;
                    
                    vTaskDelay(150 / portTICK_RATE_MS);
                }

                if(k_B == 1 && oled_pref_struct.oled_index_V != 0 && oled_pref_struct.oled_index_V == 1)    // 退出编辑频道 页面深度 -1
                {
                    if(channel_changed)
                    {
                        channel_changed = 0;

                        NRF24_Channel_Pref.begin("nrf channel");
                        NRF24_Channel_Pref.putUChar("nrf chn", nrf24l01p_channel);
                        NRF24_Channel_Pref.end();

                        NRF24_PowerDown();

                        NRF24_Init(NRF_PTX);

                        NRF24_Set_RF_Channel(nrf24l01p_channel);

                        NRF24_PowerUp();
                    }

                    if(oled_pref_struct.oled_page_depth > 0)
                        oled_pref_struct.oled_page_depth -= 1;
                    
                    vTaskDelay(150 / portTICK_RATE_MS);
                }
                
                if(k_B == 1 && oled_pref_struct.oled_index_V != 0 && oled_pref_struct.oled_index_V == 2)    // 退出 pid编辑 页面深度 -1
                {
                    in_pid_setting = 0;
                    
                    if(oled_pref_struct.oled_page_depth > 0)
                        oled_pref_struct.oled_page_depth -= 1;
                    
                    vTaskDelay(150 / portTICK_RATE_MS);
                }

                if(oled_pref_struct.key_LB == 1 && oled_pref_struct.key_RB == 1)    //退出控制台
                {
                    vTaskDelay(200 / portTICK_RATE_MS);

                    if(oled_pref_struct.key_LB == 1 && oled_pref_struct.key_RB == 1)
                    {
                        vTaskDelay(150 / portTICK_RATE_MS);

                        if(oled_pref_struct.key_LB == 1 && oled_pref_struct.key_RB == 1)
                        {
                            vTaskDelay(150 / portTICK_RATE_MS);

                            if(oled_pref_struct.key_LB == 1 && oled_pref_struct.key_RB == 1)
                            {                                
                                nrf_status = 0;
                                battery_status = 0; nrf_receive_raw.volt = 0;

                                oled_pref_struct.send_op = 0;
                                oled_pref_struct.is_control_message = 0;
                                oled_pref_struct.is_setting_message = 0;

                                vTaskDelay(150 / portTICK_RATE_MS);

                                memset(nrf_temp_tx_ram, 0, sizeof(uint8_t) * 32);

                                for(uint8_t ijk = 0; ijk < 5; ijk++)
                                {
                                    NRF_CE_LOW();

                                    NRF24_Load_payload(nrf_temp_tx_ram);

                                    NRF_CE_HIGH();

                                    vTaskDelay(100 / portTICK_RATE_MS);
                                }

                                NRF_CE_LOW();

                                if(oled_pref_struct.oled_page_depth > 0)
                                    oled_pref_struct.oled_page_depth -= 1;
                            }
                        }
                    }
                }


                switch(oled_pref_struct.oled_index_V)                               // 页面类型判断
                {
                    case 0:
                    {
                        oled_pref_struct.send_op = 1;
                        oled_pref_struct.is_control_message = 1;
                        oled_pref_struct.is_setting_message = 0;

                        draw_control_system_frame();
                        
                        break;
                    }

                    case 1:
                    {
                        float temp_val = (float) nrf24l01p_channel;

                        draw_setting_frame("NRF Channel", 0, 126, 1, &temp_val, &channel_changed, 0);

                        nrf24l01p_channel = temp_val;

                        break;
                    }

                    case 2:
                    {
                        draw_menu_title_2nd();
                        draw_menu_text_2nd();

                        draw_rolling(oled_pref_struct.oled_index_V2 * 12);           //  框选图形

                        if(k_Down == 1 && k_Up == 0)                                //  向下框选
                        {    
                            if(oled_pref_struct.oled_index_V2 < 2)
                                oled_pref_struct.oled_index_V2 += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }
                        else if(k_Down == 0 && k_Up == 1)                           //  向上框选
                        {
                            if(oled_pref_struct.oled_index_V2 > 0)
                                oled_pref_struct.oled_index_V2 -= 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }

                        if(k_A == 1)                                                //  进入该页
                        {
                            if(oled_pref_struct.oled_page_depth < 2)
                                oled_pref_struct.oled_page_depth += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }

                        break;
                    }

                    case 3:
                    {

                        break;
                    }
                }

                break;
            }

            case 2:                                                                 // 页面深度 2
            {
                if(k_B == 1)
                {
                    if(oled_pref_struct.oled_page_depth > 0)
                        oled_pref_struct.oled_page_depth -= 1;
                    
                    vTaskDelay(150 / portTICK_RATE_MS);
                }

                switch(oled_pref_struct.oled_index_V2)
                {
                    case 0:
                    {
                        draw_menu_title_3rd("Roll");
                        draw_menu_text_3rd("Roll");

                        draw_rolling(oled_pref_struct.oled_index_V3 * 12);           //  框选图形

                        if(k_Down == 1 && k_Up == 0)                                //  向下框选
                        {    
                            if(oled_pref_struct.oled_index_V3 < 2)
                                oled_pref_struct.oled_index_V3 += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }
                        else if(k_Down == 0 && k_Up == 1)                           //  向上框选
                        {
                            if(oled_pref_struct.oled_index_V3 > 0)
                                oled_pref_struct.oled_index_V3 -= 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }

                        if(k_A == 1)                                                //  进入该页
                        {
                            if(oled_pref_struct.oled_page_depth < 3)
                                oled_pref_struct.oled_page_depth += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }
                        
                        break;
                    }

                    case 1:
                    {
                        draw_menu_title_3rd("Pitch");
                        draw_menu_text_3rd("Pitch");

                        draw_rolling(oled_pref_struct.oled_index_V3 * 12);           //  框选图形

                        if(k_Down == 1 && k_Up == 0)                                //  向下框选
                        {    
                            if(oled_pref_struct.oled_index_V3 < 2)
                                oled_pref_struct.oled_index_V3 += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }
                        else if(k_Down == 0 && k_Up == 1)                           //  向上框选
                        {
                            if(oled_pref_struct.oled_index_V3 > 0)
                                oled_pref_struct.oled_index_V3 -= 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }

                        if(k_A == 1)                                                //  进入该页
                        {
                            if(oled_pref_struct.oled_page_depth < 3)
                                oled_pref_struct.oled_page_depth += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }

                        break;
                    }

                    case 2:
                    {
                        draw_menu_title_3rd("Yaw");
                        draw_menu_text_3rd("Yaw");

                        draw_rolling(oled_pref_struct.oled_index_V3 * 12);           //  框选图形

                        if(k_Down == 1 && k_Up == 0)                                //  向下框选
                        {    
                            if(oled_pref_struct.oled_index_V3 < 2)
                                oled_pref_struct.oled_index_V3 += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }
                        else if(k_Down == 0 && k_Up == 1)                           //  向上框选
                        {
                            if(oled_pref_struct.oled_index_V3 > 0)
                                oled_pref_struct.oled_index_V3 -= 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }

                        if(k_A == 1)                                                //  进入该页
                        {
                            if(oled_pref_struct.oled_page_depth < 3)
                                oled_pref_struct.oled_page_depth += 1;
                            
                            vTaskDelay(150 / portTICK_RATE_MS);
                        }

                        break;
                    }


                    default:
                    {

                        break;
                    }
                }

                break;
            }

            case 3:
            {
                oled_pref_struct.is_control_message = 0;
                oled_pref_struct.is_setting_message = 1;

                if(k_B == 1)
                {
                    oled_pref_struct.is_control_message = 0;
                    oled_pref_struct.is_setting_message = 0;


                    if(pid_roll_Kp_changed)
                    {
                        pid_roll_Kp_changed = 0;

                        NRF24_PID_Kp[NRF_PID_Roll].begin("pid roll kp");
                        NRF24_PID_Kp[NRF_PID_Roll].putFloat("roll kp", pid_roll_Kp);
                        NRF24_PID_Kp[NRF_PID_Roll].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Roll;

                        nrf_manual_raw.temp_kp = pid_roll_Kp;
                        nrf_manual_raw.temp_ki = pid_roll_Ki;
                        nrf_manual_raw.temp_kd = pid_roll_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    if(pid_roll_Ki_changed)
                    {
                        pid_roll_Ki_changed = 0;

                        NRF24_PID_Ki[NRF_PID_Roll].begin("pid roll ki");
                        NRF24_PID_Ki[NRF_PID_Roll].putFloat("roll ki", pid_roll_Ki);
                        NRF24_PID_Ki[NRF_PID_Roll].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Roll;

                        nrf_manual_raw.temp_kp = pid_roll_Kp;
                        nrf_manual_raw.temp_ki = pid_roll_Ki;
                        nrf_manual_raw.temp_kd = pid_roll_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    if(pid_roll_Kd_changed)
                    {
                        pid_roll_Kd_changed = 0;

                        NRF24_PID_Kd[NRF_PID_Roll].begin("pid roll kd");
                        NRF24_PID_Kd[NRF_PID_Roll].putFloat("roll kd", pid_roll_Kd);
                        NRF24_PID_Kd[NRF_PID_Roll].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Roll;

                        nrf_manual_raw.temp_kp = pid_roll_Kp;
                        nrf_manual_raw.temp_ki = pid_roll_Ki;
                        nrf_manual_raw.temp_kd = pid_roll_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }
                    
                    if(pid_pitch_Kp_changed)
                    {
                        pid_pitch_Kp_changed = 0;

                        NRF24_PID_Kp[NRF_PID_Pitch].begin("pid pitch kp");
                        NRF24_PID_Kp[NRF_PID_Pitch].putFloat("pitch kp", pid_pitch_Kp);
                        NRF24_PID_Kp[NRF_PID_Pitch].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Pitch;

                        nrf_manual_raw.temp_kp = pid_pitch_Kp;
                        nrf_manual_raw.temp_ki = pid_pitch_Ki;
                        nrf_manual_raw.temp_kd = pid_pitch_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    if(pid_pitch_Ki_changed)
                    {
                        pid_pitch_Ki_changed = 0;

                        NRF24_PID_Ki[NRF_PID_Pitch].begin("pid pitch ki");
                        NRF24_PID_Ki[NRF_PID_Pitch].putFloat("pitch ki", pid_pitch_Ki);
                        NRF24_PID_Ki[NRF_PID_Pitch].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Pitch;

                        nrf_manual_raw.temp_kp = pid_pitch_Kp;
                        nrf_manual_raw.temp_ki = pid_pitch_Ki;
                        nrf_manual_raw.temp_kd = pid_pitch_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    if(pid_pitch_Kd_changed)
                    {
                        pid_pitch_Kd_changed = 0;

                        NRF24_PID_Kd[NRF_PID_Pitch].begin("pid pitch kd");
                        NRF24_PID_Kd[NRF_PID_Pitch].putFloat("pitch kd", pid_pitch_Kd);
                        NRF24_PID_Kd[NRF_PID_Pitch].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Pitch;

                        nrf_manual_raw.temp_kp = pid_pitch_Kp;
                        nrf_manual_raw.temp_ki = pid_pitch_Ki;
                        nrf_manual_raw.temp_kd = pid_pitch_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    if(pid_yaw_Kp_changed)
                    {
                        pid_yaw_Kp_changed = 0;

                        NRF24_PID_Kp[NRF_PID_Yaw].begin("pid yaw kp");
                        NRF24_PID_Kp[NRF_PID_Yaw].putFloat("yaw kp", pid_yaw_Kp);
                        NRF24_PID_Kp[NRF_PID_Yaw].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Yaw;

                        nrf_manual_raw.temp_kp = pid_yaw_Kp;
                        nrf_manual_raw.temp_ki = pid_yaw_Ki;
                        nrf_manual_raw.temp_kd = pid_yaw_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    if(pid_yaw_Ki_changed)
                    {
                        pid_yaw_Ki_changed = 0;

                        NRF24_PID_Ki[NRF_PID_Yaw].begin("pid yaw ki");
                        NRF24_PID_Ki[NRF_PID_Yaw].putFloat("yaw ki", pid_yaw_Ki);
                        NRF24_PID_Ki[NRF_PID_Yaw].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Yaw;

                        nrf_manual_raw.temp_kp = pid_yaw_Kp;
                        nrf_manual_raw.temp_ki = pid_yaw_Ki;
                        nrf_manual_raw.temp_kd = pid_yaw_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    if(pid_yaw_Kd_changed)
                    {
                        pid_yaw_Kd_changed = 0;

                        NRF24_PID_Kd[NRF_PID_Yaw].begin("pid yaw kd");
                        NRF24_PID_Kd[NRF_PID_Yaw].putFloat("yaw kd", pid_yaw_Kd);
                        NRF24_PID_Kd[NRF_PID_Yaw].end();

                        nrf_manual_raw.is_setting_message = 0x32;
                        nrf_manual_raw.pid_type = NRF_PID_Yaw;

                        nrf_manual_raw.temp_kp = pid_yaw_Kp;
                        nrf_manual_raw.temp_ki = pid_yaw_Ki;
                        nrf_manual_raw.temp_kd = pid_yaw_Kd;

                        memcpy(nrf_temp_tx_ram, &nrf_manual_raw, 32);

                        for(uint8_t i = 0; i < 3; i++)
                        {
                            NRF_CE_LOW();

                            NRF24_Load_payload(nrf_temp_tx_ram);

                            NRF_CE_HIGH();

                            vTaskDelay(100 / portTICK_RATE_MS);
                        }

                        NRF_CE_LOW();
                    }

                    /****/

                    if(oled_pref_struct.oled_page_depth > 0)
                        oled_pref_struct.oled_page_depth -= 1;

                    vTaskDelay(150 / portTICK_RATE_MS);
                }

                switch(oled_pref_struct.oled_index_V3)
                {
                    case 0:
                    {
                        switch(oled_pref_struct.oled_index_V2)
                        {
                            case 0:                 // Roll - Kp
                            {
                                float temp_val = pid_roll_Kp;

                                draw_setting_frame("Roll - Kp", 0.0f, 10.0f, 0.001f, &temp_val, &pid_roll_Kp_changed, 1);

                                pid_roll_Kp = temp_val;

                                break;
                            }

                            case 1:                 // Pitch - Kp
                            {
                                float temp_val = pid_pitch_Kp;

                                draw_setting_frame("Pitch - Kp", 0.0f, 10.0f, 0.001f, &temp_val, &pid_pitch_Kp_changed, 1);

                                pid_pitch_Kp = temp_val;

                                break;
                            }

                            case 2:                 // Yaw - Kp
                            {
                                float temp_val = pid_yaw_Kp;

                                draw_setting_frame("Yaw - Kp", 0.0f, 10.0f, 0.001f, &temp_val, &pid_yaw_Kp_changed, 1);

                                pid_yaw_Kp = temp_val;

                                break;
                            }

                            default:
                            {

                                break;
                            }
                        }

                        break;
                    }

                    case 1:
                    {
                        switch(oled_pref_struct.oled_index_V2)
                        {
                            case 0:                 // Roll - Ki
                            {
                                float temp_val = pid_roll_Ki;

                                draw_setting_frame("Roll - Ki", 0.0f, 10.0f, 0.001f, &temp_val, &pid_roll_Ki_changed, 1);

                                pid_roll_Ki = temp_val;

                                break;
                            }

                            case 1:                 // Pitch - Ki
                            {
                                float temp_val = pid_pitch_Ki;

                                draw_setting_frame("Pitch - Ki", 0.0f, 10.0f, 0.001f, &temp_val, &pid_pitch_Ki_changed, 1);

                                pid_pitch_Ki = temp_val;

                                break;
                            }

                            case 2:                 // Yaw - Ki
                            {
                                float temp_val = pid_yaw_Ki;

                                draw_setting_frame("Yaw - Ki", 0.0f, 10.0f, 0.001f, &temp_val, &pid_yaw_Ki_changed, 1);

                                pid_yaw_Ki = temp_val;

                                break;
                            }

                            default:
                            {

                                break;
                            }
                        }
                        
                        break;
                    }

                    case 2:
                    {
                        switch(oled_pref_struct.oled_index_V2)
                        {
                            case 0:                 // Roll - Kd
                            {
                                float temp_val = pid_roll_Kd;

                                draw_setting_frame("Roll - Kd", 0.0f, 10.0f, 0.001f, &temp_val, &pid_roll_Kd_changed, 1);

                                pid_roll_Kd = temp_val;

                                break;
                            }

                            case 1:                 // Pitch - Kd
                            {
                                float temp_val = pid_pitch_Kd;

                                draw_setting_frame("Pitch - Kd", 0.0f, 10.0f, 0.001f, &temp_val, &pid_pitch_Kd_changed, 1);

                                pid_pitch_Kd = temp_val;

                                break;
                            }

                            case 2:                 // Yaw - Kd
                            {
                                float temp_val = pid_yaw_Kd;

                                draw_setting_frame("Yaw - Kd", 0.0f, 10.0f, 0.001f, &temp_val, &pid_yaw_Kd_changed, 1);

                                pid_yaw_Kd = temp_val;

                                break;
                            }

                            default:
                            {

                                break;
                            }
                        }

                        break;
                    }


                    default:
                    {

                        break;
                    }
                }

                /*
                if(k_A == 1)                                                //  进入该页
                {
                    if(oled_pref_struct.oled_page_depth < 4)
                        oled_pref_struct.oled_page_depth += 1;
                    
                    vTaskDelay(180 / portTICK_RATE_MS);
                }
                */
            }

            default:
            {

            }
        }
        
        u8g2.sendBuffer();

        //vTaskDelay(40 / portTICK_RATE_MS);
        
        time_stamp++;
        pack_total++;
    }
}

void draw_setting_frame(String title, float min_value, float max_value, float step, float *current_value, uint8_t* channel_flag, uint8_t is_float)
{
    float current_val = *current_value;
    float value = floatMap(current_val, min_value, max_value, 0, 58);

    if(oled_pref_struct.key_left == 1)
    {
        *channel_flag = 1;

        if(current_val > min_value)
        {
            current_val -= step;

            if(current_val > min_value)
                *current_value -= step;
            else
                *current_value = min_value;
        }

        vTaskDelay(135 / portTICK_RATE_MS);
    }
    else if(oled_pref_struct.key_right == 1)
    {
        *channel_flag = 1;

        if(current_val < max_value)
        {
            current_val += step;

            if(current_val < max_value)
                *current_value += step;
            else
                *current_value = max_value;
        }

        vTaskDelay(135 / portTICK_RATE_MS);
    }

    if((1.0f - (float) nrf_send_raw.joyLHori / (float) 32768.0f) > 0.3f)
    {
        *channel_flag = 1;

        if(is_float)
        {
            if(current_val > min_value)
            {
                current_val -= ((max_value - min_value) / 200.0f);

                if(current_val > min_value)
                    *current_value -= ((max_value - min_value) / 200.0f);
                else 
                    *current_value = min_value;
            }
        }
        else
        {
            if(current_val > min_value)
            {
                current_val -= step * 2.0f;

                if(current_val > min_value)
                    *current_value -= step * 2.0f;
                else 
                    *current_value = min_value;
            }
        }
        //vTaskDelay(25 / portTICK_RATE_MS);
    }
    else if((1.0f - (float) nrf_send_raw.joyLHori / (float) 32768.0f) < -0.3f && current_val < max_value && (current_val + step) <= max_value)
    {
        *channel_flag = 1;

        if(is_float)
        {
            if(current_val < max_value)
            {
                current_val += ((max_value - min_value) / 200.0f);

                if(current_val < max_value)
                    *current_value += ((max_value - min_value) / 200.0f);
                else
                    *current_value = max_value;
            }
        }
        else
        {
            if(current_val < max_value)
            {
                current_val += step * 2.0f;

                if(current_val < max_value)
                    *current_value += step * 2.0f;
                else
                    *current_value = max_value;
            }
        }

        //vTaskDelay(25 / portTICK_RATE_MS);
    }



    u8g2.drawFrame(10, 9, 108, 48);

    u8g2.setDrawColor(0);

    for(uint8_t i = 11; i < 108 + 9; i++)
        for(uint8_t j = 10; j < 48 + 8; j++)
            u8g2.drawPixel(i, j);

    //u8g2.setDrawColor(0);
    for(uint8_t i = 0; i < 58; i++)
        u8g2.drawVLine(16 + i, 41, 3);

    u8g2.setDrawColor(1);
    for(uint8_t i = 0; i < value; i++)
        u8g2.drawVLine(16 + i, 41, 3);

    u8g2.setDrawColor(1);

    u8g2.setCursor(25, 29);
    OLED_Setfont_6x10();
    u8g2.print(title);

    u8g2.drawHLine(15, 40, 60);
    u8g2.drawHLine(15, 44, 60);

    u8g2.drawVLine(15, 41, 3);
    u8g2.drawVLine(74, 41, 3);
    
    u8g2.setCursor(80, 45);
    OLED_Setfont_6x10();
    u8g2.print(String(current_val, 3));

    u8g2.setDrawColor(1);
}

void draw_control_system_frame(void)
{
    float data_roll = (float) nrf_receive_raw.roll / 100.0f;
    float data_pitch = (float) nrf_receive_raw.pitch / 100.0f;
    float data_yaw = (float) nrf_receive_raw.yaw / 100.0f;
    float data_temp = (float) nrf_receive_raw.temp / 100.0f;

    u8g2.setDrawColor(1);
    OLED_Setfont_6x10();

    u8g2.home();
    
    // Row 1
    u8g2.setCursor(1, 18);
    u8g2.print("R " + String(data_roll, 2));

    u8g2.setCursor(1, 28);
    u8g2.print("P " + String(data_pitch, 2));

    u8g2.setCursor(1, 38);
    u8g2.print("Y " + String(data_yaw, 2));

    u8g2.setCursor(1, 48);
    u8g2.print("T " + String(data_temp, 2));

    u8g2.setCursor(1, 58);
    u8g2.print("H " + String(nrf_receive_raw.height, 3));

    // Row 2
    u8g2.setCursor(66, 18);
    u8g2.print("L1 " + String(nrf_receive_raw.acc_L1, 2));

    u8g2.setCursor(66, 28);
    u8g2.print("L2 " + String(nrf_receive_raw.acc_L2, 2));

    u8g2.setCursor(66, 38);
    u8g2.print("R1 " + String(nrf_receive_raw.acc_R1, 2));

    u8g2.setCursor(66, 48);
    u8g2.print("R2 " + String(nrf_receive_raw.acc_R2, 2));

    u8g2.setCursor(66, 58);
    u8g2.print("V  " + String(nrf_receive_raw.volt, 2));

    u8g2.home();
}

void draw_menu_title(void)
{
    u8g2.setDrawColor(1);

    u8g2.drawHLine(0, 7, 128);

    u8g2.setFont(u8g2_font_squeezed_r6_tr);

    switch(oled_pref_struct.oled_index_V)
    {
        case 0:
        {
            u8g2.drawStr(1, 6, "Control System");

            break;
        }

        case 1:
        {
            u8g2.drawStr(1, 6, "NRF Setting");

            break;
        }

        case 2:
        {
            u8g2.drawStr(1, 6, "PID Setting");

            break;
        }

        case 3:
        {
            u8g2.drawStr(1, 6, "Untitled");

            break;
        }

        default:
        {

        }
    }
}

void draw_menu_title_2nd(void)
{
    u8g2.setDrawColor(1);

    u8g2.drawHLine(0, 7, 128);

    u8g2.setFont(u8g2_font_squeezed_r6_tr);

    switch(oled_pref_struct.oled_index_V2)
    {
        case 0:
        {
            u8g2.drawStr(1, 6, "PID Setting - Roll");

            break;
        }

        case 1:
        {
            u8g2.drawStr(1, 6, "PID Setting - Pitch");

            break;
        }

        case 2:
        {
            u8g2.drawStr(1, 6, "PID Setting - Yaw");

            break;
        }

        case 3:
        {
            u8g2.drawStr(1, 6, "PID Setting - N/A");

            break;
        }

        default:
        {

        }
    }
}

void draw_menu_title_3rd(String str_title)
{
    u8g2.setDrawColor(0);

    for(uint8_t i = 0; i < 56; i++)
        for(uint8_t j = 0; j < 7; j++)
            u8g2.drawPixel(i, j);

    u8g2.setDrawColor(1);

    u8g2.drawHLine(0, 7, 128);

    u8g2.setFont(u8g2_font_squeezed_r6_tr);

    switch(oled_pref_struct.oled_index_V2)
    {
        case 0:
        {
            u8g2.drawStr(1, 6, String(str_title + " - Kp").c_str());

            break;
        }

        case 1:
        {
            u8g2.drawStr(1, 6, String(str_title + " - Ki").c_str());

            break;
        }

        case 2:
        {
            u8g2.drawStr(1, 6, String(str_title + " - Kd").c_str());

            break;
        }

        case 3:
        {
            u8g2.drawStr(1, 6, "N/A");

            break;
        }

        default:
        {

        }
    }
}

void draw_menu_text(void)
{
    u8g2.setDrawColor(1);
    OLED_Setfont_6x10();

    u8g2.drawStr(14, 19, "Control System");

    u8g2.drawStr(14, 31, "NRF Setting");

    u8g2.drawStr(14, 43, "PID Setting");

    u8g2.drawStr(14, 55, "Untitled");
}

void draw_menu_text_2nd(void)
{
    u8g2.setDrawColor(1);
    OLED_Setfont_6x10();

    u8g2.drawStr(14, 19, "Roll");

    u8g2.drawStr(14, 31, "Pitch");

    u8g2.drawStr(14, 43, "Yaw");

    u8g2.drawStr(14, 55, "N/A");
}

void draw_menu_text_3rd(String str_text)
{
    u8g2.setDrawColor(1);
    OLED_Setfont_6x10();

    u8g2.drawStr(14, 19, String(str_text + " Kp").c_str());

    u8g2.drawStr(14, 31, String(str_text + " Ki").c_str());

    u8g2.drawStr(14, 43, String(str_text + " Kd").c_str());

    u8g2.drawStr(14, 55, "N/A");
}

void draw_rolling(uint8_t start_index)  // 0 12 24 36
{
    u8g2.setDrawColor(1);

    u8g2.drawVLine(124, 7, 57);
    
    for(uint16_t i = 10; i < 64; i = i + 12)
        u8g2.drawHLine(122, i, 5);

    for(uint16_t i = 10; i < 60; i = i + 4)
        u8g2.drawHLine(123, i, 3);

    for(uint16_t i = start_index + 12; i > start_index; i--)
    {
        if(start_index < 37)
            u8g2.drawHLine(123, i + 10, 3);

        u8g2.drawFrame(5, start_index + 10, 105, 12);
        //u8g2.drawPixel(110, 22 + start_index);
    }

    u8g2.setDrawColor(1);
}

uint8_t map_index(float volts)
{
    float temp_volts = volts;

    if(temp_volts > Volts_MAX)
        temp_volts = Volts_MAX;
    
    if(temp_volts < Volts_MIN)
        temp_volts = Volts_MIN;
    
    temp_volts = floatMap(temp_volts, Volts_MIN, Volts_MAX, 0.0f, 5.0f);

    return (uint8_t ) temp_volts;
}

void draw_battery(uint8_t index, uint8_t toggle_on)
{
    u8g2.setDrawColor(toggle_on);

    u8g2.drawHLine(104, 1, 7);
    u8g2.drawHLine(104, 4, 7);
    u8g2.drawVLine(104, 2, 2);
    u8g2.drawVLine(110, 2, 2);
    u8g2.drawVLine(111, 2, 2);

    switch(index)
    {
        case 1:
        {
            u8g2.drawVLine(105, 2, 2);

            break;
        }

        case 2:
        {
            u8g2.drawVLine(105, 2, 2);
            u8g2.drawVLine(106, 2, 2);

            break;
        }

        case 3:
        {
            u8g2.drawVLine(105, 2, 2);
            u8g2.drawVLine(106, 2, 2);
            u8g2.drawVLine(107, 2, 2);

            break;
        }

        case 4:
        {
            u8g2.drawVLine(105, 2, 2);
            u8g2.drawVLine(106, 2, 2);
            u8g2.drawVLine(107, 2, 2);
            u8g2.drawVLine(108, 2, 2);

            break;
        }

        case 5:
        {
            u8g2.drawVLine(105, 2, 2);
            u8g2.drawVLine(106, 2, 2);
            u8g2.drawVLine(107, 2, 2);
            u8g2.drawVLine(108, 2, 2);
            u8g2.drawVLine(109, 2, 2);

            break;
        }
    }

    u8g2.setDrawColor(1);
}

void draw_ble(uint8_t toggle_on)
{
    u8g2.setDrawColor(toggle_on);
    
    u8g2.drawVLine(115, 0, 5);
    u8g2.drawPixel(116, 0);
    u8g2.drawPixel(117, 0);

    u8g2.drawPixel(116, 2);
    u8g2.drawPixel(117, 2);

    u8g2.drawPixel(116, 4);
    u8g2.drawPixel(117, 4);

    u8g2.drawPixel(118, 1);
    u8g2.drawPixel(118, 3);

    u8g2.setDrawColor(1);
}

void draw_wifi(uint8_t toggle_on)
{
    u8g2.setDrawColor(toggle_on);

    u8g2.drawVLine(127, 0, 5);
    u8g2.drawVLine(125, 2, 3);
    u8g2.drawVLine(123, 3, 2);
    u8g2.drawVLine(121, 4, 1);

    u8g2.setDrawColor(1);
}

void draw_loss(float loss)
{
    uint8_t loss_in_percent = (uint8_t) (loss * 100.0f);
    u8g2.setDrawColor(1);

    u8g2.setFont(u8g2_font_squeezed_r6_tr);

    u8g2.setCursor(66, 6);
    u8g2.print("loss: " + String(loss_in_percent) + "%");

    u8g2.setDrawColor(1);
}

void draw_depth(void)
{
    u8g2.setDrawColor(1);

    u8g2.setFont(u8g2_font_squeezed_r6_tr);

    u8g2.setCursor(58, 6);
    u8g2.print(String(oled_pref_struct.oled_page_depth));

    u8g2.setDrawColor(1);
}

float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
{
    if(x > in_max)
        x = in_max;
    
    if(x < in_min)
        x = in_min;

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



