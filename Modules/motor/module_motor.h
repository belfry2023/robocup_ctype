#ifndef __BSP_MOTOR_IIC_H_
#define __BSP_MOTOR_IIC_H_

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "PID.h"

#define Motor_model_ADDR    0x26


typedef enum __Motor_IIC_ADDR_
{
	
	MOTOR_TYPE_REG = 0x01, 
	MOTOR_DeadZONE_REG = 0x02, 
	MOTOR_PluseLine_REG = 0x03, 
	MOTOR_PlusePhase_REG = 0x04, 
	WHEEL_DIA_REG = 0x05, 
	SPEED_Control_REG = 0x06, 
	PWM_Control_REG = 0x07, 
	
	READ_TEN_M1Enconer_REG = 0x10,
	READ_TEN_M2Enconer_REG = 0x11, 
	READ_TEN_M3Enconer_REG = 0x12, 
	READ_TEN_M4Enconer_REG = 0x13,
	
	READ_ALLHigh_M1_REG = 0x20, 
	READ_ALLLOW_M1_REG = 0x21,
	
	READ_ALLHigh_M2_REG = 0x22, 
	READ_ALLLOW_M2_REG = 0x23, 
	
	READ_ALLHigh_M3_REG = 0x24, 
	READ_ALLLOW_M3_REG = 0x25, 
	
	READ_ALLHigh_M4_REG = 0x26,
	READ_ALLLOW_M4_REG = 0x27,
	
	IIC_REG_MAX 

}Motor_IIC_ADDR_t;

typedef struct __IICMotorInstance
{
	int16_t speed;
	int16_t pwm;
	int encoder_offset;
	int encoder_now;
	PIDInstance pid;
	uint8_t id;
} IICMotorInstance;

typedef struct motor_set_controller
{
	PID_Init_Config_s cfg;
	uint8_t id;
}motor_config;


void Motor_Start(uint8_t type, uint16_t Pluse_Phase, uint16_t Pluse_Line, float Wheel_Dia);
void Set_motor_pwm(IICMotorInstance *motor_instance, int16_t pwm);
void Set_motor_speed(IICMotorInstance *motor_instance, int16_t speed);
void Get_Motor_Data();
IICMotorInstance* Motor_Init(motor_config *cfg);
void Motor_control();
#endif
