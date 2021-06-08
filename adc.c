#include "adc.h"

u32 adc_value[3];
u32 adc_filter_value[10]={0};
float filter_result=0;
struct pid_m
{
    float setup;
    float result;
    float kp;
    float ki;
    float kd;
    float perror;
    float lerror;
};



/*adc 的setup值因该为零，观测值为两边的电感绝对值的差，对观测值进行滤波*/
void adc_init()
{
    GPIO_InitTypeDef gpio_initure;
    DMA_InitTypeDef dma_initure;
    ADC_InitTypeDef adc_initure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    gpio_initure.GPIO_Mode=GPIO_Mode_AIN;
    gpio_initure.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_initure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
    GPIO_Init(GPIOC,&gpio_initure);

    DMA_DeInit(DMA1_Channel1);

    dma_initure.DMA_PeripheralBaseAddr=(u32) (&(ADC1->DR));
    dma_initure.DMA_MemoryBaseAddr=(u32) &adc_value;
    dma_initure.DMA_DIR=DMA_DIR_PeripheralSRC;
    dma_initure.DMA_BufferSize=3;
    dma_initure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
    dma_initure.DMA_MemoryInc=DMA_MemoryInc_Enable;
    dma_initure.DMA_M2M=DMA_M2M_Disable;
    dma_initure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
    dma_initure.DMA_Mode=DMA_Mode_Circular;
    dma_initure.DMA_Priority=DMA_Priority_High;
    dma_initure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;

    DMA_Init(DMA1_Channel1,&dma_initure);
    DMA_Cmd(DMA1_Channel1,ENABLE);

    adc_initure.ADC_Mode=ADC_Mode_Independent;
    adc_initure.ADC_ScanConvMode=ENABLE;
    adc_initure.ADC_ContinuousConvMode=ENABLE;
    adc_initure.ADC_DataAlign=ADC_DataAlign_Right;
    adc_initure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
    adc_initure.ADC_NbrOfChannel=3;
    
    ADC_Init(ADC1,&adc_initure);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_11,1,ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1,ADC_Channel_12,1,ADC_SampleTime_55Cycles5);

    ADC_DMACmd(ADC1,ENABLE);
    ADC_Cmd(ADC1,ENABLE);
    
    ADC_ResetCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}
void adc_pid(float process_value,struct pid_m *a)
{
    float this_error;
    float increment;
    float pError,iError,dError;
    this_error=a->setup-process_value;
    pError=this_error-a->lerror;
    iError=this_error;
    dError=this_error-2*a->lerror+a->perror; 
    increment=a->kp*pError+a->ki*iError+a->kd*dError;
    a->result+=increment;
    a->perror=a->lerror;
    a->lerror=this_error;
    /*
        控制句柄写在下面,
        TODO:
        在ITM5控制周期中断里调用pid,考虑到舵机的控制周期为20ms，每20ms启动一次ad转换(软件启动)
    */
   TIM_SetCompare1(TIM4,180-a->result);
	 
}
float adc_filter()//采用滑动平均滤波法，将每次的采样值0，1进行处理,这样会导致前10次pid的整定速度降低
{
    int i=1;
    float temp,result,filter_result;
    filter_result-=adc_filter_value[0];
    for(;i<10;i++)
    {
        adc_filter_value[i-1]=adc_filter_value[i];
    }
    temp=adc_value[0]-adc_value[2];
    adc_filter_value[9]=temp;
    filter_result+=temp;
    result=temp>0?filter_result/10:-filter_result/10;
    
    return filter_result/10;
}
/*TODO:
检测环，记录舵机打死一次，然后执行出环检测加速 注：注意连续过弯的情况
*/