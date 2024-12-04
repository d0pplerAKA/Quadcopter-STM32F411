#include "flash.h"

Flash_PID_Float_t flash_pid_float;

uint32_t flash_pid_u32_buffer[FLASH_BUFFER_SIZE];


uint32_t Flash_Sector_Convert(uint32_t flash_addr_sector)
{
    if(flash_addr_sector == FLASH_ADDR_SECTOR_0) return FLASH_Sector_0;
    else if(flash_addr_sector == FLASH_ADDR_SECTOR_1) return FLASH_Sector_1;
    else if(flash_addr_sector == FLASH_ADDR_SECTOR_2) return FLASH_Sector_2;
    else if(flash_addr_sector == FLASH_ADDR_SECTOR_3) return FLASH_Sector_3;
    else if(flash_addr_sector == FLASH_ADDR_SECTOR_4) return FLASH_Sector_4;
    else if(flash_addr_sector == FLASH_ADDR_SECTOR_5) return FLASH_Sector_5;
    else if(flash_addr_sector == FLASH_ADDR_SECTOR_6) return FLASH_Sector_6;
    else return FLASH_Sector_7;
}


void Flash_data_Update(uint32_t * data_to_write, uint32_t buffer_size)
{
    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP |
                    FLASH_FLAG_OPERR |
                    FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR |
                    FLASH_FLAG_PGPERR |
                    FLASH_FLAG_PGSERR);

    //FLASH_DataCacheCmd(DISABLE);

    FLASH_EraseSector(Flash_Sector_Convert(FLASH_ADDR_SECTOR_IN_USE), VoltageRange_3);

    for(uint32_t i = 0; i < buffer_size; i++)
    {
        uint32_t temp_i = i * 4;

        FLASH_ProgramWord(temp_i + FLASH_ADDR_SECTOR_IN_USE, data_to_write[i]);
    }
    
    FLASH_Lock();
}

void Flash_data_Read(uint32_t * data_to_read, uint32_t buffer_size)
{
    memcpy(data_to_read, (void *)FLASH_ADDR_SECTOR_IN_USE, sizeof(uint32_t) * buffer_size);
}

void Flash_data_toString(void)
{
    printf("    ----------------------------------------\n");
    printf("    <Storage PID DATA> \n\n");
    printf("        [type]    Kp        Ki      Kd    \n");
    printf("        [roll]    %.3f      %.3f    %.3f  \n", flash_pid_float.kp_roll, 
                                                        flash_pid_float.ki_roll, 
                                                        flash_pid_float.kd_roll);
    printf("        [pitch]   %.3f      %.3f    %.3f  \n", flash_pid_float.kp_pitch, 
                                                        flash_pid_float.ki_pitch, 
                                                        flash_pid_float.kd_pitch);
    printf("        [yaw]     %.3f      %.3f    %.3f  \n", flash_pid_float.kp_yaw,
                                                        flash_pid_float.ki_yaw,
                                                        flash_pid_float.kd_yaw);
    printf("    ----------------------------------------\n");
}
