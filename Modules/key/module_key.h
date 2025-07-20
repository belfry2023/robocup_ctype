#ifndef __MODULE_KEY_H
#define __MODULE_KEY_H

#include "gpio.h"
#include "bsp_dwt.h"


typedef struct {
    uint8_t Key_Long_Press;
    uint8_t Key_Short_Press;
    uint8_t Key_Repeat_Press; // 重复按下标志
    float dt;
    float dt_last; // 上一次按键扫描的时间间隔
} KeyInstance;

void Key_Init(KeyInstance *key);
void Key_Scan(KeyInstance *key);
#endif