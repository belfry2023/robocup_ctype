#include "task_init.h"
#include "app_chassis.h"
#include "app_gimbal.h"
#include "app_cmd.h"
#include "realtime_task.h"
#include "bsp_log.h"
#include "bsp_dwt.h"

void task_init(void)
{
    //DWT_Init(168);
    //BSPLogInit();
    // 初始化云台
    __disable_irq();
    
    DWT_Init(168);
    BSPLogInit();

    cmd_init();

    gimbal_init();

    // 初始化底盘
    chassis_init();

    // 其他任务初始化可以在这里添加
    OSTaskInit(); // 如果需要使用FreeRTOS任务调度器，可以取消注释
    __enable_irq();
    
}


void task_run(void)
{
    cmd_task(); // 执行命令处理任务
    gimbal_task(); // 执行云台任务
    chassis_task(); // 执行底盘任务
}