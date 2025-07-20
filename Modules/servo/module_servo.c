#include "module_servo.h"
#include "tim.h"
#include "stdint.h"
static servo_t *servo[2] = {NULL};
static uint8_t idx;


servo_t *servo_init(servo_c *config)
{
    servo_t *_instance = (servo_t*)malloc(sizeof (servo_t));
    memset(_instance, 0, sizeof(servo_t));
    
    PIDInit(&_instance->angle_pid, &config->angle_pid_config);

    _instance->motor_tim_driver = config->motor_tim_driver;
    _instance->Channel = config->Channel;
    _instance->angle_max = config->angle_max;
    HAL_TIM_PWM_Start(_instance->motor_tim_driver,_instance->Channel);
    
    servo[idx++] = _instance;
    
    return _instance;
}

void servo_set_angle(servo_t *servo, float angle)
{
    if (servo == NULL) return;
    
    servo->angle = angle; // 将角度转换为无符号整数
    
}

void servo_set_speed(servo_t *servo, float speed)
{
    if(servo == NULL) return;
    servo->speed = speed;
    servo_set_angle(servo, servo->angle + speed * servo->angle_pid.dt);
}

void servo_set_angle_target(servo_t *servo, float target)
{
    if (servo == NULL) return;
    target = target > servo->angle_max ? servo->angle_max : target;
    target = target > 0 ? target : 0;
    servo->angle_target = target;
}

void servo_control()
{
    for(uint8_t i = 0; i < idx; i++)
    {
        servo_set_speed(servo[i], PIDCalculate(&servo[i]->angle_pid, servo[i]->angle, servo[i]->angle_target));
    }
}

void servo_set_pwm()
{
    // 假设角度范围是0-180度，对应PWM占空比0-100%
    // 这里需要根据具体的伺服电机进行调整
    for(uint8_t i = 0; i < idx; i++)
    {
        servo_t *s = servo[i];
        if (s->angle > s->angle_max) s->angle = s->angle_max;
        if (s->angle < 0) s->angle = 0;
        uint32_t pwm_value = (uint32_t)(s->angle / s->angle_max * 2000) + 500; // 500-2500us
        __HAL_TIM_SET_COMPARE(s->motor_tim_driver, s->Channel, pwm_value);
    }
    // uint32_t pwm_value = (uint32_t)(angle / servo->angle_max * 2000) + 500; 
    // __HAL_TIM_SET_COMPARE(servo->motor_tim_driver, servo->Channel, pwm_value);
}