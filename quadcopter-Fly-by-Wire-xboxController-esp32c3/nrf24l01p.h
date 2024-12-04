#ifndef __NRF24L01P_H__
#define __NRF24L01P_H__

#include <stdint.h>

#include "nrf24l01p_config.h"
#include "nrf24l01p_tools.h"


/******************** Include Files here ********************/

#include "Arduino.h"
#include "nrf24.h"

/************************************************************/

/******************** Functions that needs override ********************/
#define NRF_CE_HIGH()                   digitalWrite(NRF_CE, HIGH)
#define NRF_CE_LOW()                    digitalWrite(NRF_CE, LOW)

#define NRF_CS_HIGH()                   digitalWrite(NRF_NSS, HIGH)
#define NRF_CS_LOW()                    digitalWrite(NRF_NSS, LOW)
// Maybe try insert these functions in your own code.



/*
* NRF_Writebyte(uint8_t command, uint8_t reg_data)
*/
#define spi_write_byte                  NRF_Writebyte

/*
* NRF_Writebytes(uint8_t command, uint8_t *reg_data, uint8_t data_len)
*/
#define spi_write_bytes                 NRF_Writebytes

/*
* NRF_Readbyte(uint8_t command, uint8_t *data_p, uint8_t dummy_byte)
*/
#define spi_read_byte                   NRF_Readbyte

/*
* NRF_Readbytes(uint8_t command, uint8_t *data_p, uint8_t data_len, uint8_t dummy_byte)
*/
#define spi_read_bytes                  NRF_Readbytes

/***********************************************************************/
#define NRF_Success                     ((uint8_t) 0)
#define NRF_Error                       ((uint8_t) 1)

#define NRF_PTX                         ((uint8_t) 0)
#define NRF_PRX                         ((uint8_t) 1)

#define NRF_Channel                     56
#define NRF_RADIO_POWER                 NRF_RF_POWER_0dBm
#define NRF_RADIO_RATE                  NRF_RF_RATE_2Mbps
#define NRF_RETRANS_DELAY               NRF_RETRANS_DELAY_500us
#define NRF_RETRANS_COUNT               NRF_RETRANS_COUNT_Up_to_2


// esp32
extern char nrf_send[];
// stm32
extern char nrf_receive[];

extern uint8_t nrf_temp_tx_ram[];
extern uint8_t nrf_temp_rx_ram[];

extern uint8_t nrf_status;


void NRF24_Init(uint8_t mode);
void NRF24_Load_payload(uint8_t *data);
uint8_t NRF24_Unload_payload(uint8_t *data);
void NRF24_PTX_IRQHandler(void);
void NRF24_PRX_IRQHandler(void);

void NRF24_TX_Init(uint8_t channel, uint8_t radio_power, uint8_t radio_speed, uint8_t retrans_delay, uint8_t retrans_count);
void NRF24_RX_Init(uint8_t channel, uint8_t radio_power, uint8_t radio_speed, uint8_t retrans_delay, uint8_t retrans_count);

void NRF24_PowerUp(void);
void NRF24_PowerDown(void);

void NRF24_TX_OnLoop(void);
void NRF24_RX_OnLoop(void);

void NRF24_Set_RF_Channel(uint8_t freq);
void NRF24_Get_RF_Channel(uint8_t *rf_freq);
void NRF24_Set_RF_Rate(uint8_t rf_rate);
void NRF24_Get_RF_Rate(uint8_t *rf_rate);
void NRF24_Set_RF_power(uint8_t rf_power);
void NRF24_Get_RF_power(uint8_t *rf_power);
uint8_t NRF24_Get_IRQ_bits(void);
void NRF24_Get_Status(uint8_t *rf_status);
void NRF24_Clear_IRQ_RX_Ready(void);
void NRF24_Clear_IRQ_TX_Sent(void);
void NRF24_Clear_IRQ_Max_transmit(void);
void NRF24_Clear_All_IRQ(void);
void NRF24_Get_Lost_packets(uint8_t *lost_pack);
void NRF24_Get_Resend_times(uint8_t *resend_time);
uint8_t NRF24_Get_Receive_power_detector(void);
void NRF24_Set_RX_Address_pipe_0(char *addr);
void NRF24_Set_RX_Address_pipe_1(char *addr);
void NRF24_Set_RX_Address_pipe_2_to_5(char addr_p2, char addr_p3, char addr_p4, char addr_p5);
void NRF24_Set_TX_Address(char *addr);
void NRF24_Set_RX_Payload_byte_num(uint8_t pipe_num, uint8_t byte_num);
uint8_t NRF24_Get_FIFO_Status(void);
void NRF24_Get_Dynamic_payload_length(uint8_t *payload_len);
void NRF24_Set_Dynamic_payload_length(uint8_t pipe_num, uint8_t state);
void NRF24_Get_Feature(uint8_t *rf_feature);
void NRF24_Cmd_dynamic_payload(uint8_t state);
void NRF24_Cmd_payload_ack(uint8_t state);
void NRF24_Cmd_tx_payload_no_ack(uint8_t state);
void NRF24_Read_RX_payload(uint8_t *data);
void NRF24_Write_TX_payload(uint8_t *data);
void NRF24_Flush_TX_FIFO(void);
void NRF24_Flush_RX_FIFO(void);
uint8_t NRF24_Read_RX_payload_length(void);
void NRF24_Write_RX_ACK_payload(uint8_t pipe_num, uint8_t *data);



#endif

