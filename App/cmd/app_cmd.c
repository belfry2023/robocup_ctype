#include "app_cmd.h"
#include "module_k230.h"
#include "message_center.h"
#include "task_def.h"
#include "string.h"
#include "module_key.h"
#include "arm_math.h"
static K230_data_t *K230_data;
static Publisher_t *car_cmd_pub;            // 云台控制消息发布者
static Subscriber_t *car_feed_sub;          // 云台反馈信息订阅者
static Publisher_t *gimbal_cmd_pub;            // 云台控制消息发布者
static Subscriber_t *gimbal_feed_sub;          // 云台反馈信息订阅者
static Gimbal_Ctrl_Cmd_s gimbal_cmd_send;      // 传递给云台的控制信息
static Gimbal_Upload_Data_s gimbal_fetch_data; // 从云台获取的反馈信息
static car_state_t car_fetch_data; // 从云台获取的反馈信息
static car_control_t car_cmd_send;      // 传递给云台的控制信息
static K230_data_t buffer;
static KeyInstance key;
static offset_data_t offset_data;
static float yaw_cmd = 90,pitch_cmd = 60;
static float k1 = 0.1,k2 = 0.1;
void cmd_init()
{
    // 初始化K230协议
    K230_data = K230ProtocolInit(&huart1);
    Key_Init(&key);
    car_cmd_pub = PubRegister("car_control", sizeof(car_control_t));
    car_feed_sub = SubRegister("car_state", sizeof(car_state_t));
    gimbal_cmd_pub = PubRegister("gimbal_cmd", sizeof(Gimbal_Ctrl_Cmd_s));
    gimbal_feed_sub = SubRegister("gimbal_feed", sizeof(Gimbal_Upload_Data_s));
    car_cmd_send.begin = 0;
}

//杂项
//2021年
static void updata()
{
    memcpy(&buffer, K230_data, sizeof(K230_data_t));
}

static void clear()
{
    memset(&buffer, 0, sizeof(buffer));
}

static void clear_flag()
{
    if(car_fetch_data.flag == 4)
    {//转弯结束，清除上一个路口的标志位，载入下一个路口
        car_cmd_send.flag = 0;
        K230_data->routine_id = 0; // 清除例程编号
    }
}
//2023年
static void calculate_offset()
{
    float y1 = tan((offset_data.b1 - 60)*PI/180);
    float y2 = y1 - 0.5;
    offset_data.a = 90;
    offset_data.b = (atan(y1 - 0.25))*180/PI + 60;
    offset_data.b2 = (atan(y2))*180/PI + 60;
    offset_data.a1 = (PI/2 + atan(0.25))*180/PI;
    offset_data.a2 = (PI/2 - atan(0.25))*180/PI;

}

static void cal_x_y(float x, float y)
{
    
}


static void upload()
{
    offset_data.b1 = gimbal_fetch_data.pitch;
    calculate_offset();
}

//k230任务区

static void k230task_1()
{
    if(buffer.color_det.x == 11 || buffer.color_det.y == 11)
        {
            car_cmd_send.flag = 1; 
        }
        else if(buffer.color_det.w == 11 || buffer.color_det.h  == 11)
        {
            car_cmd_send.flag = 2;
        }else
        if(buffer.color_det.w == 10 && buffer.color_det.h == 10  && buffer.color_det.x == 10 && buffer.color_det.y == 10)
        {
            car_cmd_send.begin = 1;
        }else
        {
            car_cmd_send.flag = 0;
        }
        clear_flag();
}

static void k230task_2()
{
    
    pitch_cmd = k1 * K230_data->color_det.x + pitch_cmd;
    yaw_cmd = k2 * K230_data->color_det.y + yaw_cmd;
    pitch_cmd = pitch_cmd > 180 ? 1800 : pitch_cmd;
    pitch_cmd = pitch_cmd > 0 ? pitch_cmd : 0;
    yaw_cmd = yaw_cmd > 270 ? 270 : yaw_cmd;
    yaw_cmd = yaw_cmd > 0 ? yaw_cmd : 0;
}

//往年任务

static void task_2021()
{
    updata();
    if(buffer.routine_id == 0)
    {
        
    }else
    {
        switch (buffer.routine_id) 
        {
        case 1:
            k230task_1(); // 处理颜色识别数据
            /* code */
            break;
        case 2:
            k230task_2();
            break;
        default:
            break;
        }
    }
    clear();
}




static void task_2023()
{
    Key_Scan(&key);
    memcpy(&gimbal_cmd_send.flag_check[LAST],&gimbal_cmd_send.flag_check[NOW],sizeof(gimbal_cmd_send.flag_check[NOW]));
    if(key.Key_Long_Press == 1)
    {
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_SET);
        
        gimbal_cmd_send.flag_check[NOW][0] = 1;
        gimbal_cmd_send.flag_check[NOW][1] = 0;
    }else
    if(key.Key_Short_Press == 1)
    {
        gimbal_cmd_send.flag_check[NOW][1]++;
        gimbal_cmd_send.flag_check[NOW][1] %= 2;
        if(gimbal_cmd_send.flag_check[NOW][1] == 1 && gimbal_cmd_send.flag_check[NOW][0] == 1)
        {
            upload();
        }
    }else
    if(key.Key_Repeat_Press == 1)
    {
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_SET);
        DWT_Delay(0.1);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_RESET);
        DWT_Delay(0.1);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_SET);
        DWT_Delay(0.1);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_RESET);
        DWT_Delay(0.1);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,GPIO_PIN_RESET);
        gimbal_cmd_send.flag_check[NOW][0] = 2;
        gimbal_cmd_send.flag_check[NOW][1] = 0;
        gimbal_cmd_send.offset_data = offset_data; // 上传偏移数据
    }
}

//执行区调用往年任务

void cmd_task()
{
    SubGetMessage(car_feed_sub, &car_fetch_data);
    SubGetMessage(gimbal_feed_sub, &gimbal_fetch_data);
    // task_2023(); // 执行任务处理
    k230task_2();
    gimbal_cmd_send.pitch = pitch_cmd;
    gimbal_cmd_send.yaw = yaw_cmd;
    PubPushMessage(car_cmd_pub, &car_cmd_send);
    PubPushMessage(gimbal_cmd_pub, &gimbal_cmd_send);
}