#include "module_motor.h"
#include "bsp_i2c.h"
#include "i2c.h"
#include "bsp_dwt.h"
static IICMotorInstance *motor_instances[4] = {NULL};
static uint8_t idx;
static float_to_bytes(float f, uint8_t *bytes) 
{
    memcpy(bytes, &f, sizeof(float));
}


static char2float(char *p)
{
  float *p_Int;
  p_Int = (float *)malloc(sizeof(float));
  memcpy(p_Int, p, sizeof(float));
  float x = *p_Int;
  free(p_Int);
  return x;
}

static IIC_Motor_Init(void)
{			
    I2C_GPIO_Init(); // Initialize GPIO for I2C
}

static Set_motor_type(uint8_t data)
{	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_TYPE_REG,2,&data);
}

static Set_motor_deadzone(uint16_t data)
{
	static uint8_t buf_tempzone[2];
	
	buf_tempzone[0] = (data>>8)&0xff;
	buf_tempzone[1] = data;
	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_DeadZONE_REG,2,buf_tempzone);
}


static Set_Pluse_line(uint16_t data)
{
	static uint8_t buf_templine[2];
	
	buf_templine[0] = (data>>8)&0xff;
	buf_templine[1] = data;
	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_PluseLine_REG,2,buf_templine);
}


static Set_Pluse_Phase(uint16_t data)
{
	static uint8_t buf_tempPhase[2];
	
	buf_tempPhase[0] = (data>>8)&0xff;
	buf_tempPhase[1] = data;
	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_PlusePhase_REG,2,buf_tempPhase);
}


static Set_Wheel_dis(float data)
{
	static uint8_t bytes[4];
	
	float_to_bytes(data,bytes);
	
	G_I2C_Mem_Write(Motor_model_ADDR,WHEEL_DIA_REG,4,bytes);
}


static control_speed()
{
	static uint8_t speed[8];
	
	speed[0] = (motor_instances[0]->speed>>8)&0xff;
	speed[1] = (motor_instances[0]->speed)&0xff;

	speed[2] = (motor_instances[1]->speed>>8)&0xff;
	speed[3] = (motor_instances[1]->speed)&0xff;

	speed[4] = (motor_instances[2]->speed>>8)&0xff;
	speed[5] = (motor_instances[2]->speed)&0xff;
	
	speed[6] = (motor_instances[3]->speed>>8)&0xff;
	speed[7] = (motor_instances[3]->speed)&0xff;
	

	G_I2C_Mem_Write(Motor_model_ADDR,SPEED_Control_REG,8,speed);
}

static  control_pwm()
{
	static uint8_t pwm[8];
	
	pwm[0] = (motor_instances[0]->pwm>>8)&0xff;
	pwm[1] = (motor_instances[0]->pwm)&0xff;

	pwm[2] = (motor_instances[1]->pwm>>8)&0xff;
	pwm[3] = (motor_instances[1]->pwm)&0xff;

	pwm[4] = (motor_instances[2]->pwm>>8)&0xff;
	pwm[5] = (motor_instances[2]->pwm)&0xff;

	pwm[6] = (motor_instances[3]->pwm>>8)&0xff;
	pwm[7] = (motor_instances[3]->pwm)&0xff;
	
	G_I2C_Mem_Write(Motor_model_ADDR,PWM_Control_REG,8,pwm);
}

static Read_10_Enconder(void)
{
	static int8_t buf[2];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M1Enconer_REG, 2, buf);
	motor_instances[0]->encoder_offset = buf[0]<<8|buf[1];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M2Enconer_REG, 2, buf);
	motor_instances[1]->encoder_offset = buf[0]<<8|buf[1];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M3Enconer_REG, 2, buf);
	motor_instances[2]->encoder_offset = buf[0]<<8|buf[1];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M4Enconer_REG, 2, buf);
	motor_instances[3]->encoder_offset = buf[0]<<8|buf[1];
}


static Read_ALL_Enconder(void)
{
	static uint8_t buf[2];
	static uint8_t buf2[2];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M1_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M1_REG, 2, buf2);
	motor_instances[0]->encoder_now = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1]; 
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M2_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M2_REG, 2, buf2);
	motor_instances[1]->encoder_now = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M3_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M3_REG, 2, buf2);
	motor_instances[2]->encoder_now = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M4_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M4_REG, 2, buf2);
	motor_instances[3]->encoder_now = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
}

void Motor_Start(uint8_t type, uint16_t Pluse_Phase, uint16_t Pluse_Line, float Wheel_Dia)
{
	IIC_Motor_Init();
	DWT_Delay(0.002);
	Set_motor_type(type);
	DWT_Delay(0.002);
	Set_Pluse_Phase(Pluse_Phase);
	DWT_Delay(0.002);
	Set_Pluse_line(Pluse_Line);
	DWT_Delay(0.002);
	Set_Wheel_dis(Wheel_Dia);
	DWT_Delay(0.002);
	Set_motor_deadzone(1500);
	DWT_Delay(0.002);
}

IICMotorInstance* Motor_Init(motor_config *cfg)
{
	IICMotorInstance *motor_instance = (IICMotorInstance *)malloc(sizeof(IICMotorInstance));
	if (motor_instance == NULL) {
		return NULL; // Memory allocation failed
	}
	
	memset(motor_instance, 0, sizeof(IICMotorInstance));
	motor_instance->speed = 0;
	motor_instance->pwm = 0;
	motor_instance->id = cfg->id;
	PIDInit(&motor_instance->pid,&cfg->cfg);
	// Initialize the motor instance
	motor_instances[motor_instance->id] = motor_instance; // Assuming we are initializing the first motor instance
	return motor_instance;
}

void Set_motor_pwm(IICMotorInstance *motor_instance, int16_t pwm)
{
	if (motor_instance == NULL) {
		return; // Invalid motor instance
	}
	motor_instance->pwm = pwm;
	control_pwm();
}

void Set_motor_speed(IICMotorInstance *motor_instance, int16_t speed)
{
	if (motor_instance == NULL) {
		return; // Invalid motor instance
	}
	motor_instance->speed = speed;
	// control_speed();
}

void Get_Motor_Data()
{
	Read_10_Enconder();
	Read_ALL_Enconder();
}

void Motor_control()
{
	// for(uint8_t i = 0; i < 4; i++)
	// {
	// 	motor_instances[i]->pwm = PIDCalculate(&motor_instances[i]->pid,motor_instances[i]->encoder_offset,motor_instances[i]->speed);
	// }
	// control_pwm();
	control_speed();
}