#include "cmd.h"
#include "master_process.h"
#include "message_center.h"
#include "bsp_dwt.h"
#include "Car_def.h"
#include "can_comm.h"
#include "remote.h"
#include "ins_task.h"
#include "PID.h"
static attitude_t *gimba_IMU_data; // 云台IMU数据
static RC_ctrl_t *rc_data;
static Publisher_t *cmd_pub;
static Car_Ctrl_Cmd_s car_cmd;
static Nav_Recv_s *nav_recv_data; // 视觉接收数据指针,初始化时返回
static PIDInstance pid;
static float yaw_ref;
void RobotCMDInit()
{
    gimba_IMU_data = INS_Init(); // IMU先初始化,获取姿态数
    rc_data = RemoteControlInit(&huart3);
    nav_recv_data = NavInit(&huart6); // 导航通信串口
    PID_Init_Config_s cfg = {
        .Kp = 10,
        .Ki = 0,
        .Kd = 5,
        .DeadBand = 0.1,
        .MaxOut = 660,
        .Derivative_LPF_RC = 0.001,
        .IntegralLimit = 100
    };
    PIDInit(&pid,&cfg);
    cmd_pub = PubRegister("Car_cmd", sizeof(Car_Ctrl_Cmd_s));
}


static void RemoteControlSet()
{
    if (switch_is_down(rc_data[TEMP].rc.switch_left)||switch_is_up(rc_data[TEMP].rc.switch_left)) // 左侧开关状态[下][上],底盘和云台分离,底盘保持不转动
    {
        nav_recv_data->vx = 0;
        nav_recv_data->vy = 0;
        nav_recv_data->wz = 0;

        car_cmd.vx = (rc_data->rc.rocker_l1);
        yaw_ref += (rc_data->rc.rocker_l_)/66/8;
        car_cmd.wz = -PIDCalculate(&pid,gimba_IMU_data->YawTotalAngle,yaw_ref);
    }
    else
    {
        car_cmd.vx = 660 * nav_recv_data->vx; // _水平方向
        car_cmd.vy = 660 * nav_recv_data->vy; // 1数值方向
        car_cmd.wz = 660 * nav_recv_data->wz;
    }
}




static void EmergencyHandler()
{
    if(switch_is_down(rc_data[TEMP].rc.switch_right)) // 右侧开关状态[下],急停
    {
        car_cmd.vx = 0;
        car_cmd.vy = 0;
        car_cmd.wz = 0;
    }
}

/* 机器人核心控制任务,200Hz频率运行(必须高于视觉发送频率) */
void RobotCMDTask()
{
    RemoteControlSet();
    EmergencyHandler(); // 处理模块离线和遥控器急停等紧急情况
    PubPushMessage(cmd_pub, &car_cmd);
}
