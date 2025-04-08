#ifndef _PID_H
#define _PID_H

#include "main.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "bsp_dwt.h"
#include "math.h"
#include <math.h>

#ifndef abs
#define abs(x) ((x > 0) ? x : -x)
#endif

typedef struct PID_Typedef
{
		float Kp;
    float Ki;
    float Kd;
    float MaxOut;
    float DeadBand;
		float IntegralLimit;     
    float CoefA;             
    float CoefB;             
    float Output_LPF_RC;     
    float Derivative_LPF_RC; 
		float Measure;
    float Last_Measure;
    float Err;
    float Last_Err;
    float Last_ITerm;

    float Pout;
    float Iout;
    float Dout;
    float ITerm;

    float Output;
    float Last_Output;
    float Last_Dout;

    float Ref;

    uint32_t DWT_CNT;
    float dt;
}PIDInstance;


typedef struct // config parameter
{
    // basic parameter
    float Kp;
    float Ki;
    float Kd;
    float MaxOut;   
    float DeadBand; 
    
    // improve parameter
    float IntegralLimit; 
    float CoefA;         
    float CoefB;         
    float Output_LPF_RC; 
    float Derivative_LPF_RC;
} PID_Init_Config_s;

void PIDInit(PIDInstance *pid, PID_Init_Config_s *config);
float PIDCalculate(PIDInstance *pid, float measure, float ref);

#endif