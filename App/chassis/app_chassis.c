#include "app_chassis.h"
#include "module_motor.h"
#include "bsp_dwt.h"
#include "module_ir.h"
#include "message_center.h"
#include "ins_task.h"
#include "task_def.h"
static IICMotorInstance *motor[4] = {NULL};
static module_ir_t *ir = NULL;
static Subscriber_t *car_sub;
static Publisher_t *car_pub; // 发布者实例指针
static car_control_t car_control; // 车的控制指令
static car_state_t car_state; // 车的状态
static attitude_t *car_IMU_data; 
static uint8_t turn_flag;
static float yaw_last;
static PIDInstance follow_pid;
static uint8_t flag;
void chassis_init()
{
	
	car_IMU_data = INS_Init(); // IMU先初始化,获取姿态数据指针赋给yaw电机的其他数据来源
	motor_config cfg = {
		.cfg = {
			.Kp = -50, // 4.5
			.Ki = -4,  // 0
			.Kd = 0,  // 0
			.IntegralLimit = 3000,
			.MaxOut = 3000
		}
	};
    for(int i = 0; i < 4; i++) {
		cfg.id = i;
		motor[i] = Motor_Init(&cfg);
		if (motor[i] == NULL) {
			// Handle memory allocation failure
			return;
		}
	}
	ir = ir_init();
	if (ir == NULL) {
		// Handle memory allocation failure
		return;
	}
	
	PID_Init_Config_s cfg_ = {
		.Kp = 0.5, // 4.5
		.Ki = 0,  // 0
		.Kd = 0,  // 0
		.IntegralLimit = 3000,
		.MaxOut = 3000
	};

	PIDInit(&follow_pid,&cfg_);

	Motor_Start(1, 20, 13, 48.00); // Example parameters for motor initialization
	car_sub = SubRegister("car_control", sizeof(car_control_t)); // 订阅车的控制指令
    car_pub = PubRegister("car_state", sizeof(car_state_t)); // 发布车的状态
}

static void car_move_toward(int16_t speed)
{
	Set_motor_speed(motor[0],speed);
	Set_motor_speed(motor[1],speed);
	Set_motor_speed(motor[2],speed);
	Set_motor_speed(motor[3],speed);
}

static void car_move_turn_right(int16_t wz, int16_t r)
{
	int16_t speedR = (r - 3) * wz;
	int16_t speedL = (r + 3) * wz;
	Set_motor_speed(motor[0],speedL);
	Set_motor_speed(motor[1],speedL);
	Set_motor_speed(motor[2],speedR);
	Set_motor_speed(motor[3],speedR);
}

static void check()
{
	uint16_t err = 40*(1-ir->ir_data[0]) + 20*(1-ir->ir_data[1]);
	uint16_t err_ = 40*(1-ir->ir_data[7]) + 20*(1-ir->ir_data[6]);
	turn_flag = 0;
	if(err != 0 && err_ == 0)//左转，--
	{
		flag = 8;
		car_move_turn_right(-err,-3);
	}
	else
	if(err == 0 && err_ != 0)//右转，++
	{
		flag = 7;
		car_move_turn_right(err_,3);
	}
	else
	if(err == 0 && err_ == 0)
	{
		flag = 6;
		car_move_toward(100);
	}
	else
	if(err != 0 && err_ != 0)
	{
		flag = 5;
		yaw_last = car_IMU_data->YawTotalAngle;
		turn_flag = 1;
	}
}



void chassis_task()
{
	SubGetMessage(car_sub, &car_control); // 获取车的控制指令
	INS_Task(); // 执行惯性导航系统任务
	if(car_control.begin)
	{
	if(turn_flag == 0)
	{
		check();
	}else
	{
		if(car_control.flag == 1)//左转90，--
		{
			int16_t speed = -50;
			flag = 1;
			car_move_turn_right(speed, -3);
		}else
		if(car_control.flag == 2)//右转90，++
		{
			int16_t speed = 50;
			flag = 2;
			car_move_turn_right(speed, 3);
		}
		else
		if(car_control.flag == 0){
			flag = 3;
			car_move_toward(100);
		}
		if(car_IMU_data->YawTotalAngle - yaw_last > 90 || car_IMU_data->YawTotalAngle - yaw_last < -90)
		{
			flag = 4;
			turn_flag = 0;
			yaw_last = car_IMU_data->YawTotalAngle;
		}
	}
	}
	//car_move_turn_right(100,3);
	deal_IRdata(); // Process IR data
	Motor_control(); // Control motors based on the data
	car_state.flag = flag;
    PubPushMessage(car_pub, &car_state); // 发布车的状态
}