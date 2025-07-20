#ifndef TASK_DEF_H
#define TASK_DEF_H

#include "stdint.h"

#define NOW 0
#define LAST 1

#pragma pack(1)

typedef struct{
    float a1;
    float a2;
    float b1;
    float b2;
    float a;
    float b;
}offset_data_t;

typedef struct car_control_s
{
    uint8_t flag;
    uint8_t begin;
} car_control_t;

typedef struct
{ // 云台角度控制
    offset_data_t offset_data; // 云台偏移数据
    float yaw; // 云台偏航角度
    float pitch; // 云台俯仰角度
    uint8_t flag_check[2][2]; 
} Gimbal_Ctrl_Cmd_s;

typedef struct
{
    float yaw; // 云台偏航角度
    float pitch;
} Gimbal_Upload_Data_s;

typedef struct car_state_s
{
    uint8_t flag;
    uint8_t begin;
} car_state_t;

#pragma pack()



#endif // TASK_DEF_H