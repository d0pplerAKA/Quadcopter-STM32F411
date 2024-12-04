#include "Arduino.h"
#include "xbox.h"
#include "oled.h"
#include "nrf24.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/FreeRTOSConfig.h"

SemaphoreHandle_t xNRF_IRQ;

void setup(void)
{
    Serial.begin(115200);

    //vTaskDelay(500 / portTICK_RATE_MS);                      // For hardware setup, such as toggle the switch. 
    oled_setup();    
    xbox_setup();
    //xTaskCreate(, "xbox setup", 4096, NULL, 2, NULL);
    //xTaskCreate(, "oled setup", 4096, NULL, 2, NULL);

    
    if(xbox_boot_mode == XBOX_ModeSetting) 
    {
        xTaskCreate(oled_boot_operation, "oled op boot", 8192, NULL, 5, NULL);

        xbox_boot();
    }
    
    xTaskCreate(oled_operation, "oled op", 8192, NULL, 10, NULL);

    pinMode(XBOX_ModeSelectPin, INPUT_PULLUP);                // NRF IRQ Same
    xNRF_IRQ = xSemaphoreCreateBinary();
    NRF_IRQ_Install();

    xTaskCreate(nrf_send_operation, "nrf op send", 8192, NULL, 7, &nrf24_send_op_handle);
    xTaskCreate(nrf_recv_operation, "nrf op recv", 8192, NULL, 8, &nrf24_recv_op_handle);


    //xTaskCreate(xbox_operation, "xbox_op", 8192, NULL, 4, NULL);

    Serial.end();

    //vTaskStartScheduler();
}

void loop(void)
{
    xbox_operation();
}


