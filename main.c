#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "pwm.h"
#include "pid.h"
#include "adc.h"
int main()
{
    float adc_local;
    u16 len,x,t,l,flag=1,i;
    u8 gg[2]={0x0d,0x0a};
    u8 test[27]="whowhowhowhowhowhowhowho\n\r";
    extern u16 speed;
	delay_init();	 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);    
    USART2_Init(115200);// ������ʼ��
    pwm_init_1(0,8999,100);
    pwm_init_2(0,8999,100);//Ƶ�ʲ���̫�� 8khz
    pulse_encode(0);
    pulse_encode_x(0);
    tim_tick(7200-1,100);//72000000/7200/10000=1                                     s
    pid_DeInit(2000,100);
    pwm_init_steer(400-1,3600-1,270);//72000000/400/3600=50hz,90<x<270 ,1.5ms�����
    adc_init();
    l=200; 
    while(1)
    {
        if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵�ֵΪ:\r\n\r\n");
            x=0;
			for(t=0;t<len;t++)
			{
				x+=(USART_RX_BUF[t]-'0')*pow0(len,t);
			}
            printf("\n\r%d",x);
            //TIM_SetCompare2(TIM3,100);
            pid_DeInit(x,100);
            //TIM_SetCompare1(TIM4,x);
            printf("speed is %d",speed-32767);
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;
        }
        flag=1;
        if(0)
        {
            if(flag) l++;
            else l--;
            if(l>400) flag=0;
            if(l<=120)  flag=1;      
            TIM_SetCompare1(TIM4,l);
            delay_ms(30);
            
        }
        if(1)
        {
            //usart2_send('7');
            delay_ms(1000);
            //for(i=0;i<15;i++)
            //{
                //printf("what");
                //printf("\r\n\r\n");
            usart_send_data(test,27);
            adc_local = (float) adc_value[0]/4096*3.3;
            //printf("%f\r\n",adc_local);
               // delay_ms(100);
            usart_send_data(gg,2);
            usart_send_data(gg,2);
        //}
        //flag=0;
        }
    } 
}

