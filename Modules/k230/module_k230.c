#include "module_k230.h"
#include "string.h"
#include "bsp_usart.h"
#include "memory.h"
#include "stdlib.h"

#define K230_PROTOCOL_MAX_FRAME_SIZE 128u // 最大帧长度
#define K230_PROTOCOL_TIMEOUT_MS 100      // 100ms未收到数据视为离线


static K230_data_t K230_data;
static uint8_t K230_init_flag = 0; 

// K230拥有的串口实例和守护进程实例
static USARTInstance *K230_usart_instance;

/**
 * @brief 解析K230协议数据
 * @param buf 接收到的数据缓冲区
 * @param len 数据长度
 */
static void ParseK230Protocol(const uint8_t *buf, uint16_t len)
{
    // 检查帧头帧尾
    if (buf[0] != '$' || buf[len-1] != '#') {
        return;
    }

    // 分割数据
    char *token;
    uint8_t temp_buf[K230_PROTOCOL_MAX_FRAME_SIZE];
    memcpy(temp_buf, buf, len);
    temp_buf[len] = '\0'; // 确保字符串结束
    
    // 跳过帧头，解析长度(暂时不使用)
    token = strtok((char *)temp_buf + 1, ",");
    if (token == NULL) return;
    
    // // 
    // token = strtok(NULL, ",");
    // if (token == NULL) return;
    
    // 解析例程编号
    token = strtok(NULL, ",");
    if (token == NULL) return;
    
    uint8_t routine_id = atoi(token);
    K230_data.routine_id = routine_id;
    
    // 根据例程编号解析不同数据
    switch (routine_id) {
        case 1: // 颜色识别
            token = strtok(NULL, ","); // x
            if (token) K230_data.color_det.x = atoi(token);
            token = strtok(NULL, ","); // y
            if (token) K230_data.color_det.y = atoi(token);
            token = strtok(NULL, ","); // w
            if (token) K230_data.color_det.w = atoi(token);
            token = strtok(NULL, ","); // h
            if (token) K230_data.color_det.h = atoi(token);
            break;
            
        case 2: // 条形码识别
            token = strtok(NULL, ","); // x
            if (token) K230_data.barcode.x = atoi(token);
            token = strtok(NULL, ","); // y
            if (token) K230_data.barcode.y = atoi(token);
            token = strtok(NULL, ","); // w
            if (token) K230_data.barcode.w = atoi(token);
            token = strtok(NULL, ","); // h
            if (token) K230_data.barcode.h = atoi(token);
            token = strtok(NULL, ","); // msg
            if (token) strncpy(K230_data.barcode.msg, token, sizeof(K230_data.barcode.msg)-1);
            break;
            
        case 3: // 二维码识别
            token = strtok(NULL, ","); // x
            if (token) K230_data.qrcode.x = atoi(token);
            token = strtok(NULL, ","); // y
            if (token) K230_data.qrcode.y = atoi(token);
            token = strtok(NULL, ","); // w
            if (token) K230_data.qrcode.w = atoi(token);
            token = strtok(NULL, ","); // h
            if (token) K230_data.qrcode.h = atoi(token);
            token = strtok(NULL, ","); // msg
            if (token) strncpy(K230_data.qrcode.msg, token, sizeof(K230_data.qrcode.msg)-1);
            break;
            
        
        case 12: // 手势识别
            token = strtok(NULL, ","); // msg
            if (token) {
                if (strcmp(token, "UP") == 0) K230_data.gesture = GESTURE_UP;
                else if (strcmp(token, "DOWN") == 0) K230_data.gesture = GESTURE_DOWN;
                else if (strcmp(token, "LEFT") == 0) K230_data.gesture = GESTURE_LEFT;
                else if (strcmp(token, "RIGHT") == 0) K230_data.gesture = GESTURE_RIGHT;
                else if (strcmp(token, "MIDDLE") == 0) K230_data.gesture = GESTURE_MIDDLE;
            }
            break;
            
        case 13: // OCR字符识别
            token = strtok(NULL, ","); // msg
            if (token) strncpy(K230_data.ocr_msg, token, sizeof(K230_data.ocr_msg)-1);
            break;
            
        default:
            ;
            break;
    }
}

/**
 * @brief 串口接收回调函数
 */
static void K230RxCallback()
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // 点亮LED指示灯
    // 检查帧头帧尾
    uint8_t *buf = K230_usart_instance->recv_buff;
    uint16_t len = K230_usart_instance->recv_buff_size;
    
    if (len < 5) return; // 最小帧长度检查
    
    // 查找帧头帧尾
    uint8_t *start = memchr(buf, '$', len);
    uint8_t *end = memchr(buf, '#', len);
    
    if (start && end && end > start) {
        uint16_t frame_len = end - start + 1;
        ParseK230Protocol(start, frame_len);
    }
}

/**
 * @brief K230离线的回调函数
 */
static void K230LostCallback(void *id)
{
    memset(&K230_data, 0, sizeof(K230_data)); // 清空数据
    USARTServiceInit(K230_usart_instance);     // 尝试重新启动接收
}

/**
 * @brief 初始化K230通信
 * @param huart 串口句柄
 * @return K230_data_t* 返回K230数据指针
 */
K230_data_t *K230ProtocolInit(UART_HandleTypeDef *huart)
{
    USART_Init_Config_s conf = {
        .module_callback = K230RxCallback,
        .usart_handle = huart,
        .recv_buff_size = K230_PROTOCOL_MAX_FRAME_SIZE,
    };
    K230_usart_instance = USARTRegister(&conf);

    K230_init_flag = 1;
    return &K230_data;
}