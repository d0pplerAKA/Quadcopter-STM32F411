#include "xbox.h"

uint8_t data_exchange_lock_receive = 0;
uint8_t data_exchange_lock_send_ack = 0;


uint8_t ws2812_mode = 0x00;
uint8_t drone_hover_switch = 0x00;


volatile uint8_t xbox_abort_flag = 0;
volatile uint8_t xbox_recovery_flag = 0;
volatile uint8_t xbox_upload_parameter_flag = 0;

NRF_RECEIVE_RAW nrf_receive_raw;
NRF_SEND_RAW nrf_send_raw;
NRF_MANUAL_RAW nrf_manual_raw;

JOY_STICK_t nrf_joy_stick;


const uint8_t nrf_default_signal[32] = {
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,

    0,0x80,
    0,0x80,
    0,0x80,
    0,0x80,

    0,0,
    0,0,

    0,0,0,0
};



void XBOX_JoyStick_Convert(JOY_STICK_t * joy)
{
    joy->JoyLeftV = 1.0f - (float) ((float) nrf_send_raw.joyLVert / (float) 32768.0f);
    joy->JoyRightH = 1.0f - (float) ((float) nrf_send_raw.joyRHori / (float) 32768.0f);
    joy->JoyRightV = 1.0f - (float) ((float) nrf_send_raw.joyRVert / (float) 32768.0f);

    joy->TrigLeft = (float) ((float) nrf_send_raw.trigLT / (float) 1024.0f);
    joy->TrigRight = (float) ((float) nrf_send_raw.trigRT / (float) 1024.0f);
}


void XBOX_IRQHandler(void)
{
    if(package_is_control_message && nrf_send_raw.is_control_message == 0xCE)
    {
        /*********************************************/
        if(nrf_send_raw.btnA && xbox_sampling_cmp())
        {
            
        }

        if(nrf_send_raw.btnB && xbox_sampling_cmp())
        {
            xbox_abort_flag++;
        }

        if(nrf_send_raw.btnY && xbox_sampling_cmp())
        {

            XBOX_WS2812_Controll(ws2812_mode);

            ws2812_mode++;

            if(ws2812_mode == I2C_Esp32Soc_WS2812_Mode7 + 1) ws2812_mode = 0;
            
            /*            
            if(drone_hover_switch == 0)
            {
                I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, ws2812_mode);

                ws2812_mode++;

                if(ws2812_mode == I2C_Esp32Soc_WS2812_Mode4)
                    ws2812_mode++;
                
                if(ws2812_mode == (I2C_Esp32Soc_WS2812_Mode5 + 1))
                    ws2812_mode = I2C_Esp32Soc_WS2812_Mode0;
            }
            else
            {
                I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode4);
            }
            */
        }

        if(nrf_send_raw.btnX && xbox_sampling_cmp())
        {
            /*
                Hover Mode Switch

                Use ws2812 led breath mode to indicate if the drone is in a hovering mode.

                Breathing means yes. [Auto Hovering]
                No breathing means no. [Manual]
            */
            /*
            drone_hover_switch = !drone_hover_switch;

            if(drone_hover_switch)
                I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode4);
            else
                I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode5);
            */
            //printf("Drone Hover state: %d\n", drone_hover_switch);
        }
        /*********************************************/

        /*********************************************/
        if(nrf_send_raw.btnDirLeft && xbox_sampling_cmp())
        {

        }

        if(nrf_send_raw.btnDirUp && xbox_sampling_cmp())
        {

        }

        if(nrf_send_raw.btnDirRight && xbox_sampling_cmp())
        {

        }

        if(nrf_send_raw.btnDirDown && xbox_sampling_cmp())
        {

        }
        /*********************************************/

        if(nrf_send_raw.btnShare && xbox_sampling_cmp())
        {
            if(xbox_abort_flag > 0)
                xbox_upload_parameter_flag++;
        }

        if(nrf_send_raw.btnSelect && xbox_sampling_cmp())
        {
            if(xbox_abort_flag > 0)
                xbox_recovery_flag++;
        }

        /*********************************************/
        
        XBOX_JoyStick_Convert(&nrf_joy_stick);

        /*********************************************/

        Drone_Change_TargetAttitude(&drone_attitude);


        /*
        printf("Joy left V: %.3f right H: %.3f right V: %.3f Trig L: %.3f Trig R: %.3f\n", 
            nrf_joy_stick.JoyLeftV,
                nrf_joy_stick.JoyRightH,
                    nrf_joy_stick.JoyRightV,
                        nrf_joy_stick.TrigLeft,
                            nrf_joy_stick.TrigRight);
        */
    }

    xbox_sampling_counter++;
}

void XBOX_WS2812_Controll(uint8_t ws2812_mode)
{
    I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, (uint8_t) (I2C_Esp32Soc_WS2812_Mode0 | ws2812_mode));
}

