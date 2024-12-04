#include "nrf24.h"


Preferences NRF24_Channel_Pref;

Preferences NRF24_PID_Kp[3];
Preferences NRF24_PID_Ki[3];
Preferences NRF24_PID_Kd[3];

NRF_RECEIVE_RAW nrf_receive_raw;
NRF_SEND_RAW nrf_send_raw;
NRF_MANUAL_RAW nrf_manual_raw;

uint8_t nrf_status = 0;


TaskHandle_t nrf24_send_op_handle;
TaskHandle_t nrf24_recv_op_handle;


void NRF_SPI_setup(void)
{
    pinMode(NRF_NSS, OUTPUT);  // NRF CS Init
    NRF_CS_HIGH();             // No working

    pinMode(NRF_CE, OUTPUT);  // NRF CE Init
    NRF_CE_LOW();             // Low power mode

    pinMode(NRF_IRQ, INPUT_PULLUP);  // NRF IRQ Init

    //NRF_IRQ_Install()

    SPI.begin(NRF_SCLK, NRF_MISO, NRF_MOSI, NRF_NSS);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setFrequency(SPI_Speed);
}

void NRF_Writebyte(uint8_t command, uint8_t reg_data)
{
    NRF_CS_LOW();

    SPI.transfer(command);
    SPI.transfer(reg_data);
    
    NRF_CS_HIGH();
}

void NRF_Writebytes(uint8_t command, uint8_t *reg_data, uint8_t data_len)
{
    NRF_CS_LOW();

    SPI.transfer(command);

    for (uint8_t i = 0; i < data_len; i++)
    {
        SPI.transfer(*reg_data);
        reg_data++;
    }

    NRF_CS_HIGH();
}

void NRF_Readbyte(uint8_t command, uint8_t *data_p, uint8_t dummy_byte)
{
    NRF_CS_LOW();

    SPI.transfer(command);
    *data_p = SPI.transfer(dummy_byte);

    NRF_CS_HIGH();
}

void NRF_Readbytes(uint8_t command, uint8_t *data_p, uint8_t data_len, uint8_t dummy_byte)
{
    NRF_CS_LOW();

    SPI.transfer(command);

    for (uint8_t i = 0; i < data_len; i++) 
    {
        *data_p = SPI.transfer(dummy_byte);
        data_p++;
    }

    NRF_CS_HIGH();
}

void nrf_send_operation(void *pt)
{
    vTaskDelay(200 / portTICK_RATE_MS);

    NRF_SPI_setup();

    NRF24_PowerDown();

    NRF24_Init(NRF_PTX);

    NRF24_Channel_Pref.begin("nrf channel");
    nrf24l01p_channel = (uint8_t ) NRF24_Channel_Pref.getUChar("nrf chn", NRF_Channel);
    NRF24_Set_RF_Channel(nrf24l01p_channel);
    NRF24_Channel_Pref.end();

    //putFloat
    //getFloat

    NRF24_PID_Kp[NRF_PID_Roll].begin("pid roll kp");
    NRF24_PID_Ki[NRF_PID_Roll].begin("pid roll ki");
    NRF24_PID_Kd[NRF_PID_Roll].begin("pid roll kd");

    pid_roll_Kp = NRF24_PID_Kp[NRF_PID_Roll].getFloat("roll kp", 0.0f);
    pid_roll_Ki = NRF24_PID_Ki[NRF_PID_Roll].getFloat("roll ki", 0.0f);
    pid_roll_Kd = NRF24_PID_Kd[NRF_PID_Roll].getFloat("roll kd", 0.0f);

    NRF24_PID_Kp[NRF_PID_Pitch].begin("pid pitch kp");
    NRF24_PID_Ki[NRF_PID_Pitch].begin("pid pitch ki");
    NRF24_PID_Kd[NRF_PID_Pitch].begin("pid pitch kd");

    pid_pitch_Kp = NRF24_PID_Kp[NRF_PID_Pitch].getFloat("pitch kp", 0.0f);
    pid_pitch_Ki = NRF24_PID_Ki[NRF_PID_Pitch].getFloat("pitch ki", 0.0f);
    pid_pitch_Kd = NRF24_PID_Kd[NRF_PID_Pitch].getFloat("pitch kd", 0.0f);

    NRF24_PID_Kp[NRF_PID_Yaw].begin("pid yaw kp");
    NRF24_PID_Ki[NRF_PID_Yaw].begin("pid yaw ki");
    NRF24_PID_Kd[NRF_PID_Yaw].begin("pid yaw kd");

    pid_yaw_Kp = NRF24_PID_Kp[NRF_PID_Yaw].getFloat("yaw kp", 0.0f);
    pid_yaw_Ki = NRF24_PID_Ki[NRF_PID_Yaw].getFloat("yaw ki", 0.0f);
    pid_yaw_Kd = NRF24_PID_Kd[NRF_PID_Yaw].getFloat("yaw kd", 0.0f);


    for(uint8_t i = 0; i < 3; i++)
    {
        NRF24_PID_Kp[i].end();
        NRF24_PID_Ki[i].end();
        NRF24_PID_Kd[i].end();
    }

    NRF_CE_LOW();

    NRF24_PowerUp();

    TickType_t xLastWake;

    while(1)
    {
        //printf("op: %d\n", oled_pref_struct.send_op);

        vTaskDelay(100 / portTICK_RATE_MS);

        xLastWake = xTaskGetTickCount();

        while(oled_pref_struct.send_op == 1)
        {
            vTaskDelayUntil(&xLastWake, NRF_Speed);
            //printf("sending\n");
            NRF_CE_LOW();

            if(data_exchange_lock == 0)
            {
                //NRF_CE_LOW();

                NRF24_Load_payload(nrf_temp_tx_ram);

                NRF_CE_HIGH();
            }
        }
    }
}

void nrf_recv_operation(void *pt)
{
    while(1)
    {
        if(xSemaphoreTake(xNRF_IRQ, 45) == pdTRUE)
        {
            NRF24_PTX_IRQHandler();
        }
    }
}

void IRAM_ATTR nrf_irq_operation(void)
{
    xSemaphoreGiveFromISR(xNRF_IRQ, NULL);
}

