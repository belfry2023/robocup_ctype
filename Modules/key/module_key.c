#include "module_key.h"
#include "gpio.h"
#include "bsp_dwt.h"
#include "string.h"

void Key_Init(KeyInstance *key)
{
    memset(key, 0, sizeof(KeyInstance)); // 初始化结构体
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // 设置GPIO引脚初始状态
}

void Key_Scan(KeyInstance *key)
{

    key->Key_Long_Press = 0; // 重置长按标志
    key->Key_Short_Press = 0; // 重置短按标志
    key->Key_Repeat_Press = 0; // 重置重复按下标志
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
        DWT_Delay(0.01); // 防抖延时
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
            key->dt = 0.01; // 设置按下时间间隔
            while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET) {
                key->dt += 0.01; // 按键保持按下状态，累加时间
                DWT_Delay(0.01); // 防抖延时
            }
            if (key->dt >= 1.0f) { // 长按超过1秒
                key->Key_Long_Press = 1;
            } else { // 短按
                key->Key_Short_Press = 1;
            }
            if(key->dt_last < 80)
            {
                key->Key_Repeat_Press = 1;
                key->Key_Long_Press = 0; // 重复按下不设置长按标志
                key->Key_Short_Press = 0; // 重复按下不设置短按标志
            }
        }
        key->dt_last = 0; // 重置上一次按键扫描的时间间隔
    }else
    {
        key->dt_last++;
        if(key->dt_last > 1000) // 如果超过1秒没有按下，重置时间间隔
        {
            key->dt_last = 100;
        }
    }
}