#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "tim.h"
#include "gpio.h"

#ifndef __MOTOR_H
#define __MOTOR_H
#include <stdint.h>
#include "main.h"
#include "PID.h"
typedef struct motor_typedef
{
	PIDInstance position_pid;
	PIDInstance rotation_pid;
  float dt;
	float Rotation_Ref;
	float PWM_Ref;
	float Rotation;
	float Position;
	float Position_last;
	uint8_t flag_reverse;
	uint16_t gpio_pin_1;
	uint16_t gpio_pin_2;
	uint32_t Channel;
	uint32_t feed_cnt;
	TIM_HandleTypeDef *motor_tim_encoder;
	TIM_HandleTypeDef *motor_tim_driver;
	GPIO_TypeDef *gpio;
}motor_t;

typedef struct motor_config
{
	PID_Init_Config_s position_pid;
	PID_Init_Config_s rotation_pid;
	uint32_t Channel;
	uint16_t gpio_pin_1;
	uint16_t gpio_pin_2;
	uint8_t flag_reverse;
	TIM_HandleTypeDef *motor_tim_encoder;
	TIM_HandleTypeDef *motor_tim_driver;
	GPIO_TypeDef *gpio;
}motor_c;

motor_t *motor_init(motor_c *config);
void motor_cal();
void motor_control();
#endif





#endif // !__MOTOR_H__