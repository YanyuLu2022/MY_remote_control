#include "stdio.h"
#include "string.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"     // ARM.FreeRTOS::RTOS:Core
#include "task.h"         // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h" // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"       // ARM.FreeRTOS::RTOS:Core
#include <stdarg.h>
#include <stdlib.h>

#include "MY_bluetooth.h"
#include "My_Usart.h"

#define BlueTx_Thread_lenght 128 // 任务堆栈大小
#define BlueRx_Thread_lenght 128 // 任务堆栈大小

static StackType_t BlueTx_Thread_Buffer[BlueTx_Thread_lenght]; // 任务存储数组
static StackType_t BlueRx_Thread_Buffer[BlueRx_Thread_lenght]; // 任务存储数组

static QueueHandle_t BlueTx_Queue = NULL; // 队列句柄
static QueueHandle_t BlueRx_Queue = NULL; // 队列句柄

QueueHandle_t ReBlueTx_QueueStruct(void)  // 返回队列句柄
{
    return BlueTx_Queue;
}
QueueHandle_t ReBlueRx_QueueStruct(void) // 返回队列句柄
{
    return BlueRx_Queue;
}

void Blue_Init(void)
{
	My_Uart2_Init();
    BlueRx_Queue = xQueueCreate(BlueRxQueueLenght, sizeof(BlueRx_Struct)); // 创建队列
    BlueTx_Queue = xQueueCreate(BlueTxQueueLenght, sizeof(BlueTx_Struct));
    
}

int validateString(const char *input)
{
    // 检查是否以 "+{" 开头，"}+" 结尾
    if (strncmp(input, "+{", 2) != 0 || strncmp(input + strlen(input) - 2, "}+", 2) != 0)
    {
        return 0; // 格式错误
    }

    // 检查关键键值对是否存在
    if (strstr(input, "T:") == NULL || strstr(input, "H:") == NULL ||
        strstr(input, "light:") == NULL || strstr(input, "air:") == NULL ||
        strstr(input, "led:") == NULL || strstr(input, "car_modl:") == NULL)
    {
        return 0; // 缺少键值对
    }

    return 1; // 格式正确
}
void BlueRx_Thread(void *argument)
{
    static Uart2_Struct Uart2Rx_Buffer; // 接收缓冲区
    QueueHandle_t Uart_queue = Return2_P_UartStruct();
    BlueRx_Struct BlueRx_Data = {0};
    char *input;
    uint8_t output[50];
	
    while (1)
    {
        xQueueReceive(Uart_queue, &Uart2Rx_Buffer, portMAX_DELAY);
        input = (char *)&Uart2Rx_Buffer.rx_str;
        const char *start = strstr(input, "+{");
        if (start)
        {
            const char *end = strstr(start, "}+"); // 查找结束标志 "}+"
            if (end)
            {
                size_t length = end - start + 2; // 计算子字符串长度，包含 "}+"
                if (length < RX2_Counter_Max)
                {
                    strncpy((char *)output, start, length); // 提取子字符串
                    output[length] = '\0';                  // 添加字符串结束符
                    if (validateString((char *)output) == 1)
                    {
						int T;
						int H;
						int led;
						int car_modl;						
                        sscanf(input, "+{T:%d,H:%d,light:%f,air:%f,led:%d,car_modl:%d}+",
                               &T,
                               &H,
                               &BlueRx_Data.light,
                               &BlueRx_Data.air,
                               &led,
                               &car_modl);
							   BlueRx_Data.T = T;
							   BlueRx_Data.H = H;
							   BlueRx_Data.led = led;
							   BlueRx_Data.car_modl = car_modl;							   

                        xQueueSend(BlueRx_Queue, &BlueRx_Data, 0);
                    }
                }
            }
        }

        /* code */
    }
}
void BlueTx_Thread(void *argument)
{

    static BlueTx_Struct BlueTx_Data = {0};
    static uint8_t UartTx_Data[6];
    UartTx_Data[0] = 0x7E;
    UartTx_Data[5] = 0XEF;
    while (1)
    {

        xQueueReceive(BlueTx_Queue, &BlueTx_Data, portMAX_DELAY);

        if (0 != BlueTx_Data.directive)
            UartTx_Data[1] = BlueTx_Data.directive;
        if (0 != BlueTx_Data.mod)
            UartTx_Data[2] = BlueTx_Data.mod;
        if (0 != BlueTx_Data.music)
            UartTx_Data[3] = BlueTx_Data.music;
        if (0 != BlueTx_Data.led)
            UartTx_Data[4] = BlueTx_Data.led;
        Usart_TX_Data(2, 1000, &UartTx_Data, 6);
	

    }
}

void BlueTx_Thread_Start(StaticTask_t *PThread, void *Parameters)
{
    uint32_t BlueTx_Stack_L = sizeof(BlueTx_Thread_Buffer) / sizeof(BlueTx_Thread_Buffer[0]);
    xTaskCreateStatic(BlueTx_Thread,
                      "BlueTx",
                      BlueTx_Stack_L,
                      Parameters,
                      (osPriority_t)osPriorityNormal,
                      BlueTx_Thread_Buffer,
                      PThread);
}

void BlueRx_Thread_Start(StaticTask_t *PThread, void *Parameters)
{
    uint32_t BlueRx_Stack_L = sizeof(BlueRx_Thread_Buffer) / sizeof(BlueRx_Thread_Buffer[0]);
    xTaskCreateStatic(BlueRx_Thread,
                      "BlueRx",
                      BlueRx_Stack_L,
                      Parameters,
                      (osPriority_t)osPriorityNormal,
                      BlueRx_Thread_Buffer,
                      PThread);
}
