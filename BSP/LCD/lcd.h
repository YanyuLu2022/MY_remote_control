#ifndef __LCD_H
#define __LCD_H
 
#include "main.h"


#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif
//
	 
 
//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
 
//define LCD PIN
#define LCD_SCLK_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)
#define LCD_SCLK_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)

#define LCD_MOSI_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)
#define LCD_MOSI_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)

#define LCD_RES_Clr()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)
#define LCD_RES_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)

#define LCD_DC_Clr()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET)
#define LCD_DC_Set()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET)
 		     
#define LCD_CS_Clr()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET)
#define LCD_CS_Set()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET)

#define LCD_BLK_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET)
#define LCD_BLK_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET)
 
void LCD_Init(void);
void LCD_Reset(void);
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
uint32_t mypow(uint8_t m,uint8_t n);
void LCD_ShowIntNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]);
void LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);














#endif
