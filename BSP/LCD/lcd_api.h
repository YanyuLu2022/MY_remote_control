#ifndef __LCDAPI_H
#define __LCDAPI_H
#include "main.h"
#include "lcd.h"

#define LCDQueueLenght 1
//extern float air;        // 空气质量
//extern float light;      // 亮度
//extern uint8_t H;        // 湿度
//extern uint8_t T;        // 温度
//extern uint8_t led;      // LED灯状态
//extern uint8_t car_modl; // 小车模式

//extern uint8_t air_Max;   // 空气质量
//extern uint8_t air_Min;   // 空气质量
//extern uint8_t light_Max; // 亮度
//extern uint8_t light_Min; // 亮度
//extern uint8_t H_Max;     // 湿度
//extern uint8_t H_Min;     // 湿度
//extern uint8_t T_Max;     // 温度
//extern uint8_t T_Min;     // 温度
typedef struct 
{
uint8_t LCD_Mode[4] ;
uint8_t grade;
	
}LCD_API_DATA;

void LCDapi_init(void);
void LCD_Clear(uint16_t color);
void * ReLCD_QueueStruct(void);  // 返回队列句柄
void LCD_Thread_Start(void *PThread, void *Parameters);

#endif
