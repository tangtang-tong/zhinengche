#ifndef __OLED_H
#define __OLED_H 

#include "sys.h"


#define OLED_dc_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_13)//dc
#define OLED_dc_Set() GPIO_SetBits(GPIOB,GPIO_Pin_13)

#define OLED_res_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_0)//res
#define OLED_res_Set() GPIO_SetBits(GPIOD,GPIO_Pin_0)

#define OLED_clk_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_14)//clk
#define OLED_clk_Set() GPIO_SetBits(GPIOB,GPIO_Pin_14)
 		     
#define OLED_mosi_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_15)//mosi
#define OLED_mosi_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_15)

#define u32 unsigned int
#define u8 unsigned char
    
void test(void);
void spi_write_data(u8 data,u8 cmd);//0 «√¸¡Ó πƒ‹
void spi_write_data_u16(u16 data,u8 cmd);
void oled_write_ram(void);
void oled_init(void);
void oled_source_it(void);
void test_it(void);
void OLED_DisPlay_On(void);void oled_write_char(u8 x,u8 y,u8 w);
void oled_write_point(u8 x,u8 y);
void oled_write_nums(u8 x,u8 y,u8 w);
void oled_write_string(u8 x,u8 y,char *a);
#endif
