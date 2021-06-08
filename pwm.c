#include "pwm.h"
#include "adc.h"
u16 speed=0;
float adc_filter();
void adc_pid(float process_value,struct pid_m *a);

void pwm_init_2(u32 psc,u32 arr,u16 cmp)//tim3ͨ��2 remap->PC7 finish
{
    GPIO_InitTypeDef gpio_initure;
    TIM_TimeBaseInitTypeDef tim_initure;
    TIM_OCInitTypeDef tim_ociniture;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    
    tim_initure.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_initure.TIM_CounterMode=TIM_CounterMode_Up;
    tim_initure.TIM_Period=arr;
    tim_initure.TIM_Prescaler=psc;
    TIM_TimeBaseInit(TIM3,&tim_initure);
    
    gpio_initure.GPIO_Mode=GPIO_Mode_AF_PP;
    gpio_initure.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_initure.GPIO_Pin=GPIO_Pin_7;
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
    GPIO_Init(GPIOC,&gpio_initure);
    

    tim_ociniture.TIM_OCMode=TIM_OCMode_PWM2;
    tim_ociniture.TIM_OutputState=TIM_OutputState_Enable;
    tim_ociniture.TIM_Pulse=cmp;
    tim_ociniture.TIM_OCPolarity=TIM_OCPolarity_High;
    tim_ociniture.TIM_OCIdleState=TIM_OCIdleState_Set;

    TIM_OC2Init(TIM3,&tim_ociniture);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); 
    TIM_Cmd(TIM3,ENABLE);
}
void pulse_encode(u32 psc)//tim2 ch1 remap->PA15,PB3 finish
{
     GPIO_InitTypeDef gpio_initure;
    TIM_TimeBaseInitTypeDef tim_initure;
    TIM_ICInitTypeDef tim_iciniture;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    tim_initure.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_initure.TIM_CounterMode=TIM_CounterMode_Up;
    tim_initure.TIM_Period=65535;
    tim_initure.TIM_Prescaler=psc;
    TIM_TimeBaseInit(TIM2,&tim_initure);

    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2,ENABLE);
    
    gpio_initure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    gpio_initure.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_initure.GPIO_Pin=GPIO_Pin_15;
    GPIO_Init(GPIOA,&gpio_initure);
    
        gpio_initure.GPIO_Pin=GPIO_Pin_3;
    GPIO_Init(GPIOB,&gpio_initure);
    
    
    TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

    tim_iciniture.TIM_ICFilter=10;
    TIM_ICInit(TIM2,&tim_iciniture);
    TIM_SetCounter(TIM2,0x7fff);
    TIM_ClearFlag(TIM2,TIM_IT_Update);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2,ENABLE);
    TIM2->CNT=0x7fff;
}
void tim_tick(u16 psc,u32 arr)//tim 6 ch1 pb6,5ms��ʱ��arr=7200-1,psc
{
    GPIO_InitTypeDef gpio_initure;
    TIM_TimeBaseInitTypeDef tim_initure;
    TIM_OCInitTypeDef tim_ociniture;
    NVIC_InitTypeDef nvic_initure;
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
    
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
    
    //gpio_initure.GPIO_Mode=GPIO_Mode_AF_PP;
    //gpio_initure.GPIO_Speed=GPIO_Speed_50MHz;
    //gpio_initure.GPIO_Pin=GPIO_Pin_6;
   // GPIO_Init(GPIOB,&gpio_initure);
    
    
    tim_initure.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_initure.TIM_CounterMode=TIM_CounterMode_Up;
    tim_initure.TIM_Period=100;
    tim_initure.TIM_Prescaler=71;
    TIM_TimeBaseInit(TIM6,&tim_initure);
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
    
    nvic_initure.NVIC_IRQChannel=TIM6_IRQn;
    nvic_initure.NVIC_IRQChannelCmd=ENABLE;
    nvic_initure.NVIC_IRQChannelSubPriority=3;
    nvic_initure.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_Init(&nvic_initure);
    
    TIM_Cmd(TIM6,ENABLE);
}
void TIM6_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
    {        
			printf("L speed is %d",speed);
			speed_rpm_read(&speed,TIM2);
			start_pid(speed);
			printf("R speed is %d",speed);
			speed_rpm_read(&speed,TIM1);
			start_pid_1(speed);
			TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
			
			struct pid_m tong;
			tong.kd = 0;
			tong.ki = 0;
			tong.kp = 2;
			tong.lerror = 0;
			tong.perror = 0;
			tong.result = 0;
			tong.setup = 0;
			tong.time = 0;
			tong.time++;
			while(1){
				 if(tong.time ==2)
					  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
				    float filter_try = adc_filter();
				    tong.time = 0;
				    adc_pid(filter_try,&tong);
				    
				    
				    
				    
				   
			
			
			  
    }
}
u16 getTIMX_data(TIM_TypeDef *TIMx)
{
    u16 cnt;
    cnt=TIMx->CNT-0x7fff;
    TIMx->CNT=0x7fff;
    return cnt;
}
void speed_rpm_read(u16 *a,TIM_TypeDef *TIMx)
{
    *a=getTIMX_data(TIMx);
}
u16 pow0(u16 len,u16 t)
{
    u16 i=0,res=1,h;
    h=len-t-1;
    for(i=0;i<h;i++)
    {
        res*=10;
    }
    return res;
}
void pwm_init_steer(u32 psc,u32 arr,u16 cmp)//tim4ͨ��1 PD12
{
    GPIO_InitTypeDef gpio_initure;
    TIM_TimeBaseInitTypeDef tim_initure;
    TIM_OCInitTypeDef tim_ociniture;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
    
    tim_initure.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_initure.TIM_CounterMode=TIM_CounterMode_Up;
    tim_initure.TIM_Period=arr;
    tim_initure.TIM_Prescaler=psc;
    TIM_TimeBaseInit(TIM4,&tim_initure);
    
    GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);

    gpio_initure.GPIO_Mode=GPIO_Mode_AF_PP;
    gpio_initure.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_initure.GPIO_Pin=GPIO_Pin_12;
    GPIO_Init(GPIOD,&gpio_initure);
    

    tim_ociniture.TIM_OCMode=TIM_OCMode_PWM1;
    tim_ociniture.TIM_OutputState=TIM_OutputState_Enable;
    tim_ociniture.TIM_Pulse=cmp;
    tim_ociniture.TIM_OCPolarity=TIM_OCPolarity_High;
    tim_ociniture.TIM_OCIdleState=TIM_OCIdleState_Set;

    TIM_OC1Init(TIM4,&tim_ociniture);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); 
    TIM_Cmd(TIM4,ENABLE);
}
void pwm_init_1(u32 psc,u32 arr,u16 cmp)//tim5 PA3
{
    GPIO_InitTypeDef gpio_initure;
    TIM_TimeBaseInitTypeDef tim_initure;
    TIM_OCInitTypeDef tim_ociniture;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
    
    tim_initure.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_initure.TIM_CounterMode=TIM_CounterMode_Up;
    tim_initure.TIM_Period=arr;
    tim_initure.TIM_Prescaler=psc;
    TIM_TimeBaseInit(TIM5,&tim_initure);
    
   

    gpio_initure.GPIO_Mode=GPIO_Mode_AF_PP;
    gpio_initure.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_initure.GPIO_Pin=GPIO_Pin_3;
    GPIO_Init(GPIOA,&gpio_initure);
    

    tim_ociniture.TIM_OCMode=TIM_OCMode_PWM2;
    tim_ociniture.TIM_OutputState=TIM_OutputState_Enable;
    tim_ociniture.TIM_Pulse=cmp;
    tim_ociniture.TIM_OCPolarity=TIM_OCPolarity_High;
    tim_ociniture.TIM_OCIdleState=TIM_OCIdleState_Set;

    TIM_OC4Init(TIM5,&tim_ociniture);
    TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable); 
    TIM_Cmd(TIM5,ENABLE);
}
void pulse_encode_x(u32 psc)//tim1 ch1 remap->PE9,PE11 finish
{
     GPIO_InitTypeDef gpio_initure;
    TIM_TimeBaseInitTypeDef tim_initure;
    TIM_ICInitTypeDef tim_iciniture;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO|RCC_APB2Periph_TIM1,ENABLE);
    
    
    tim_initure.TIM_ClockDivision=TIM_CKD_DIV1;
    tim_initure.TIM_CounterMode=TIM_CounterMode_Up;
    tim_initure.TIM_Period=65535;
    tim_initure.TIM_Prescaler=psc;
    TIM_TimeBaseInit(TIM1,&tim_initure);

    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);
    
    gpio_initure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    gpio_initure.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_initure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_11;
    GPIO_Init(GPIOE,&gpio_initure);
    

    
    
    TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);

    tim_iciniture.TIM_ICFilter=10;
    TIM_ICInit(TIM1,&tim_iciniture);
    TIM_SetCounter(TIM1,0x7fff);
    TIM_ClearFlag(TIM1,TIM_IT_Update);
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM1,ENABLE);
    TIM1->CNT=0x7fff;
}