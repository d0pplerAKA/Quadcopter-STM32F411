#include "xbox.h"

//XBOX_DATAPACK xbox_datapack;

char MacAddress_const[7] = "mAddr ";
Preferences MAC_AddressE2PROM[XBOX_Max_Maclist];

uint8_t xbox_boot_mode;


String MAC_Address_Selected = "";
uint8_t MAC_Address_withStar_index;
uint8_t XBOX_isConnect = XBOX_UNCONNECT;

uint8_t data_exchange_lock = 0;


void xbox_setup(void)
{
    for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
    {
        MacAddress_const[6] = i + '0';
        MAC_AddressE2PROM[i].begin(MacAddress_const);
    }

    MAC_Address_withStar_index = xbox_check_mac_list_status();
    if(MAC_Address_withStar_index == 127)
    {
        xbox_boot_mode = XBOX_ModeSetting;
        return;
    }

    pinMode(XBOX_ModeSelectPin, INPUT);                // NRF IRQ Same

    if(digitalRead(XBOX_ModeSelectPin) == XBOX_ModeSetting)
    {
        vTaskDelay(8 / portTICK_RATE_MS);

        if(digitalRead(XBOX_ModeSelectPin) == XBOX_ModeSetting)     xbox_boot_mode = XBOX_ModeSetting;
        else                                                        xbox_boot_mode = XBOX_ModeNormal;
    } 
    else                                                            xbox_boot_mode = XBOX_ModeNormal;
    
    //xbox_boot_mode = XBOX_DEBUG;
}


void xbox_boot(void)
{
    if(xbox_boot_mode == XBOX_ModeSetting)  xbox_setting();
    else
    {
        for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
        {
            MAC_AddressE2PROM[i].end();

            vTaskDelay(XBOX_Serial_Timeout / portTICK_RATE_MS);
        }

        //Serial.end();
    }
}


void xbox_setting(void)
{
    while(1)
    {
        while(Serial.read() >= 0);

        MAC_Address_withStar_index = xbox_check_mac_list_status();

        String command_string;
        char command_key;

        xbox_setting_showlist(xbox_get_mac_total());

        xbox_get_SerialCommand(&command_string, &command_key);

        String p1;
        String p2;
        String p3;
        
        xbox_get_All_parameters(command_string, command_key, &p1, &p2, &p3);

        switch(command_key)
        {
            case 'A':
            {
                /*
                * @param1: index (convert to int)
                * @param2: boot sign
                * @param3: Mac Address
                */

                int8_t index_p = p1.toInt();
                String boot_sign = p2;

                if((index_p >= 0) && (index_p <= (XBOX_Max_Maclist - 1)))
                {
                    String t_str = MAC_AddressE2PROM[index_p].getString("mac", "##################");
                    char t_c = t_str.charAt(0);


                    if(t_c != '*' && t_c != '>' && t_c != '#')
                    {
                        Serial.printf("> The index that you have typed in(%d) is unvalid. Config Rom Error!\n", index_p+1);
                        Serial.printf("> Process denied!\n");
                        Serial.printf("> Insert -help for more information.\n");

                        break;
                    }
                    else
                    {
                        if(t_c == '#')
                        {
                            if(boot_sign == "#")
                            {
                                if(xbox_get_mac_valid(p3))
                                {
                                    MAC_AddressE2PROM[index_p].putString("mac", "*" + p3);

                                    Serial.printf("> Process finished.\n");
                                    Serial.printf("> You have inserted the Mac Address.\n");

                                break;
                                }
                                else
                                {
                                    Serial.printf("> The Mac address that you have typed in(");

                                    for(uint8_t i = 0; i < p3.length(); i++)
                                        Serial.print(p3.charAt(i));

                                    Serial.printf(") is unvalid.\n");

                                    Serial.printf("> Process denied!\n");
                                    Serial.printf("> Insert -help for more information.\n");

                                    break;
                                }
                            }
                            else
                            {
                                if(xbox_get_mac_valid(p3))
                                {
                                    for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
                                    {
                                        String tt_str = MAC_AddressE2PROM[i].getString("mac", "##################");

                                        if(tt_str.charAt(0) == '>')
                                        {
                                            tt_str.replace(">", "*");
                                            MAC_AddressE2PROM[i].putString("mac", tt_str);
                                            break;
                                        }
                                    }

                                    MAC_AddressE2PROM[index_p].putString("mac", ">" + p3);

                                    Serial.printf("> Process finished.\n");
                                    Serial.printf("> You have added the Mac Address, and set it to a BOOT Mac address automatically.\n");

                                    break;
                                }
                                else
                                {
                                    Serial.printf("> The Mac address that you have typed in(");

                                    for(uint8_t i = 0; i < p3.length(); i++)
                                        Serial.print(p3.charAt(i));

                                    Serial.printf(") is unvalid.\n");

                                    Serial.printf("> Process denied!\n");
                                    Serial.printf("> Insert -help for more information.\n");

                                    break;
                                }
                            }
                        }
                        else if(t_c == '>')
                        {
                            Serial.printf("> You are trying to wipe a BOOT MAc address.\n");
                            Serial.printf("> Process denied!\n");
                            Serial.printf("> Insert -help for more information.\n");

                            break;
                        }
                        else
                        {
                            Serial.printf("> The index that you have typed in(%d) is unvalid. There is already a Mac address saved\n", index_p+1);
                            Serial.printf("> Process denied!\n");
                            Serial.printf("> Insert -help for more information.\n");

                            break;
                        }
                    }
                }
                else
                {
                    Serial.printf("> The index that you have typed in(%d) is unvalid. It's out of range!\n", index_p+1);
                    Serial.printf("> Process denied!\n");
                    Serial.printf("> Insert -help for more information.\n");

                    break;
                }

                break;
            }
            case 'C':
            {
                /*
                * @param1: index to be replaced (get rid of ">")
                * @param2: index to replace (add ">")
                * @param3: ""
                */

                uint8_t index_to_be_replaced = p1.toInt();
                uint8_t index_to_replace = p2.toInt();

                String t_str = MAC_AddressE2PROM[index_to_be_replaced].getString("mac", "##################");

                char t_c = t_str.charAt(0);

                if(t_c == '*')
                {
                    Serial.printf("> The index you have typed in isn't point to any Mac Address.\n");
                    Serial.printf("> Process denied!\n");
                    Serial.printf("> Insert -help for more information.\n");

                    break;
                }
                else if(t_c == '>')
                {
                    String tt_str = MAC_AddressE2PROM[index_to_replace].getString("mac", "##################");
                    char tt_c = tt_str.charAt(0);

                    if(tt_c != '*')
                    {
                        Serial.printf("> Error happend. Config Rom error. \n");
                        Serial.printf("> Process denied!\n");
                        Serial.printf("> Insert -help for more information.\n");

                        break;
                    }
                    else
                    {
                        String be_replaced = MAC_AddressE2PROM[index_to_be_replaced].getString("mac", "##################");
                        be_replaced.replace(">", "*");

                        String to_replace = MAC_AddressE2PROM[index_to_replace].getString("mac", "##################");
                        to_replace.replace("*", ">");

                        MAC_AddressE2PROM[index_to_be_replaced].putString("mac", be_replaced);
                        MAC_AddressE2PROM[index_to_replace].putString("mac", to_replace);

                        Serial.printf("> Process finished.\n");
                        Serial.printf("> You have changed No.%d as a BOOT Mac Address.\n", index_to_replace+1);

                        break;
                    }
                }
                else if(t_c == '#')
                {
                    Serial.printf("> The index that you have typed in(%d) is unvalid. There is no Mac Address!\n", index_to_replace+1);
                    Serial.printf("> Process denied!\n");
                    Serial.printf("> Insert -help for more information.\n");
                }
                else
                {
                    Serial.printf("> The index that you have typed in(%d) is unvalid. Config Rom Error!\n", index_to_replace+1);
                    Serial.printf("> Process denied!\n");
                    Serial.printf("> Insert -help for more information.\n");
                }

                break;
            }
            case 'D':
            {
                /*
                * @param1: index to delete (be wiped)
                * @param2: ""
                * @param3: ""
                */

                int8_t index_p = p1.toInt();

                if(index_p == -1)
                {
                    Serial.printf("> The index that you want to delete(%d) is the BOOT Mac Address. Can't do.\n", MAC_Address_withStar_index);
                    Serial.printf("> Process denied!\n");
                    Serial.printf("> Insert -help for more information.\n");

                    break;
                }
                else
                {
                    if((index_p < 0 || index_p > XBOX_Max_Maclist - 1))
                    {
                        Serial.printf("> The index that you have typed in(%d) is unvalid. It's out of range!\n", index_p+1);
                        Serial.printf("> Process denied!\n");
                        Serial.printf("> Insert -help for more information.\n");

                        break;
                    }
                    else
                    {
                        String t_str = MAC_AddressE2PROM[index_p].getString("mac", "##################");
                        char t_c = t_str.charAt(0);

                        if(t_c == '#')
                        {
                            Serial.printf("> The index that you have typed in(%d) is unvalid. There is no Mac Address!\n", index_p+1);
                            Serial.printf("> Process denied!\n");
                            Serial.printf("> Insert -help for more information.\n");

                            break;
                        }
                        else
                        {
                            MAC_AddressE2PROM[index_p].putString("mac", "##################");

                            Serial.printf("> Process finished.\n");
                            Serial.printf("> You have deleted the target Mac Address.\n");

                            break;
                        }
                    }
                }

                break;
            }
            case 'W':
            {
                /*
                * @param1: Mac Address
                * @param2: ""
                * @param3: ""
                */
                String temp_mac_address = p1;

                if(xbox_get_mac_valid(temp_mac_address))
                {
                    for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
                    MAC_AddressE2PROM[i].putString("mac", "##################");

                    MAC_AddressE2PROM[0].putString("mac", ">" + temp_mac_address);

                    Serial.printf("> Process finished.\n");
                    Serial.printf("> You have wiped the whole Config Rom.\n");
                }
                else
                {
                    Serial.printf("> The Mac address that you have typed in(");

                    for(uint8_t i = 0; i < p3.length(); i++)
                        Serial.print(temp_mac_address.charAt(i));

                    Serial.printf(") is unvalid.\n");

                    Serial.printf("> Process denied!\n");
                    Serial.printf("> Insert -help for more information.\n");

                    break;
                }

                break;
            }
            case 'P':
            {

                break;
            }
            case 'p':
            {

                break;
            }
            case 127:         // -error
            {
                Serial.printf("> Wrong Insert!\n");
                Serial.printf("> Error! What you have insert is not a command!\n");
                Serial.printf("> Insert -help for full commands.\n");

                break;
            }
            case 126:         // -help
            {
                xbox_setting_ref();

                break;
            }
            case 125:         // -restart
            {
                Serial.printf("> Restarting process start now. \n");
                
                vTaskDelay(50 / portTICK_RATE_MS);

                for(uint8_t i = 5; i > 0; i--)
                {
                    Serial.printf("> Restart in %d...\n", i);
                    vTaskDelay(999 / portTICK_RATE_MS);
                }

                vTaskDelay(XBOX_Serial_Timeout / portTICK_RATE_MS);
                Serial.printf("> Restarting rignt now. Thank you for using!\n\n");
                
                for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
                {
                    MAC_AddressE2PROM[i].end();
                    vTaskDelay(5 / portTICK_RATE_MS);
                }

                vTaskDelay(799 / portTICK_RATE_MS);
                Serial.end();

                ESP.restart();

                break;  // Warning prevent
            }
            default:
            {
                Serial.printf("> Error! What you have insert is not a command!\n");
                Serial.printf("> Insert -help for full commands.\n");
                
                break;
            }
        }
    }
}


void xbox_setting_showlist(uint8_t mac_num)
{
    Serial.printf("\n");
    Serial.printf(">-----------------------------------------------------------------------------------------------\n");
    Serial.printf("> - Bluetooth 5.0 Mac Config -\n");
    Serial.printf("> There are totally %d Mac address(s) found in Config Rom slot.\n", mac_num);
    Serial.printf("> The one with an arrow(>) symbol means this is BOOT Mac address. Currently choose: [");

    for(uint8_t i = 0; i < MAC_Address_Selected.length(); i++)
        Serial.printf("%c", MAC_Address_Selected.charAt(i));

    Serial.printf("]\n");
    Serial.printf("> Mac Address List: \n");
    Serial.printf("> index      Mac Address\n");
    
    for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
    {
        uint8_t k = i + 1;
        Serial.printf("> %d         ", k);
        
        if(i <= 8)
            Serial.printf(" ");

        String t_str = MAC_AddressE2PROM[i].getString("mac", "##################");

        for(uint8_t j = 0; j < XBOX_Max_Macsize; j++)
            Serial.printf("%c", t_str.charAt(j));

        Serial.printf("\n");
    }

    Serial.printf("> Insert -help for full commands.\n");
    Serial.printf(">-----------------------------------------------------------------------------------------------\n");
    Serial.printf("> Waiting commands...\n");
    Serial.printf("> \n");
}

void xbox_setting_ref(void)
{
    Serial.printf("\n");
    Serial.printf(">-----------------------------------------------------------------------------------------------\n");
    Serial.printf("> Help information.\n");
    Serial.printf("> -A [index number] [>][mac]     // Add a new BLE mac address.\n");
    Serial.printf(">                                // [index number] -> index number that is goint to assign Mac\n");
    Serial.printf(">                                // [>] -> add [>] to switch as BOOT directly or keep it empty\n");
    Serial.printf(">                                // [mac] -> new mac address\n");
    Serial.printf(">                                e.g. -A 15 >68:6c:e6:42:57:89\n");
    Serial.printf(">                                e.g. -A 15 68:6c:e6:42:57:89\n");
    Serial.printf("> \n");
    Serial.printf("> -C [index number]              // Change the selected BLE mac address to BOOT mac address.\n");
    Serial.printf(">                                // [index number] -> index number from BLE list\n");
    Serial.printf(">                                e.g. -C 8\n");
    Serial.printf("> \n");
    Serial.printf("> -D [index number]              // Delete the BLE mac address.\n");
    Serial.printf(">                                // [index number] -> index number from BLE list\n");
    Serial.printf(">                                e.g. -D 2\n");
    Serial.printf("> \n");
    Serial.printf("> -W [mac]                       // Wipe the whole rom and set a new boot Mac Address.\n");
    Serial.printf(">                                // [mac] -> new mac address\n");
    Serial.printf(">                                e.g. -W 68:6c:e6:42:57:89\n");
    Serial.printf("> \n");
    Serial.printf("> -P                             // Print the BLE list again\n");
    Serial.printf(">                                e.g. -P\n");
    Serial.printf("> \n");
    Serial.printf("> -help                          // Print the command list.\n");
    Serial.printf("> -restart                       // Software Reset.\n");
    Serial.printf(">-----------------------------------------------------------------------------------------------\n");
}


void xbox_get_SerialCommand(String *cmd_str, char *cmd_key)
{
    *cmd_str = "";
    String temp_str = "";
    String t_str = "";

    uint16_t timeout_s;

    while(Serial.available() == 0)      // 0 input
    {
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    timeout_s = XBOX_Serial_Timeout;
    while(Serial.available() < 2)       // order input
    {
        timeout_s--;
        vTaskDelay(5 / portTICK_RATE_MS);

        if(timeout_s == 0)
            return;
    }

    vTaskDelay(XBOX_Serial_Timeout / portTICK_RATE_MS);

    temp_str = Serial.readString();
    temp_str.trim();

    Serial.printf("> Got Command: ");

    for(uint32_t i = 0; i < temp_str.length(); i++)
        Serial.printf("%c", temp_str.charAt(i));

    Serial.printf("\n");
    
    for(uint32_t i = 0; i < temp_str.length(); i++)
    {
        char temp_c = temp_str.charAt(i);

        if(((temp_c >= '0' && temp_c <= '9') || (temp_c >= 'a' && temp_c <= 'z') || (temp_c >= 'A' && temp_c <= 'Z')) || 
            (temp_c == ':' || temp_c == ' ' || temp_c == '-' || temp_c == '>' || temp_c == '\n' || temp_c == '\r'))
        {
            continue;
        }
        else
        {
            *cmd_key = 127;
            return;
        }
    }

    t_str = temp_str;
    if(t_str.substring(0, 5) == "-help")
    {
        *cmd_key = 126;
        return;
    }
    
    t_str = temp_str;
    if(t_str.substring(0, 8) == "-restart")
    {
        *cmd_key = 125;
        return;
    }

    *cmd_str = temp_str;
    *cmd_key = temp_str.charAt(1);
}


uint8_t xbox_check_mac_list_status(void)
{  
    for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
    {
        String t_str = MAC_AddressE2PROM[i].getString("mac", "##################");

        if(t_str.charAt(0) == '>')
        {
            MAC_Address_Selected = t_str.substring(1, t_str.length());
                
            return i;
        }
    }

    return 127;
}


void xbox_get_All_parameters(String cmd_string, char cmd_key, String *param1, String *param2, String *param3)
{
    *param1 = "";
    *param2 = "";
    *param3 = "";

    
    String all_parameters_str = cmd_string.substring(3, cmd_string.length());
    uint8_t cmd_str_len = all_parameters_str.length();


    if(cmd_key == 127 || cmd_key == 126 || cmd_key == 125)  return;
    else
    {
        if(cmd_key == 'A')
        {
            /*
            * @param1: index (convert to int)
            * @param2: boot sign
            * @param3: Mac Address
            */

            String index_toAdd = "";
            String temp_mac_address = "";

            for(uint8_t i = 0; i < cmd_str_len; i++)
            {
                char t_c = all_parameters_str.charAt(i);

                if(t_c == ' ')
                    break;
                
                index_toAdd += t_c;
            }

            temp_mac_address = all_parameters_str.substring(all_parameters_str.length() - 18, all_parameters_str.length());
            
            int8_t temp_index = index_toAdd.toInt();
            temp_index--;
            index_toAdd = String(temp_index);

            if(temp_mac_address.charAt(0) == '>')
            {
                *param1 = index_toAdd;
                *param2 = ">";
                *param3 = temp_mac_address.substring(1, temp_mac_address.length());
            }
            else
            {
                *param1 = index_toAdd;
                *param2 = "#";
                *param3 = temp_mac_address.substring(1, temp_mac_address.length());
            }
        }
        else if(cmd_key == 'C')
        {
            /*
            * @param1: index to be replaced (convert to int, get rid of ">")
            * @param2: index to replace (convert to int, add ">")
            * @param3: ""
            */

            String index_to_be_replaced = "";
            String index_to_replace = "";

            bool t_b = true;

            for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
            {
                String t_str = MAC_AddressE2PROM[i].getString("mac", "##################");

                if(t_str.charAt(0) == '>')
                {
                    t_b = false;
                    index_to_be_replaced = String(i);
                    break;
                }
            }

            if(t_b)
            {
                Serial.printf("> Error happend. Config Rom error. \n");
                return;
            }


            for(uint8_t i = 0; i < cmd_str_len; i++)
            {
                char t_c = all_parameters_str.charAt(i);
                
                index_to_replace += t_c;
            }

            int8_t temp_index = index_to_replace.toInt();
            temp_index--;
            index_to_replace = String(temp_index);

            *param1 = index_to_be_replaced;
            *param2 = index_to_replace;
            *param3 = "";
        }
        else if(cmd_key == 'D')
        {
            /*
            * @param1: index to delete (be wiped)
            * @param2: ""
            * @param3: ""
            */

            String index_with_arrow;
            String index_to_delete;
        
            for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
            {
                String t_str = MAC_AddressE2PROM[i].getString("mac", "##################");

                if(t_str.charAt(0) == '>')
                {
                    index_with_arrow = String(i);
                    break;
                }
            }

            for(uint8_t i = 0; i < cmd_str_len; i++)
            {
                char t_c = all_parameters_str.charAt(i);
                
                index_to_delete += t_c;
            }

            int8_t temp_index = index_to_delete.toInt();
            temp_index--;
            index_to_delete = String(temp_index);

            if(index_with_arrow == index_to_delete) *param1 = "-1";
            else  *param1 = index_to_delete;
        }
        else if(cmd_key == 'W')
        {
            /*
            * @param1: Mac Address
            * @param2: ""
            * @param3: ""
            */

            String temp_mac_address = all_parameters_str.substring(all_parameters_str.length() - 17, all_parameters_str.length());

            *param1 = temp_mac_address;
        }
        else return;
    }
}

bool xbox_get_mac_valid(String mac)
{
    for(uint8_t i = 1; i < 18; i++)
    {
        char t_c = mac.charAt(i - 1);

        if(i % 3 == 0)
        {
            if(t_c != ':')
                return false;
        }
    }

    return true;
}


uint8_t xbox_get_mac_total(void)
{
    uint8_t num = 0;

    for(uint8_t i = 0; i < XBOX_Max_Maclist; i++)
    {
        String t_str = MAC_AddressE2PROM[i].getString("mac", "##################");
        char t_c = t_str.charAt(0);
        
        if(t_c == '*' || t_c == '>')
            num++;
    }

    return num;
}


void xbox_operation(void)
{  
    XboxSeriesXControllerESP32_asukiaaa::Core xboxController(MAC_Address_Selected);
    XboxSeriesXHIDReportBuilder_asukiaaa::ReportBase repo;

    TickType_t xLastWake = xTaskGetTickCount();

    xboxController.begin();
  
    while(1)
    {
        vTaskDelayUntil(&xLastWake, NRF_Speed);

        xboxController.onLoop();

        if(xboxController.isConnected()) 
        {
            XBOX_isConnect = XBOX_CONNECTED;
            
            if(!xboxController.isWaitingForFirstNotification()) 
            {
                //vTaskDelay(5 / portTICK_RATE_MS);
                data_exchange_lock = 1;

                oled_pref_struct.key_A = (uint8_t) xboxController.xboxNotif.btnA;
                oled_pref_struct.key_B = (uint8_t) xboxController.xboxNotif.btnB;

                oled_pref_struct.key_LB = (uint8_t) xboxController.xboxNotif.btnLB;
                oled_pref_struct.key_RB = (uint8_t) xboxController.xboxNotif.btnRB;

                oled_pref_struct.key_up = (uint8_t) xboxController.xboxNotif.btnDirUp;
                oled_pref_struct.key_left = (uint8_t) xboxController.xboxNotif.btnDirLeft;
                oled_pref_struct.key_right = (uint8_t) xboxController.xboxNotif.btnDirRight;
                oled_pref_struct.key_down = (uint8_t) xboxController.xboxNotif.btnDirDown;

                
                nrf_send_raw.btnA = (uint8_t) xboxController.xboxNotif.btnA;
                nrf_send_raw.btnB = (uint8_t) xboxController.xboxNotif.btnB;
                nrf_send_raw.btnX = (uint8_t) xboxController.xboxNotif.btnX;
                nrf_send_raw.btnY = (uint8_t) xboxController.xboxNotif.btnY;
                
                
                nrf_send_raw.btnDirUp = (uint8_t) xboxController.xboxNotif.btnDirUp;
                nrf_send_raw.btnDirLeft = (uint8_t) xboxController.xboxNotif.btnDirLeft;
                nrf_send_raw.btnDirRight = (uint8_t) xboxController.xboxNotif.btnDirRight;
                nrf_send_raw.btnDirDown = (uint8_t) xboxController.xboxNotif.btnDirDown;
                
                
                nrf_send_raw.btnShare = (uint8_t) xboxController.xboxNotif.btnShare;
                nrf_send_raw.btnStart = (uint8_t) xboxController.xboxNotif.btnStart;
                nrf_send_raw.btnSelect = (uint8_t) xboxController.xboxNotif.btnSelect;
                nrf_send_raw.btnXbox = (uint8_t) xboxController.xboxNotif.btnXbox;
                

                nrf_send_raw.btnLB = (uint8_t) xboxController.xboxNotif.btnLB;
                nrf_send_raw.btnRB = (uint8_t) xboxController.xboxNotif.btnRB;
                nrf_send_raw.btnLS = (uint8_t) xboxController.xboxNotif.btnLS;
                nrf_send_raw.btnRS = (uint8_t) xboxController.xboxNotif.btnRS;

                
                nrf_send_raw.joyLHori = xboxController.xboxNotif.joyLHori;
                nrf_send_raw.joyLVert = xboxController.xboxNotif.joyLVert;
                nrf_send_raw.joyRHori = xboxController.xboxNotif.joyRHori;
                nrf_send_raw.joyRVert = xboxController.xboxNotif.joyRVert;

                nrf_send_raw.trigLT = xboxController.xboxNotif.trigLT;
                nrf_send_raw.trigRT = xboxController.xboxNotif.trigRT;

                if(oled_pref_struct.is_control_message)
                {   
                    nrf_send_raw.is_control_message = 0xCE;
                    memcpy(nrf_temp_tx_ram, &nrf_send_raw, 32);
                }
                else memset(nrf_temp_tx_ram, 0, sizeof(uint8_t) * 32);

                data_exchange_lock = 0;

            }
        } 
        else 
        {
            XBOX_isConnect = XBOX_UNCONNECT;

            if (xboxController.getCountFailedConnection() > 3)
                ESP.restart();
        }
    }
}




