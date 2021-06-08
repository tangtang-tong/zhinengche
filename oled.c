#include "sys.h"
#include "oled.h"
#include "delay.h"
#include "font.h"
#include "usart.h"
u8 OLED_GRAM[128][8];
void oled_init()
{
    GPIO_InitTypeDef gpio_initstructe;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    gpio_initstructe.GPIO_Pin=GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13;
    gpio_initstructe.GPIO_Mode=GPIO_Mode_Out_PP;
    gpio_initstructe.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpio_initstructe); 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    gpio_initstructe.GPIO_Pin=GPIO_Pin_0;
    gpio_initstructe.GPIO_Mode=GPIO_Mode_Out_PP;
    gpio_initstructe.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOD,&gpio_initstructe);
    
    OLED_res_Clr();
    delay_ms(200);//等待20ms
    OLED_res_Set();
   /* spi_write_data(0xAE,0);
    spi_write_data(0x00,0);
    spi_write_data(0x10,0);
    spi_write_data(0x40,0);
    spi_write_data(0x81,0);
    spi_write_data(0xCF,0);
    spi_write_data(0xA1,0);
    spi_write_data(0xC8,0);
    spi_write_data(0xA6,0);
    spi_write_data(0xA8,0);
        spi_write_data(0x3F,0);
        spi_write_data(0xD3,0);
        spi_write_data(0x00,0);
        spi_write_data(0xd5,0);
        spi_write_data(0x80,0);
        spi_write_data(0xD9,0);
        spi_write_data(0xF1,0);
        spi_write_data(0xDA,0);
        spi_write_data(0x12,0);
        spi_write_data(0xDB,0);
        spi_write_data(0x40,0);
            spi_write_data(0x20,0);
            spi_write_data(0x02,0);
            spi_write_data(0x8D,0);
            spi_write_data(0x14,0);
            spi_write_data(0xA4,0);
            spi_write_data(0xA6,0);

    spi_write_data(0xAF,0);*/
    spi_write_data(0xC8,0);//上下正常
    spi_write_data(0xA1,0);//左右正常
}
void spi_write_data(u8 data,u8 cmd)//0是命令使能
{
    u8 i;
    if(cmd) OLED_dc_Set();
    else OLED_dc_Clr();

    
    for(i=0;i<8;i++)
    {
        OLED_clk_Clr();
        if(data&0x80) OLED_mosi_Set();
        else OLED_mosi_Clr();
        OLED_clk_Set();
        data<<=1;
    }

    
    OLED_dc_Set();
}
void OLED_DisPlay_On(void)
{
    spi_write_data(0x8D,0);
    spi_write_data(0x14,0);
    spi_write_data(0xAF,0);
}

void oled_write_ram(void)
{
    u8 i,j;
    for(i=0;i<8;i++)//page addressing mode 
    {
        spi_write_data(0xb0+i,0);
        spi_write_data(0x00,0);
        spi_write_data(0x10,0);//
        for(j=0;j<128;j++)
        {
            spi_write_data(OLED_GRAM[j][i],1);
        }
    }
}
void test(void)
{
    u8 i,j;
    for(i=0;i<128;i++)
    {
    for( j=0;j<8;j++)
        {
            OLED_GRAM[i][j]=0x00;
        }
    }
    oled_write_ram();   
    //refresh
}
void oled_write_point(u8 x,u8 y)
{
    u8 a,b;
    a=y/8;
    b=y%8;
    OLED_GRAM[x][a]|=1<<b;
    oled_write_ram();
}
void oled_write_char(u8 x,u8 y,u8 w)
{
    u8 i,g;
    g=y%8;
    y=y/8;
    if(g==0)
    {
        for(i=0;i<8;i++)
    {
     OLED_GRAM[x+i][y]|=lower[w][2*i];
     OLED_GRAM[x+i][y+1]|=lower[w][2*i+1];
        
    }
    
    }else{
    for(i=0;i<8;i++)
    {
    OLED_GRAM[x+i][y]|=lower[w][2*i]>>g-1;
        OLED_GRAM[x+i][y+1]|=lower[w][2*i]<<(8-g+1);
    OLED_GRAM[x+i][y+1]|=lower[w][2*i+1]>>g-1;
        if(y<=7) OLED_GRAM[x+i][y+2]|=lower[w][2*i+1]<<(8-g+1);
    }}
    oled_write_ram();
}
void oled_write_nums(u8 x,u8 y,u8 w)
{
    u8 i,g;
    g=y%8;
    y=y/8;
    if(g==0)
    {
        for(i=0;i<8;i++)
    {
     OLED_GRAM[x+i][y]|=nums[w][2*i];
     OLED_GRAM[x+i][y+1]|=nums[w][2*i+1];
        
    }
    
    }else{
    for(i=0;i<8;i++)
    {
     OLED_GRAM[x+i][y]|=nums[w][2*i]>>(g-1);
        OLED_GRAM[x+i][y+1]|=nums[w][2*i]<<(8-g+1);
     OLED_GRAM[x+i][y+1]|=nums[w][2*i+1]>>(g-1);
        if(y<=7) OLED_GRAM[x+i][y+2]|=nums[w][2*i+1]<<(8-g+1);
}}
    oled_write_ram();
}
void oled_write_string(u8 x,u8 y,char *a)
{
    u8 i;
    for(i=0;a[i]!='\0';i++)
    {
            if(a[i]>=48&&a[i]<=57)
            {
            oled_write_nums(x+i*8,y,a[i]-'0');
                
            }
            else if(a[i]>=97&&a[i]<=122)
            {
            oled_write_char(x+i*8,y,a[i]-'a');
            }
    }
    oled_write_ram();
}

