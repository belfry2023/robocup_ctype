#include "init.h"
#include "bsp_dwt.h"
#include "Car_Task.h"
#include "cmd.h"
#include "car.h"

void V_Init()
{  
    __disable_irq();
    
    DWT_Init(168);
    RobotCMDInit();
	car_init();
    OSTaskInit();

    __enable_irq();
}

void RobotTask()
{
    RobotCMDTask();
    car_task();
	motor_control();
}