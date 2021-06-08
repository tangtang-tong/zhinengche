#include "sys.h" 
#include "usart.h"
#include "pid.h"
void pwm_init_1(u32 psc,u32 arr,u16 cmp);//tim5 PA3
void pwm_init_2(u32 psc,u32 arr,u16 cmp);//tim3ͨ��2
void speed_rpm_read(u16 *a,TIM_TypeDef *TIMx);
u16 getTIMX_data(TIM_TypeDef *TIMx);
void tim_tick(u16 psc,u32 arr);//tim 5 ch1 pb6,5ms��ʱ��arr=7200-1,psc
void pulse_encode(u32 psc);//tim2 ch1//pa 15 pb3
void pulse_encode_x(u32 psc);//tim1 ch1 remap->PE9,PE11 finish
u16 pow0(u16 len,u16 t);
extern u16 speed;
void pwm_init_steer(u32 psc,u32 arr,u16 cmp);
struct pid_m
{
    float setup;
    float result;
    float kp;
    float ki;
    float kd;
    float perror;
    float lerror;
	  float time;
	  float temp0;
};

float temp1;


