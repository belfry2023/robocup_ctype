#include "app_gimbal.h"
#include "module_servo.h"
#include "message_center.h"
#include "task_def.h"
static servo_t *yaw_servo, *pitch_servo;
static Publisher_t *gimbal_pub;                   // 云台应用消息发布者(云台反馈给cmd)
static Subscriber_t *gimbal_sub;                  // cmd控制消息订阅者
static Gimbal_Upload_Data_s gimbal_feedback_data; // 回传给cmd的云台状态信息
static Gimbal_Ctrl_Cmd_s gimbal_cmd_recv;         // 来自cmd的控制信息
static float angle_test = 60.0f, yaw_buffer, pitch_buffer; // 测试角度
void gimbal_init()
{
    servo_c config_yaw = {
        .angle_pid_config = {
            .Kp = 1, // 4.5
			.Ki = 0,  // 0
			.Kd = 0,  // 0
			.IntegralLimit = 3,
			.MaxOut = 270
        },
        .Channel = TIM_CHANNEL_2,
        .motor_tim_driver = &htim1,
        .angle_max = 270.0f
    };
    yaw_servo = servo_init(&config_yaw);
    servo_c config_pitch = {
        .angle_pid_config = {
            .Kp = 1, // 4.5
			.Ki = 0,  // 0
			.Kd = 0,  // 0
			.IntegralLimit = 3,
			.MaxOut = 180
        },
        .Channel = TIM_CHANNEL_1,
        .motor_tim_driver = &htim1,
        .angle_max = 180.0f
    };
    pitch_servo = servo_init(&config_pitch);

    servo_set_angle(pitch_servo,60);
    servo_set_speed(pitch_servo, 0);
    servo_set_angle(yaw_servo,90);
    servo_set_speed(yaw_servo, 0);
    gimbal_pub = PubRegister("gimbal_feed", sizeof(Gimbal_Upload_Data_s));
    gimbal_sub = SubRegister("gimbal_cmd", sizeof(Gimbal_Ctrl_Cmd_s));
}


static void check()
{
    if(gimbal_cmd_recv.flag_check[NOW][1] == 1)
    {

    }else
    if(gimbal_cmd_recv.flag_check[NOW][1] == 0)
    {
        static uint8_t i, flag_i;
        i++;
        i%=2;
        if(i)
            if(flag_i)
                angle_test++;
            else
                angle_test--;
        if(angle_test >= 180)
        {
            flag_i = 0;
        }
        if(angle_test <= 60)
        {
            flag_i = 1;
        }
    }
}

static void step_run(uint8_t step)
{
    switch (step)
    {
    case 0:
        yaw_servo->angle = gimbal_cmd_recv.offset_data.a1;
        pitch_servo->angle = gimbal_cmd_recv.offset_data.b1;
        break;
    case 1:
        yaw_servo->angle = gimbal_cmd_recv.offset_data.a1;
        pitch_servo->angle = gimbal_cmd_recv.offset_data.b2;
        break;
    case 2:
        yaw_servo->angle = gimbal_cmd_recv.offset_data.a2;
        pitch_servo->angle = gimbal_cmd_recv.offset_data.b2;
        break;
    case 3:
        yaw_servo->angle = gimbal_cmd_recv.offset_data.a2;
        pitch_servo->angle = gimbal_cmd_recv.offset_data.b1;
        break;
    case 4:
        yaw_servo->angle = gimbal_cmd_recv.offset_data.a1;
        pitch_servo->angle = gimbal_cmd_recv.offset_data.b1;
        break;
    default:
        break;
    }
}

void gimbal_task()
{
    SubGetMessage(gimbal_sub, (&gimbal_cmd_recv));
    // if(gimbal_cmd_recv.flag_check[NOW][0] == 1)
    // {
    //     check();
    //     servo_set_angle(pitch_servo, angle_test); // 设置云台俯仰角度
    // }
    // if(gimbal_cmd_recv.flag_check[NOW][0] == 2)
    // {
    //     if(gimbal_cmd_recv.flag_check[NOW][1] == 0)
    //     {
    //         static uint16_t time;
    //         time++;
    //         time%=100;
    //         if(!time)
    //         {
    //             if(gimbal_cmd_recv.flag_check[NOW][1] == 1 && gimbal_cmd_recv.flag_check[NOW][1] == 0)
    //             {
    //                 servo_set_angle(yaw_servo, yaw_buffer);
    //                 servo_set_angle(pitch_servo, pitch_buffer);
    //             }else
    //             {
    //                 static uint8_t step = 0;
    //                 step++;
    //                 step %= 5; // 0-3四个步骤
    //                 step_run(step);
    //             }
    //         }
    //     }else
    //     {
    //         if(gimbal_cmd_recv.flag_check[NOW][1] == 1 && gimbal_cmd_recv.flag_check[NOW][1] == 0)
    //         {
    //             yaw_buffer = yaw_servo->angle;
    //             pitch_buffer = pitch_servo->angle;
    //         }
    //         servo_set_angle(yaw_servo, gimbal_cmd_recv.offset_data.a);
    //         servo_set_angle(pitch_servo, gimbal_cmd_recv.offset_data.b);
    //     }
    // }

    // servo_set_angle(pitch_servo,angle_test);

    servo_set_angle_target(yaw_servo, gimbal_cmd_recv.yaw);
    servo_set_angle_target(pitch_servo, gimbal_cmd_recv.pitch);
    servo_control();
    servo_set_pwm();
    gimbal_feedback_data.pitch = pitch_servo->angle;
    gimbal_feedback_data.yaw = yaw_servo->angle;
    PubPushMessage(gimbal_pub, (void *)&gimbal_feedback_data);
}