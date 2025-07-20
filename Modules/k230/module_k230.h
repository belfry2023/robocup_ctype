#ifndef __MODULE_K230_H
#define __MODULE_K230_H

#include "stdint.h"
#include "usart.h"
// 手势类型定义
typedef enum {
    GESTURE_NONE = 0,
    GESTURE_UP,
    GESTURE_DOWN,
    GESTURE_LEFT,
    GESTURE_RIGHT,
    GESTURE_MIDDLE
} GestureType;

// 颜色识别数据结构
typedef struct {
    int16_t x; // 左上角x坐标
    int16_t y; // 左上角y坐标
    int16_t w; // 宽度
    int16_t h; // 高度
} ColorDetection;

// 条形码/二维码数据结构
typedef struct {
    int16_t x;     // 左上角x坐标
    int16_t y;     // 左上角y坐标
    int16_t w;     // 宽度
    int16_t h;     // 高度
    char msg[64];   // 识别到的字符串
} CodeDetection;

// K230所有数据
typedef struct {
    uint8_t routine_id;  // 例程编号
    
    // 各例程数据
    ColorDetection color_det;    // 颜色识别
    CodeDetection barcode;       // 条形码识别
    CodeDetection qrcode;        // 二维码识别
    GestureType gesture;         // 手势识别
    char ocr_msg[128];           // OCR识别结果
    // 其他例程的数据结构可以在这里添加...
} K230_data_t;

// 函数声明
K230_data_t *K230ProtocolInit(UART_HandleTypeDef *huart);

#endif