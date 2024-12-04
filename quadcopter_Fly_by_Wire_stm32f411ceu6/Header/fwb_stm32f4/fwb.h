#ifndef __FWB_H__
#define __FWB_H__

#include "stm32f4xx.h"
#include "arm_math.h"

#include "math.h"


#include "bl_motor/bl_motor.h"
#include "xbox/xbox.h"


#define FWB_SAMPLING_RATE               200
#define FWB_SAMPLING_RATE_DT            ((float) (1.0f / (float)FWB_SAMPLING_RATE))
#define FWB_CUTOFF_FREQ                 35
#define FWB_PI                          ((float) acosf(-1.0f))
#define FWB_LFP_BUFFER                  4

#define FWB_deg2rad                     0.017453f
#define FWB_rad2deg                     57.29576f
#define FWB_e                           2.718282f

#define FWB_LADRC_U_MAX                 ((float) 100.0f)
#define FWB_LADRC_U_MIN                 ((float) -100.0f)

#define FWB_PID_MAX                     ((float) 85.0f)
#define FWB_PID_MIN                     ((float) -85.0f)

#define FWB_PID_I_Value_Max             ((float) 75.0f)


#define FWB_THROTTLE_MAX                ((float) 680.0f)
#define FWB_THROTTLE_BASE_Up            ((float) 0.85f)
#define FWB_THROTTLE_BASE_Down          ((float) 0.75f)


typedef struct
{
    float v1, v2;
    float r;
    float h;
    float z1, z2, z3;
    float w0, wc, b0, u;

    float bt_1, bt_2, bt_3;

    float kp, kd;

    float u_max;
    float u_min;
    /* data */
} FWB_LADRC_t;


typedef struct
{
    float dt;
    float kp, ki, kd;

    float err, last_err;

    float p_value;
    float i_value;
    float d_value;

    float output;
    /* data */
} FWB_PID_t;


typedef struct
{
    float desired_roll;
    float desired_pitch;
    float desired_yaw;
    /* data */
} DRONE_ATTITUDE_t;

typedef struct
{
    float rc;
    float alpha;

    float output;

    /* data */
} FWB_LPF_1P_t;

typedef struct
{
    float _b0;
    float _b1;
    float _b2;

    float _a0;
    float _a1;

    float fr;
    float ohm;
    float c;

    float _d0;
    float _d1;

    float output;
    /* data */
} FWB_LPF_2P_t;



//extern FWB_LADRC_t ladrc_roll;
//extern FWB_LADRC_t ladrc_pitch;
//extern FWB_LADRC_t ladrc_yaw;


extern FWB_PID_t pid_roll_degree;
extern FWB_PID_t pid_pitch_degree;

extern FWB_PID_t pid_roll_gyro;
extern FWB_PID_t pid_pitch_gyro;

extern FWB_PID_t pid_yaw;

extern FWB_PID_t pid_flow_x;
extern FWB_PID_t pid_flow_y;

extern DRONE_ATTITUDE_t drone_attitude;

extern FWB_LPF_1P_t lpf_roll;
//extern FWB_LPF_2P_t lpf2_roll;
extern FWB_LPF_1P_t lpf_pitch;
extern FWB_LPF_1P_t lpf_yaw;

extern FWB_LPF_1P_t lpf_roll_degree;
extern FWB_LPF_1P_t lpf_pitch_degree;


extern float LPF_FIFO_ROLL[FWB_LFP_BUFFER];
extern float LPF_FIFO_PITCH[FWB_LFP_BUFFER];



/*
void FWB_LADRC_Init(FWB_LADRC_t * ladrc);
void FWB_LADRC_Reset(FWB_LADRC_t * ladrc);
void FWB_LADRC_TD(FWB_LADRC_t * ladrc, float target);             // TD 跟踪器
void FWB_LADRC_ESO(FWB_LADRC_t * ladrc, float rt_val);            // ESO 观测器
void FWB_LADRC_LC(FWB_LADRC_t * ladrc);                           // 线性控制      
void FWB_LADRC_onLoop(FWB_LADRC_t * ladrc, float target_value, float reference_value);
*/

void FWB_PID_Init(float kp, float ki, float kd, float dt, FWB_PID_t * pid);
void FWB_PID_onLoop(FWB_PID_t * pid, float target, float actual, float offset, uint8_t i_On);

void Drone_attitude_Init(DRONE_ATTITUDE_t * drone);
void Drone_Change_TargetAttitude(DRONE_ATTITUDE_t * att);

float Drone_weight_factor_2nd_linear(float x);
void Drone_throttle_factor(float x, volatile float * throttle);
void Drone_Yaw_adjusting(float trig_left, float trig_right, float * offset);

void FWB_Init(void);

void FWB_LPF_Init(FWB_LPF_1P_t * lpf);
void FWB_LPF_onLoop(FWB_LPF_1P_t * lpf, float input);

void FWB_LPF2_Init(FWB_LPF_2P_t * lpf2);
void FWB_LPF2_onLoop(FWB_LPF_2P_t * lpf2, float input);

float constrain_float(float input, float min, float max);

#endif
