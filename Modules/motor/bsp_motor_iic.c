#include "bsp_motor_iic.h"
#include "bsp_i2c.h"
int Encoder_Offset[4];
int Encoder_Now[4];

  

void float_to_bytes(float f, uint8_t *bytes) 
{
    memcpy(bytes, &f, sizeof(float));
}


float char2float(char *p)
{
  float *p_Int;
  p_Int = (float *)malloc(sizeof(float));
  memcpy(p_Int, p, sizeof(float));
  float x = *p_Int;
  free(p_Int);
  return x;
}

void IIC_Motor_Init(void)
{			
    I2C_GPIO_Init(); // Initialize GPIO for I2C
}

void Set_motor_type(uint8_t data)
{	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_TYPE_REG,2,&data);
}

void Set_motor_deadzone(uint16_t data)
{
	static uint8_t buf_tempzone[2];
	
	buf_tempzone[0] = (data>>8)&0xff;
	buf_tempzone[1] = data;
	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_DeadZONE_REG,2,buf_tempzone);
}


void Set_Pluse_line(uint16_t data)
{
	static uint8_t buf_templine[2];
	
	buf_templine[0] = (data>>8)&0xff;
	buf_templine[1] = data;
	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_PluseLine_REG,2,buf_templine);
}


void Set_Pluse_Phase(uint16_t data)
{
	static uint8_t buf_tempPhase[2];
	
	buf_tempPhase[0] = (data>>8)&0xff;
	buf_tempPhase[1] = data;
	
	G_I2C_Mem_Write(Motor_model_ADDR,MOTOR_PlusePhase_REG,2,buf_tempPhase);
}


void Set_Wheel_dis(float data)
{
	static uint8_t bytes[4];
	
	float_to_bytes(data,bytes);
	
	G_I2C_Mem_Write(Motor_model_ADDR,WHEEL_DIA_REG,4,bytes);
}


void control_speed(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4)
{
	static uint8_t speed[8];
	
	speed[0] = (m1>>8)&0xff;
	speed[1] = (m1)&0xff;
	
	speed[2] = (m2>>8)&0xff;
	speed[3] = (m2)&0xff;
	
	speed[4] = (m3>>8)&0xff;
	speed[5] = (m3)&0xff;
	
	speed[6] = (m4>>8)&0xff;
	speed[7] = (m4)&0xff;
	
	G_I2C_Mem_Write(Motor_model_ADDR,SPEED_Control_REG,8,speed);

}

void control_pwm(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4)
{
	static uint8_t pwm[8];
	
	pwm[0] = (m1>>8)&0xff;
	pwm[1] = (m1)&0xff;
	
	pwm[2] = (m2>>8)&0xff;
	pwm[3] = (m2)&0xff;
	
	pwm[4] = (m3>>8)&0xff;
	pwm[5] = (m3)&0xff;
	
	pwm[6] = (m4>>8)&0xff;
	pwm[7] = (m4)&0xff;
	
	G_I2C_Mem_Write(Motor_model_ADDR,PWM_Control_REG,8,pwm);

}

void Read_10_Enconder(void)
{
	static int8_t buf[2];
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M1Enconer_REG, 2, buf);
	Encoder_Offset[0] = buf[0]<<8|buf[1]; 
	
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M2Enconer_REG, 2, buf);
	Encoder_Offset[1] = buf[0]<<8|buf[1];
	
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M3Enconer_REG, 2, buf);
	Encoder_Offset[2] = buf[0]<<8|buf[1];
	
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_TEN_M4Enconer_REG, 2, buf);
	Encoder_Offset[3] = buf[0]<<8|buf[1];
	
}


void Read_ALL_Enconder(void)
{
	static uint8_t buf[2];
	static uint8_t buf2[2];
	
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M1_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M1_REG, 2, buf2);
	Encoder_Now[0] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1]; 
	
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M2_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M2_REG, 2, buf2);
	Encoder_Now[1] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M3_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M3_REG, 2, buf2);
	Encoder_Now[2] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLHigh_M4_REG, 2, buf);
	G_I2C_Mem_Read(Motor_model_ADDR, READ_ALLLOW_M4_REG, 2, buf2);
	Encoder_Now[3] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
}
