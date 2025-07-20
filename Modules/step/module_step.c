#include "module_step.h"
#include "gpio.h"
#include "bsp_dwt.h"
void step_init()
{

}

void one_step()
{
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_SET);
    DWT_Delay(0.8);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_RESET);
    DWT_Delay(0.8);
}

void step_motor_control(int steps)
{
    if(steps < 0)
    {
        steps = -steps;
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);
        DWT_Delay(0.05);
        for(uint32_t i = 0; i < steps; i++)
        {
            one_step();
        }
    }else{
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
        DWT_Delay(0.05);
        for(uint32_t i = 0; i < steps; i++)
        {
            one_step();
        }
    }
}