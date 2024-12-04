#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f4xx.h"

#include "stdio.h"
#include "string.h"


#define FLASH_SUCCESS                       0
#define FLASH_ERROR                         1

// Code size:                                     0x08006244
#define FLASH_ADDR_SECTOR_0                 ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes
#define FLASH_ADDR_SECTOR_1                 ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes
#define FLASH_ADDR_SECTOR_2                 ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes
#define FLASH_ADDR_SECTOR_3                 ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes

#define FLASH_ADDR_SECTOR_4                 ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes

#define FLASH_ADDR_SECTOR_5                 ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes
#define FLASH_ADDR_SECTOR_6                 ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes
#define FLASH_ADDR_SECTOR_7                 ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes


#define FLASH_ADDR_SECTOR_IN_USE            FLASH_ADDR_SECTOR_3

#define FLASH_BUFFER_SIZE                   9





typedef struct
{
    float kp_roll;
    float kp_pitch;
    float kp_yaw;
    
    float ki_roll;
    float ki_pitch;
    float ki_yaw;

    float kd_roll;
    float kd_pitch;
    float kd_yaw;

    /* data */
} __attribute__((packed)) Flash_PID_Float_t;



extern Flash_PID_Float_t flash_pid_float;
extern uint32_t flash_pid_u32_buffer[FLASH_BUFFER_SIZE];

uint32_t Flash_Sector_Convert(uint32_t flash_addr_sector);
void Flash_data_Update(uint32_t * data_to_write, uint32_t buffer_size);
void Flash_data_Read(uint32_t * data_to_read, uint32_t buffer_size);
void Flash_data_toString(void);



#endif


/*
    王禹衡 青少年 Poster 分享者

    电子/电气工程本科，大一在读，因为热爱所以选择。
    普普通通的单片机小极客，尝试入门嵌入式和机器人技术领域，争做一个全栈开发者。
    很高兴能够参与到本次集智年会当中，给各位大佬汇报自己的学习、发现以及小成果！ Orz

    本次汇报关键词: 自抗扰(LADRC)、PID、分布式、自适应简单系统
*/
