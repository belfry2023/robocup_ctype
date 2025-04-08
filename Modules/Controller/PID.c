#include "PID.h"
#include "string.h"

static void f_Trapezoid_Intergral(PIDInstance *pid)
{
    
    pid->ITerm = pid->Ki * ((pid->Err + pid->Last_Err) / 2) * pid->dt;
}


static void f_Changing_Integration_Rate(PIDInstance *pid)
{
    if (pid->Err * pid->Iout > 0)
    {
       
        if (abs(pid->Err) <= pid->CoefB)
            return; // Full integral
        if (abs(pid->Err) <= (pid->CoefA + pid->CoefB))
            pid->ITerm *= (pid->CoefA - abs(pid->Err) + pid->CoefB) / pid->CoefA;
        else 
            pid->ITerm = 0;
    }
}

static void f_Integral_Limit(PIDInstance *pid)
{
    static float temp_Output, temp_Iout;
    temp_Iout = pid->Iout + pid->ITerm;
    temp_Output = pid->Pout + pid->Iout + pid->Dout;
    if (abs(temp_Output) > pid->MaxOut)
    {
        if (pid->Err * pid->Iout > 0) 
        {
            pid->ITerm = 0; 
        }
    }

    if (temp_Iout > pid->IntegralLimit)
    {
        pid->ITerm = 0;
        pid->Iout = pid->IntegralLimit;
    }
    if (temp_Iout < -pid->IntegralLimit)
    {
        pid->ITerm = 0;
        pid->Iout = -pid->IntegralLimit;
    }
}

/// @brief 
/// @param pid 
static void f_Derivative_On_Measurement(PIDInstance *pid)
{
    pid->Dout = pid->Kd * (pid->Last_Measure - pid->Measure) / pid->dt;
}

/// @brief 
/// @param pid 
static void f_Derivative_Filter(PIDInstance *pid)
{
    pid->Dout = pid->Dout * pid->dt / (pid->Derivative_LPF_RC + pid->dt) +
                pid->Last_Dout * pid->Derivative_LPF_RC / (pid->Derivative_LPF_RC + pid->dt);
}

/// @brief 
/// @param pid 
static void f_Output_Filter(PIDInstance *pid)
{
    pid->Output = pid->Output * pid->dt / (pid->Output_LPF_RC + pid->dt) +
                  pid->Last_Output * pid->Output_LPF_RC / (pid->Output_LPF_RC + pid->dt);
}

/// @brief 
/// @param pid 
static void f_Output_Limit(PIDInstance *pid)
{
    if (pid->Output > pid->MaxOut)
    {
        pid->Output = pid->MaxOut;
    }
    if (pid->Output < -(pid->MaxOut))
    {
        pid->Output = -(pid->MaxOut);
    }
}



/// @brief 
/// @param pid 
/// @param config 
void PIDInit(PIDInstance *pid, PID_Init_Config_s *config)
{
    
    memset(pid, 0, sizeof(PIDInstance));
    // utilize the quality of struct that its memeory is continuous
    memcpy(pid, config, sizeof(PID_Init_Config_s));
    // set rest of memory to 0
    DWT_GetDeltaT(&pid->DWT_CNT);
}

/// @brief 
/// @param pid 
/// @param measure 
/// @param ref 
/// @return 
float PIDCalculate(PIDInstance *pid, float measure, float ref)
{
   

    pid->dt = DWT_GetDeltaT(&pid->DWT_CNT); 

    
    pid->Measure = measure;
    pid->Ref = ref;
    pid->Err = pid->Ref - pid->Measure;

   
    if (abs(pid->Err) > pid->DeadBand)
    {
       
        pid->Pout = pid->Kp * pid->Err;
        pid->ITerm = pid->Ki * pid->Err * pid->dt;
        pid->Dout = pid->Kd * (pid->Err - pid->Last_Err) / pid->dt;

       
				f_Trapezoid_Intergral(pid);
				
				f_Changing_Integration_Rate(pid);
				
				f_Derivative_On_Measurement(pid);
        
        f_Derivative_Filter(pid);
       
        f_Integral_Limit(pid);
        pid->Iout += pid->ITerm;                         
        pid->Output = pid->Pout + pid->Iout + pid->Dout; 

       
        f_Output_Filter(pid);

        
        f_Output_Limit(pid);
    }
    else 
    {
        pid->Output = 0;
        pid->ITerm = 0;
    }

    
    pid->Last_Measure = pid->Measure;
    pid->Last_Output = pid->Output;
    pid->Last_Dout = pid->Dout;
    pid->Last_Err = pid->Err;
    pid->Last_ITerm = pid->ITerm;

    return pid->Output;
}