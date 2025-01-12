#ifndef __My__ADC__H
#define __My__ADC__H


#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "adc.h"
#define READCQueueLenght 5
/*
1上
2下
3左
4右
*/
typedef struct
{
    uint8_t ReadRocker;
} Rocker_Queue_Data;
void My_Rocker_Init(void);
void Rocker_Thread_Start(void *PThread,void * pvParameters);
void * ReRocker_QueueStruct(void);

#endif //__Main_TASK_H
