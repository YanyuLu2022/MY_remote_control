#include "my_Rocker.h"
#include "cmsis_os.h"
#include <string.h>
#include "FreeRTOS.h"	  // ARM.FreeRTOS::RTOS:Core
#include "task.h"		  // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h" // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"		  // ARM.FreeRTOS::RTOS:Core 
#include "queue.h"
#define ADC_ThreadBuffer_L 128
// 队列句柄
static QueueHandle_t READC_Queue;
static float Rocker_X;
static float Rocker_Y;
static uint8_t Rocker_Up;
static uint8_t Rocker_state = 0;
StackType_t ADC_ThreadBuffer[ADC_ThreadBuffer_L];

uint16_t Read_ADCData[2] = {0};
void My_Rocker_Init(void)
{


  HAL_ADCEx_Calibration_Start(&hadc1);    //AD校准
  READC_Queue = xQueueCreate(READCQueueLenght, sizeof(Rocker_Queue_Data));

}
void * ReRocker_QueueStruct(void)
{
	return READC_Queue;
}
void ADC_GETDATA_Thread(void *argument)
{
    Rocker_Queue_Data Get_ADCData;
	while (1)
	{

        ADC_ChannelConfTypeDef _adc;
        _adc.Channel=ADC_CHANNEL_4;
        _adc.Rank=1;
        _adc.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc1,&_adc);
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,10);
        vTaskDelay(5);
        Rocker_X	= (float)(HAL_ADC_GetValue(&hadc1) *100) / 4096;

        _adc.Channel=ADC_CHANNEL_5;
        _adc.Rank=1;
        _adc.SamplingTime=ADC_SAMPLETIME_239CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc1,&_adc);
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,10);
        vTaskDelay(5);
        Rocker_Y	= (float)(HAL_ADC_GetValue(&hadc1) *100) / 4096;
        if(Rocker_X > 90 )
        {
          if(Rocker_Y < 15)
          {
            Get_ADCData.ReadRocker = 4;//右
          }else if(Rocker_Y > 90)
          {
            Get_ADCData.ReadRocker = 3;//左
          }else {

             Get_ADCData.ReadRocker = 2;//下
          }
        }else if(Rocker_X < 30 )
        {
          if(Rocker_Y < 15)
          {
            Get_ADCData.ReadRocker = 4;//右
          }else if(Rocker_Y > 90)
          {
            Get_ADCData.ReadRocker = 3;//左
          }else {

			  Get_ADCData.ReadRocker = 1;//上
          }
        }else
        {
          if(Rocker_Y < 30)
          {
            Get_ADCData.ReadRocker = 4;//右
          }else if(Rocker_Y > 90)
          {
            Get_ADCData.ReadRocker = 3;//左
          }else {

             Get_ADCData.ReadRocker = 0;//停
          }
        }
        switch (Rocker_state)
        {
        case 0:
          /* code */
          Rocker_Up = Get_ADCData.ReadRocker;
          Rocker_state = 1;
          xQueueSend(READC_Queue, &Get_ADCData, 0); /* code */	
          break;
        case 1:
          /* code */
          if(Get_ADCData.ReadRocker == Rocker_Up)
          {
            //相等时不发送
          }else
          {
			  
            Rocker_Up = Get_ADCData.ReadRocker;
            xQueueSend(READC_Queue, &Get_ADCData, 0); /* code */
          }
          break;
      

        default:
          break;
        }


	}
}
/*
StaticTask_t *PThread
*/
void Rocker_Thread_Start(void *PThread,void * pvParameters)
{
	xTaskCreateStatic(ADC_GETDATA_Thread,
					  "Rocker",
					  ADC_ThreadBuffer_L,
					  pvParameters,
					  (osPriority_t)osPriorityNormal,
					  ADC_ThreadBuffer,
					  PThread);
}
