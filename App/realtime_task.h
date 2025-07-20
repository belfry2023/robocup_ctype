#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "module_motor.h"
#include "bsp_log.h"
#include "task_init.h"
osThreadId testTaskHandle;


void StartTESTTASK(void const *argument);

void OSTaskInit()
{
    osThreadDef(testtask, StartTESTTASK, osPriorityAboveNormal, 0, 1024);
    testTaskHandle = osThreadCreate(osThread(testtask), NULL);
    
    
}

__attribute__((noreturn)) void StartTESTTASK(void const *argument)
{
    for (;;)
    {
        task_run();
        osDelay(10);
    }
}
