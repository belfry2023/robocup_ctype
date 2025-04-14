#include "car.h"
#include "motor.h"
#include "tim.h"
#include "message_center.h"
#include "Car_def.h"
#include "remote.h"
static motor_t *l_motor, *r_motor;
static Subscriber_t *cmd_sub;
static Car_Ctrl_Cmd_s car_cmd;
void car_init()
{
    
	motor_c l_config = {
		.motor_tim_driver = &htim8,
		.motor_tim_encoder = &htim1,
		.Channel = TIM_CHANNEL_2,
		.position_pid = {
			.Kp = 60, // 4.5
			.Ki = 0,  // 0
			.Kd = 3.5,  // 0
			.IntegralLimit = 3000,
			.MaxOut = 12000,
		},
		.rotation_pid = {
			.Kp = 2, // 4.5
			.Ki = 0,  // 0
			.Kd = 0,  // 0
			.IntegralLimit = 3000,
			.MaxOut = 12000,
		},
		.flag_reverse = 1,
		.gpio = GPIOB,
		.gpio_pin_1 = GPIO_PIN_12,
		.gpio_pin_2 = GPIO_PIN_13,
	};
	motor_c r_config = {
		.motor_tim_driver = &htim8,
		.motor_tim_encoder = &htim1,
		.Channel = TIM_CHANNEL_3,
		.position_pid = {
			.Kp = 60, // 4.5
			.Ki = 0,  // 0
			.Kd = 3.5,  // 0
			.IntegralLimit = 3000,
			.MaxOut = 100,
		},
		.rotation_pid = {
			.Kp = 2, // 4.5
			.Ki = 0,  // 0
			.Kd = 0,  // 0
			.IntegralLimit = 3000,
			.MaxOut = 2000,
		},
		.flag_reverse = 1,
		.gpio = GPIOB,
		.gpio_pin_1 = GPIO_PIN_15,
		.gpio_pin_2 = GPIO_PIN_14,
	};
	
	l_motor = motor_init(&l_config);
	r_motor = motor_init(&r_config);
	
	cmd_sub = SubRegister("Car_cmd", sizeof(Car_Ctrl_Cmd_s));
   
}

void car_task()
{
	SubGetMessage(cmd_sub, &car_cmd);
    motor_cal();
    l_motor->PWM_Ref = 2000 * (0.5 * car_cmd.vx + 0.5 * car_cmd.wz)/660;
    r_motor->PWM_Ref = 2000 * (0.5 * car_cmd.vx - 0.5 * car_cmd.wz)/660;
}
