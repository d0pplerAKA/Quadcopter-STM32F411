#include "fwb.h"


DRONE_ATTITUDE_t drone_attitude;


//FWB_LADRC_t ladrc_roll;
//FWB_LADRC_t ladrc_pitch;
//FWB_LADRC_t ladrc_yaw;


FWB_PID_t pid_roll_degree;
FWB_PID_t pid_pitch_degree;
FWB_PID_t pid_roll_gyro;
FWB_PID_t pid_pitch_gyro;
FWB_PID_t pid_yaw;

FWB_PID_t pid_flow_x;
FWB_PID_t pid_flow_y;


FWB_LPF_1P_t lpf_roll;
//FWB_LPF_2P_t lpf2_roll;
FWB_LPF_1P_t lpf_pitch;
//FWB_LPF_2P_t lpf2_pitch;
FWB_LPF_1P_t lpf_yaw;

FWB_LPF_1P_t lpf_roll_degree;
FWB_LPF_1P_t lpf_pitch_degree;


float LPF_FIFO_ROLL[FWB_LFP_BUFFER];
float LPF_FIFO_PITCH[FWB_LFP_BUFFER];



/*
    LADRC参数整定方法
    1.设置一个较小的b0，使系统运行，逐渐增大b0，直到控制效果相对稳定。

    2.设置较小的wc，较大的w0。

    2.逐渐增大wc，期间如果系统不稳定，观察系统是否发生抖动，则重新增大b0。
*/

/*
void FWB_LADRC_Init(FWB_LADRC_t * ladrc)
{
    ladrc->h = 1.0f / (float) FWB_SAMPLING_RATE;
    ladrc->r = 50;

    ladrc->w0 = 50;
    ladrc->wc = 80;
    ladrc->b0 = 5;

    ladrc->z1 = 0;
    ladrc->z2 = 0;
    ladrc->z3 = 0;

    ladrc->v1 = 0;
    ladrc->v2 = 0;

    ladrc->bt_1 = ladrc->w0 * 3;
    ladrc->bt_2 = powf(ladrc->w0, 2) * 3;
    ladrc->bt_3 = powf(ladrc->w0, 3);

    ladrc->kp = powf(ladrc->wc, 2);
    ladrc->kd = ladrc->wc * 2;

    ladrc->u = 0;

    ladrc->u_max = FWB_LADRC_U_MAX;
    ladrc->u_min = FWB_LADRC_U_MIN;
}
*/

/*
void FWB_LADRC_Reset(FWB_LADRC_t * ladrc)
{
    ladrc->z1 = 0.0f;
    ladrc->z2 = 0.0f;
    ladrc->z3 = 0.0f;
}
*/

/*
void FWB_LADRC_TD(FWB_LADRC_t * ladrc, float target)             // TD 跟踪器
{
    float fh = powf(ladrc->r, 2) * (ladrc->v1 - target) - 2 * ladrc->r * ladrc->v2;

    ladrc->v1 += ladrc->v2 * ladrc->h;
    ladrc->v2 += fh * ladrc->h;
}
*/

/*
void FWB_LADRC_ESO(FWB_LADRC_t * ladrc, float rt_val)            // ESO 观测器
{
    if(rt_val < 0.325f * FWB_deg2rad && rt_val > -0.325f * FWB_deg2rad)
    {
        FWB_LADRC_Reset(ladrc);
    }
    else
    {
        float error = ladrc->z1 - rt_val;

        ladrc->z1 += (ladrc->z2 - ladrc->bt_1 * error) * ladrc->h;
        ladrc->z2 += (ladrc->z3 - ladrc->bt_2 * error + ladrc->b0 * ladrc->u) * ladrc->h;
        ladrc->z3 += -ladrc->bt_3 * error * ladrc->h;
    }
}
*/


/*
void FWB_LADRC_LC(FWB_LADRC_t * ladrc)                           // 线性控制      
{
    float e1 = ladrc->v1 - ladrc->z1;
    float e2 = ladrc->v2 - ladrc->z2;

    float u0 = ladrc->kp * e1 + ladrc->kd * e2;

    ladrc->u = (u0 - ladrc->z3) / ladrc->b0;

    if(ladrc->u > ladrc->u_max)
        ladrc->u = ladrc->u_max;
    else if(ladrc->u < ladrc-> u_min)
        ladrc->u = ladrc->u_min;
    else
        ladrc->u = ladrc->u;
}
*/


/*
* @param * ladrc:           struct instance address
* @param target_value:      target degree/radius value address for the drone (read only)
* @param reference_value:   value of current degree/radius
* @param * output_value:    final output pwm value (write only)
*/

/*
void FWB_LADRC_onLoop(FWB_LADRC_t * ladrc, float target_value, float reference_value)
{
    FWB_LADRC_TD(ladrc, target_value);
    FWB_LADRC_ESO(ladrc, reference_value);
    FWB_LADRC_LC(ladrc);
}
*/




/**************************************************************************************************/



void FWB_PID_Init(float kp, float ki, float kd, float dt, FWB_PID_t * pid)
{
    pid->dt = dt;

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->err = 0.0f;
    pid->last_err = 0.0f;

    pid->p_value = 0.0f;
    pid->i_value = 0.0f;
    pid->d_value = 0.0f;

    pid->output = 0.0f;
}

void FWB_PID_onLoop(FWB_PID_t * pid, float target_val, float actual_val, float offset, uint8_t i_On)
{
    pid->err = target_val - actual_val;

    if(i_On)
    {
        if(pid->err <= offset && pid->err >= -offset)
        {
            pid->err /= 2;
            pid->i_value = 0.0f;
        }
        else
        {
            pid->i_value += pid->err * pid->dt;

            if(pid->i_value > FWB_PID_I_Value_Max)          pid->i_value = FWB_PID_I_Value_Max;
            else if(pid->i_value < -FWB_PID_I_Value_Max)    pid->i_value = -FWB_PID_I_Value_Max;
            else                                            pid->i_value = pid->i_value;

            pid->i_value *= pid->ki;
        }
    }
    else pid->i_value = 0.0f;

    pid->p_value = pid->kp * pid->err;
    pid->d_value = pid->kd * (pid->err - pid->last_err) / pid->dt;

    pid->output = pid->p_value + pid->i_value + pid->d_value;

    if(pid->output > FWB_PID_MAX)               pid->output = FWB_PID_MAX;
    else if(pid->output < FWB_PID_MIN)          pid->output = FWB_PID_MIN;
    else pid->output = pid->output;

    pid->last_err = pid->err;
}




/**************************************************************************************************/

void Drone_attitude_Init(DRONE_ATTITUDE_t * drone)
{
    drone->desired_roll = 0.0f;
    drone->desired_pitch = 0.0f;
    drone->desired_yaw = 0.0f;
}

void Drone_Change_TargetAttitude(DRONE_ATTITUDE_t * att)
{
    float temp_roll_degree, temp_pitch_degree;

    temp_roll_degree = Drone_weight_factor_2nd_linear(nrf_joy_stick.JoyRightH);
    temp_pitch_degree = -Drone_weight_factor_2nd_linear(nrf_joy_stick.JoyRightV);
    //printf("joy: %.4f roll deg: %.2f roll: %.2f\n", nrf_joy_stick.JoyRightH, temp_roll_degree, GY_Roll());
    
    att->desired_roll = temp_roll_degree;
    att->desired_pitch = temp_pitch_degree;
}


/**************************************************************************************************/

float Drone_weight_factor_2nd_linear(float x)
{
    float rtn;

    if(x >= 0.065f)
        rtn = -3.432f * powf((x - 0.065f), 2.0f);
    else if(x <= -0.065f)
        rtn = 3.432f * powf((x + 0.065f), 2.0f);
    else rtn = 0.0f;

    return rtn;
}

void Drone_throttle_factor(float x, volatile float * throttle)
{
    float x_val = x;
    float temp_throttle = * throttle;

    if(x > 0.075f) x_val -= 0.075f;
    else if(x < -0.075f) x_val += 0.075f;
    else x_val = 0.0f;

    if(x_val > 0.0f) temp_throttle += FWB_THROTTLE_BASE_Up * x_val;
    else if(x_val < 0.0f) temp_throttle += FWB_THROTTLE_BASE_Down * x_val;
    else temp_throttle -= 0.0125f;

    if(temp_throttle < -BLDC_BaseSpeed) temp_throttle = -BLDC_BaseSpeed;
    if(temp_throttle > 680.0f) temp_throttle = 680.0f;

    *throttle = temp_throttle;
}

void Drone_Yaw_adjusting(float trig_left, float trig_right, float * offset)
{
    float temp_offset = (trig_left - trig_right) * 4.35f;

    * offset = temp_offset;
}

void FWB_Init(void)
{
    // Roll
    FWB_PID_Init(6.725f, 0.0f, 0.0f, 1.0f / (float) FWB_SAMPLING_RATE, &pid_roll_degree);
	FWB_PID_Init(flash_pid_float.kp_roll, flash_pid_float.ki_roll, flash_pid_float.kd_roll, 
                    1.0f / (float) FWB_SAMPLING_RATE, &pid_roll_gyro);
    // Pitch
	FWB_PID_Init(6.725f, 0.0f, 0.0f, 1.0f / (float) FWB_SAMPLING_RATE, &pid_pitch_degree);
	FWB_PID_Init(flash_pid_float.kp_pitch, flash_pid_float.ki_pitch, flash_pid_float.kd_pitch, 
                    1.0f / (float) FWB_SAMPLING_RATE, &pid_pitch_gyro);

	FWB_PID_Init(flash_pid_float.kp_yaw, 0.0f, flash_pid_float.kd_yaw, 
                    1.0f / (float) FWB_SAMPLING_RATE, &pid_yaw);

    // GL9306
    FWB_PID_Init(2.235f, 0.185f, 0.0f, 1.0f / (float) GL9306_Sampling_Time, &pid_flow_x);
    FWB_PID_Init(2.235f, 0.185f, 0.0f, 1.0f / (float) GL9306_Sampling_Time, &pid_flow_y);

	Drone_attitude_Init(&drone_attitude);


    FWB_LPF_Init(&lpf_roll);
    FWB_LPF_Init(&lpf_pitch);
    FWB_LPF_Init(&lpf_yaw);

    FWB_LPF_Init(&lpf_roll_degree);
    FWB_LPF_Init(&lpf_pitch_degree);
}

/**************************************************************************************************/


void FWB_LPF_Init(FWB_LPF_1P_t * lpf)
{
    lpf->rc =  (float) ((float) 1.0f / (2.0f * FWB_PI * FWB_CUTOFF_FREQ));

    lpf->alpha = constrain_float(FWB_SAMPLING_RATE_DT / (FWB_SAMPLING_RATE_DT + lpf->rc), 0.0f, 1.0f);

    lpf->output = 0.0f;

    memset(LPF_FIFO_ROLL, 0, sizeof(float) * FWB_LFP_BUFFER);
    memset(LPF_FIFO_PITCH, 0, sizeof(float) * FWB_LFP_BUFFER);
}

void FWB_LPF_onLoop(FWB_LPF_1P_t * lpf, float input)
{
    lpf->output += (input - lpf->output) * lpf->alpha;
}

void FWB_LPF2_Init(FWB_LPF_2P_t * lpf2)
{
    lpf2->fr = (float) FWB_SAMPLING_RATE / (float) FWB_CUTOFF_FREQ;
    lpf2->ohm = tanf(FWB_PI / lpf2->fr);
    lpf2->c = 1.0f + 2.0f * cosf(FWB_PI / 4.0f) * lpf2->ohm + lpf2->ohm * lpf2->ohm;

    lpf2->_b0 = lpf2->ohm * lpf2->ohm / lpf2->c;
    lpf2->_b1 = 2.0f * lpf2->_b0;
    lpf2->_b2 = lpf2->_b0;

    lpf2->_a0 = 2.0f * (lpf2->ohm * lpf2->ohm - 1.0f) / lpf2->c;
    lpf2->_a1 = (1.0f - 2.0f * cosf(FWB_PI / 4.0f) * lpf2->ohm + lpf2->ohm * lpf2->ohm) / lpf2->c;

    lpf2->_d0 = 0.0f;
    lpf2->_d1 = 0.0f;

    lpf2->output = 0.0f;
}

void FWB_LPF2_onLoop(FWB_LPF_2P_t * lpf2, float input)
{
    float temp_input = input - lpf2->_d0 * lpf2->_a0 - lpf2->_d1 * lpf2->_a1;

    if(!isfinite(temp_input))    temp_input = input;

    lpf2->output = temp_input * lpf2->_b0 + lpf2->_d0 * lpf2->_b1 + lpf2->_d1 * lpf2->_b2;

    lpf2->_d1 = lpf2->_d0;
    lpf2->_d0 = temp_input;
}


/**************************************************************************************************/


float constrain_float(float input, float min, float max)
{
    if(input < min)
        return min;
    else if(input > max)
        return max;
    else return input;
}
