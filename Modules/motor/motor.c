#include "motor.h"
#include "stdlib.h"
#include "string.h"
#include "tim.h"

static motor_t *motor[2] = {NULL};
static uint8_t idx;

static uint8_t SetSpeed(float speed, motor_t *motor)
{
	if(speed<0)
	{
		motor->flag_reverse = 0;
		speed = -speed;
	}else motor->flag_reverse = 1;
	if(motor->flag_reverse)
	{
		HAL_GPIO_WritePin(motor->gpio,motor->gpio_pin_1,0);
		HAL_GPIO_WritePin(motor->gpio,motor->gpio_pin_2,1);
	}else
	{
		HAL_GPIO_WritePin(motor->gpio,motor->gpio_pin_1,1);
		HAL_GPIO_WritePin(motor->gpio,motor->gpio_pin_2,0);
	}
	__HAL_TIM_SET_COMPARE(motor->motor_tim_driver, motor->Channel, speed);
	return motor->Channel;
}

static void GetMeassure(motor_t *motor)
{
	motor->dt = DWT_GetDeltaT(&motor->feed_cnt);
	motor->Position = (int16_t)__HAL_TIM_GET_COUNTER(motor->motor_tim_encoder);
	motor->Rotation = (motor->Position - motor->Position_last) / motor->dt;
	motor->Position_last = motor->Position;
}

motor_t *motor_init(motor_c *config)
{
	motor_t *_instance = (motor_t*)malloc(sizeof (motor_t));
	memset(_instance, 0, sizeof(motor_t));
	_instance->gpio = config->gpio;
	_instance->gpio_pin_1 = config->gpio_pin_1;
	_instance->gpio_pin_2 = config->gpio_pin_2;
	_instance->motor_tim_driver = config->motor_tim_driver;
	_instance->motor_tim_encoder = config->motor_tim_encoder;
	_instance->Channel = config->Channel;
	HAL_TIM_Encoder_Start(_instance->motor_tim_encoder,TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(_instance->motor_tim_encoder,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(_instance->motor_tim_driver,_instance->Channel);
    PIDInit(&_instance->position_pid, &config->position_pid);
	PIDInit(&_instance->rotation_pid, &config->rotation_pid);
	_instance->flag_reverse = config->flag_reverse;
	
	motor[idx++] = _instance;
	
	return _instance;
}

void motor_cal()
{
	for(size_t i = 0; i<idx; i++)
	{
		GetMeassure(motor[i]);
		motor[i]->PWM_Ref = motor[i]->Rotation_Ref;
		motor[i]->PWM_Ref = PIDCalculate(&motor[i]->rotation_pid, motor[i]->Rotation, motor[i]->PWM_Ref);
//		motor->PWM_Ref = PIDCalculate(&motor[i]->speed_pid, motor[i]->Speed, motor[i]->Speed_Ref);
//		motor->PWM_Ref = PIDCalculate(&motor[i]->position_pid, motor[i]->Position, motor->PWM_Ref);
//		motor->PWM_Ref = PIDCalculate(&motor[i]->rotation_pid, motor[i]->Rotation, motor->PWM_Ref);
	}
}


void motor_control()
{
	for(size_t i = 0; i<idx; i++)
	{
		motor[i]->PWM_Ref = motor[i]->PWM_Ref > 2000 ? 2000 : motor[i]->PWM_Ref;
		motor[i]->PWM_Ref = motor[i]->PWM_Ref < -2000 ? -2000 : motor[i]->PWM_Ref;
		SetSpeed(motor[i]->PWM_Ref, motor[i]);
	}
}


