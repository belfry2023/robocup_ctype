#ifndef __MODULE_SERVO_H
#define __MODULE_SERVO_H

#include "tim.h"
#include "stdint.h"
#include "main.h"
#include "PID.h"

typedef struct servo_typedef
{
    TIM_HandleTypeDef *motor_tim_driver; // 驱动定时器
    uint32_t Channel;                    // PWM通道
    float angle_max;
    float angle; // 当前角度
    float angle_target;
    float speed;
    PIDInstance angle_pid;
} servo_t;

typedef struct servo_config
{
    TIM_HandleTypeDef *motor_tim_driver; // 驱动定时器
    uint32_t Channel;                    // PWM通道
    float angle_max;                  // 最大角度
    PID_Init_Config_s angle_pid_config;
} servo_c;

servo_t *servo_init(servo_c *config);

void servo_set_angle(servo_t *servo, float angle);
void servo_set_pwm(); // 设置PWM输出
void servo_set_speed(servo_t *servo, float speed);
void servo_set_angle_target(servo_t *servo,float target);
void servo_control();

#endif // __MODULE_SERVO_H