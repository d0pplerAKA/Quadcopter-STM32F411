#include "stm32f4xx.h"
#include "arm_math.h"

#include "delay/delay.h"
#include "uart/uart.h"
#include "led/led.h"

#include "adc_voltage/adc_voltage.h"

#include "post/post.h"

#include "fwb_stm32f4/fwb.h"

#include "flash_stm32f4/flash.h"



void setup(void);
void stm_restart(void);


int main(void)
{
	delay_ms(2000);

	xbox_recovery_flag = 1;

	setup();
	
	for(uint8_t i = 0; i < 15; i++)
    {
        I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
        delay_ms(75);
        I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode5);
        delay_ms(75);
    }

    I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
	delay_ms(75);
	I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
	delay_ms(75);
    I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
    delay_ms(75);
	
	Timer_Cmd();

	TOF_OnLoopTrigger();

	Drone_Motor.value_L1 = 0.0f;
	Drone_Motor.value_L2 = 0.0f;
	Drone_Motor.value_R1 = 0.0f;
	Drone_Motor.value_R2 = 0.0f;

	BLDC_SetSpeed(&Drone_Motor);

	xbox_recovery_flag = 0;

		
	while(1)
	{
		//printf("Height=%.2f,z=0\n", TOF_Height());
		//GL9306_toString();
		//printf("Acc x: %.2f, Acc y: %.2f\n", GY_Accel_x(), GY_Accel_y());
		//printf("roll=%.2f, pitch=%.2f, yaw=%.2f\n", GY_Roll(), GY_Pitch(), GY_Yaw());
		//printf("Gyro x: %.2f Gyro y: %.2f Gyro z: %.2f\n", lpf_roll.output, lpf_pitch.output, lpf_yaw.output);
		//printf("yaw output: %.3f\n", pid_yaw.output);
		//printf("X: %d, Y: %d, ENV: %d\n", GL_X(), GL_Y(), GY_ENV());
		//printf("Speed x: %.2f y: %.2f Displacement: x: %.2f y: %.2f\n", GL_SpeedX(), GL_SpeedY(), GL_DisX(), GL_DisY());
		
		
		//printf("flowX=%.2f, flowY=%.2f\n", pid_flow_x.output, pid_flow_y.output);
		
		//printf("yaw output: %.3f\n", pid_yaw.output * FWB_rad2deg);

		/*
		printf("L1: %.2f L2: %.2f R1: %.2f R2: %.2f\n", 
					Drone_Motor.value_L1, Drone_Motor.value_L2, Drone_Motor.value_R1, Drone_Motor.value_R2);
		*/
		
		/*
		printf("Roll=%.3f,Pitch=%.3f,Yaw=%.3f,HT=%.3f,Target_R=%.3f,Target_P=%.3f,Output_R=%.3f,Output_P=%.3f\n",
					GY_Roll(), GY_Pitch(), GY_Yaw(), MS_Height(), drone_attitude.desired_roll, drone_attitude.desired_pitch,
					pid_roll_gyro.output/100.0f, pid_pitch_gyro.output/100.0f);
		*/
		
		

		//printf("PID Roll: %.3f Output: %.3f\n", GY_Roll(), pid_roll_gyro.output);
		//printf("PID Pitch: %.3f Output: %.3f\n", GY_Pitch(), pid_pitch_gyro.output);
		//printf("PID Yaw: %.3f Output: %.3f\n", GY_Gyro_z(), pid_yaw.output);
		
		
		if(package_is_control_message && nrf_send_raw.is_control_message == 0xCE)
		{
			if(xbox_abort_flag >= 3)
			{
				xbox_recovery_flag = 1;

				Drone_Motor.value_L1 = 0.0f;
				Drone_Motor.value_L2 = 0.0f;
				Drone_Motor.value_R1 = 0.0f;
				Drone_Motor.value_R2 = 0.0f;

				BLDC_SetSpeed(&Drone_Motor);
				
				throttle_val = 0;

				printf("Abort! Pressed. \n");

				
				while(1)
				{
					Drone_Motor.value_L1 = 0.0f;
					Drone_Motor.value_L2 = 0.0f;
					Drone_Motor.value_R1 = 0.0f;
					Drone_Motor.value_R2 = 0.0f;

					BLDC_SetSpeed(&Drone_Motor);

					I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode5);
					delay_ms(100);
					I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
					delay_ms(100);
					I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode5);
					delay_ms(100);
					I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
					delay_ms(100);
					
					delay_ms(500);


					if(xbox_upload_parameter_flag > 3)
					{
						__set_PRIMASK(1);
						delay_ms(25);

						/********************/
						printf("Uploading pid attitude data. \n");
						/*
						* Save Flash Data
						*/

						Flash_data_toString();

						memcpy(flash_pid_u32_buffer, &flash_pid_float, sizeof(uint32_t) * FLASH_BUFFER_SIZE);

						Flash_data_Update(flash_pid_u32_buffer, FLASH_BUFFER_SIZE);


						/********************/

						//Drone_data_calibration(&Drone_offset);

						for(uint8_t i = 0; i < 7; i++)
						{
							I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
							delay_ms(100);
							I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode2);
							delay_ms(150);
							I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
							delay_ms(100);
						}

						__set_PRIMASK(0);


						xbox_upload_parameter_flag = 0;
						motor_startup = 0.0f;
						xbox_recovery_flag = 0;
						xbox_abort_flag = 0;

						printf("Done. \n");
						TIM_Cmd(TIM_GY95T, ENABLE);
						
						break;
					} // upload

					if(xbox_recovery_flag > 3)
					{
						//__set_PRIMASK(1);
						//Drone_data_calibration(&Drone_offset);
						//__set_PRIMASK(0);


						for(uint8_t i = 0; i < 7; i++)
						{
							I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
							delay_ms(100);
							I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode3);
							delay_ms(150);
							I2C_SendTo_EspSoc(I2C_Esp32Soc_Register_WS2812_Mode, I2C_Esp32Soc_WS2812_Mode0);
							delay_ms(100);
						}

						printf("System recovery. \n");

						xbox_upload_parameter_flag = 0;
						motor_startup = 0.0f;
						xbox_recovery_flag = 0;
						xbox_abort_flag = 0;

						TIM_Cmd(TIM_GY95T, ENABLE);

						break;
					} // recovery
				} // while 1
			}  // abort flag
		}
		else
		{
			Drone_Motor.value_L1 = 0.0f;
			Drone_Motor.value_L2 = 0.0f;
			Drone_Motor.value_R1 = 0.0f;
			Drone_Motor.value_R2 = 0.0f;

			BLDC_SetSpeed(&Drone_Motor);
			
			xbox_upload_parameter_flag = 1;
			motor_startup = 0.0f;
			
			//printf("No Control, Zero Speed. \n");

		} //if(package_is_control_message && nrf_send_raw.is_control_message == 0xCE)

		//printf("drone_attitude.desired_roll: %.3f\n", pid_roll_degree.output);

		//printf("Roll: %.2f Gyro: %.2f output: %.2f\n", GY_Roll(), GY_Gyro_x(), pid_roll_degree.output);
		//printf("Gyro z: %.3f MFC: %d\n", GY_Gyro_z(), GY_MFC());

		//printf("motor_lock %d\n", motor_lock);
		
		/*
		printf("L1: %.2f L2: %.2f R1: %.2f R2: %.2f\n", 
			Drone_Motor.value_L1, Drone_Motor.value_L2, Drone_Motor.value_R1, Drone_Motor.value_R2);
		*/
	}
}

void setup(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
	Drone_NVIC_Init();

	UART1_Init(115200);
	UART2_Init(19200);
	UART2_DMA_Init();
	printf("Uart Init\n");

	LED_Init();
	printf("LED init\n");
	
	Flash_data_Read(flash_pid_u32_buffer, FLASH_BUFFER_SIZE);
	memcpy(&flash_pid_float, flash_pid_u32_buffer, sizeof(uint32_t) * FLASH_BUFFER_SIZE);
	Flash_data_toString();

	FWB_Init();

	Drone_Init(Drone_POST());

	delay_ms(1000);
}

void stm_restart(void)
{
	//SCB->AIRCR = 0x05FA0000 | (uint32_t) 0x04;

	__set_FAULTMASK(ENABLE);

	NVIC_SystemReset();
}

