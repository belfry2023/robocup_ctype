#include "bsp_i2c.h"

#define I2C2_SCL_Pin GPIO_PIN_1
#define I2C2_SDA_Pin GPIO_PIN_0
#define I2C2_SCL_GPIO_Port GPIOF
#define I2C2_SDA_GPIO_Port GPIOF

// 定义 I2C ACK 和 NACK
#define I2C_ACK GPIO_PIN_RESET
#define I2C_NACK GPIO_PIN_SET

void I2C_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 初始化SCL引脚
    GPIO_InitStruct.Pin = I2C2_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C2_SCL_GPIO_Port, &GPIO_InitStruct);

    // 初始化SDA引脚
    GPIO_InitStruct.Pin = I2C2_SDA_Pin;
    HAL_GPIO_Init(I2C2_SDA_GPIO_Port, &GPIO_InitStruct);

    // 将SCL和SDA拉高
    HAL_GPIO_WritePin(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin, GPIO_PIN_SET);
}

//输出模式
void I2C_Set_SDA_Output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = I2C2_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C2_SDA_GPIO_Port, &GPIO_InitStruct);
}

// 输入模式
void I2C_Set_SDA_Input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = I2C2_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(I2C2_SDA_GPIO_Port, &GPIO_InitStruct);
}

// 设置SCL线电平
static void I2C_Set_SCL(GPIO_PinState state)
{
    HAL_GPIO_WritePin(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin, state);
}

// 设置SDA线电平
static void I2C_Set_SDA(GPIO_PinState state)
{
    I2C_Set_SDA_Output();  // 设置为输出模式
    HAL_GPIO_WritePin(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin, state);
}

// 读取SDA线电平
static GPIO_PinState I2C_Read_SDA(void)
{
    I2C_Set_SDA_Input();  // 设置为输入模式
    return HAL_GPIO_ReadPin(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);
}

void I2C_Delay(void)
{
    // 延时函数，可以根据时钟频率调整
    for (volatile int i = 0; i < 355; i++)
    {
        __NOP();
    }
}

void I2C_Start(void)
{
	I2C_Set_SDA(GPIO_PIN_SET);
    I2C_Set_SCL(GPIO_PIN_SET);
    I2C_Delay();
    I2C_Set_SDA(GPIO_PIN_RESET);
    I2C_Delay();
    I2C_Set_SCL(GPIO_PIN_RESET);
    I2C_Delay();
}

void I2C_Stop(void)
{
	I2C_Set_SDA(GPIO_PIN_RESET);
    I2C_Set_SCL(GPIO_PIN_SET);
    I2C_Delay();
    I2C_Set_SDA(GPIO_PIN_SET);
    I2C_Delay();
}

// 发送一个字节，返回ACK状态
static HAL_StatusTypeDef I2C_Send_Byte(uint8_t byte)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        GPIO_PinState bit = (byte & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        I2C_Set_SDA(bit);
        I2C_Delay();
        I2C_Set_SCL(GPIO_PIN_SET);
        I2C_Delay();
        I2C_Set_SCL(GPIO_PIN_RESET);
        I2C_Delay();
    }

    // 接收ACK
    I2C_Set_SDA_Input();  // 切换SDA为输入模式
    I2C_Delay();
    I2C_Set_SCL(GPIO_PIN_SET);
    I2C_Delay();
    GPIO_PinState ack = I2C_Read_SDA();  // 读取ACK信号
    I2C_Set_SCL(GPIO_PIN_RESET);
    I2C_Delay();

    return (ack == GPIO_PIN_RESET) ? HAL_OK : HAL_ERROR;
}

// 读取一个字节，并发送ACK或NACK
static uint8_t I2C_Read_Byte(GPIO_PinState ack)
{
    uint8_t byte = 0;

    I2C_Set_SDA_Input();  // 设置SDA为输入模式
    I2C_Delay();

    for (int8_t i = 7; i >= 0; i--)
    {
        I2C_Set_SCL(GPIO_PIN_SET);
        I2C_Delay();
        if (I2C_Read_SDA() == GPIO_PIN_SET)
        {
            byte |= (1 << i);
        }
        I2C_Set_SCL(GPIO_PIN_RESET);
        I2C_Delay();
    }

    // 发送ACK或NACK
    I2C_Set_SDA(ack);  // 设置SDA为输出模式，并发送ACK或NACK
    I2C_Delay();
    I2C_Set_SCL(GPIO_PIN_SET);
    I2C_Delay();
    I2C_Set_SCL(GPIO_PIN_RESET);
    I2C_Delay();

    return byte;
}

// 模拟I2C写入函数，类似HAL_I2C_Mem_Write
HAL_StatusTypeDef G_I2C_Mem_Write(uint8_t devAddr, uint8_t memAddr, uint16_t size, uint8_t *pData)
{
    I2C_Start();

    if (I2C_Send_Byte(devAddr << 1) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    if (I2C_Send_Byte(memAddr) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    for (uint16_t i = 0; i < size; i++)
    {
        if (I2C_Send_Byte(pData[i]) != HAL_OK)
        {
            I2C_Stop();
            return HAL_ERROR;
        }
    }

    I2C_Stop();
    return HAL_OK;
}

// 模拟I2C读取函数，类似HAL_I2C_Mem_Read
HAL_StatusTypeDef G_I2C_Mem_Read(uint8_t devAddr, uint8_t memAddr, uint16_t size, uint8_t *pData)
{
    I2C_Start();

    if (I2C_Send_Byte(devAddr << 1) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    if (I2C_Send_Byte(memAddr) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    I2C_Start();  // 重新启动信号

    if (I2C_Send_Byte((devAddr << 1) | 0x01) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    for (uint16_t i = 0; i < size; i++)
    {
        pData[i] = I2C_Read_Byte((i == size - 1) ? I2C_NACK : I2C_ACK);
    }

    I2C_Stop();
    return HAL_OK;
}

// 模拟I2C写入函数，支持16位寄存器地址
HAL_StatusTypeDef I2C_Mem_Write(uint8_t devAddr, uint16_t memAddr, uint8_t *pData, uint16_t size)
{
    I2C_Start();

    if (I2C_Send_Byte(devAddr << 1) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    // 发送高位地址字节
    if (I2C_Send_Byte((uint8_t)(memAddr >> 8)) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    // 发送低位地址字节
    if (I2C_Send_Byte((uint8_t)(memAddr & 0xFF)) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    // 发送数据
    for (uint16_t i = 0; i < size; i++)
    {
        if (I2C_Send_Byte(pData[i]) != HAL_OK)
        {
            I2C_Stop();
            return HAL_ERROR;
        }
    }

    I2C_Stop();
    return HAL_OK;
}


// 模拟I2C读取函数，支持16位寄存器地址
HAL_StatusTypeDef I2C_Mem_Read(uint8_t devAddr, uint16_t memAddr, uint8_t *pData, uint16_t size)
{
    I2C_Start();

    if (I2C_Send_Byte(devAddr << 1) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    // 发送高位地址字节
    if (I2C_Send_Byte((uint8_t)(memAddr >> 8)) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    // 发送低位地址字节
    if (I2C_Send_Byte((uint8_t)(memAddr & 0xFF)) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    I2C_Start();  // 重新启动信号

    if (I2C_Send_Byte((devAddr << 1) | 0x01) != HAL_OK)
    {
        I2C_Stop();
        return HAL_ERROR;
    }

    // 接收数据
    for (uint16_t i = 0; i < size; i++)
    {
        pData[i] = I2C_Read_Byte((i == size - 1) ? I2C_NACK : I2C_ACK);
    }

    I2C_Stop();
    return HAL_OK;
}