#ifndef __I2C_GPIO_H
#define __I2C_GPIO_H

#include "stm32f4xx_hal.h"
#include "stdint.h"

void I2C_GPIO_Init(void);
void I2C_Delay(void);
void I2C_Start(void);
void I2C_Stop(void);
HAL_StatusTypeDef I2C_Mem_Write(uint8_t devAddr, uint16_t memAddr, uint8_t *pData, uint16_t size);
HAL_StatusTypeDef I2C_Mem_Read(uint8_t devAddr, uint16_t memAddr, uint8_t *pData, uint16_t size);

// I2C API函数声明
HAL_StatusTypeDef G_I2C_Mem_Write(uint8_t devAddr, uint8_t memAddr, uint16_t size, uint8_t *pData);
HAL_StatusTypeDef G_I2C_Mem_Read(uint8_t devAddr, uint8_t memAddr, uint16_t size, uint8_t *pData);


#endif /* __I2C_GPIO_H */
