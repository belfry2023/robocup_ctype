#ifndef __BALANCE_TASK_H
#define __BALANCE_TASK_H


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "motor.h"
#include "car.h"
#include "bsp_dwt.h"
#include "ins_task.h"
#include "tim.h"
osThreadId insTaskHandle;
osThreadId motorTaskHandle;
osThreadId daemonTaskHandle;

void StartINSTASK(void const *argument);
void StartMOTORTASK(void const *argument);
void StartDAEMONTASK(void const *argument);

/**
 * @brief ³õÊ¼»¯»úÆ÷ÈËÈÎÎñ,ËùÓÐ³ÖÐøÔËÐÐµÄÈÎÎñ¶¼ÔÚÕâÀï³õÊ¼»¯
 *
 */
void OSTaskInit()
{
    osThreadDef(instask, StartINSTASK, osPriorityAboveNormal, 0, 1024);
    insTaskHandle = osThreadCreate(osThread(instask), NULL);

	osThreadDef(motortask, StartMOTORTASK, osPriorityNormal, 0, 256);
    motorTaskHandle = osThreadCreate(osThread(motortask), NULL); 
	
    osThreadDef(daemontask, StartDAEMONTASK, osPriorityNormal, 0, 128);
    daemonTaskHandle = osThreadCreate(osThread(daemontask), NULL);
}

__attribute__((noreturn)) void StartINSTASK(void const *argument)
{
    INS_Init(); // 确保BMI088被正确初始化.
    for (;;)
    {
        INS_Task();
        osDelay(1);
    }
}

__attribute__((noreturn)) void StartMOTORTASK(void const *argument)
{

    for (;;)
    {
		RobotTask();
        osDelay(10);
    }
}

__attribute__((noreturn)) void StartDAEMONTASK(void const *argument)
{
    
    
    //LOGINFO("[freeRTOS] Daemon Task Start");
    for (;;)
    {
        DaemonTask();
        osDelay(10);
    }
}

#endif

