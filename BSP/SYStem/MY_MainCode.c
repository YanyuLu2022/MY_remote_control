#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"     // ARM.FreeRTOS::RTOS:Core
#include "task.h"         // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h" // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"       // ARM.FreeRTOS::RTOS:Core
#include "queue.h"
#include "MY_MainCode.h"

#include "MY_bluetooth.h"
#include "LCD_API.h"
#include "key.h"
#define true 1
#define false 0
#define T_MAX 60 // 温度最大值
#define T_MIN 10 // 温度最小值
#define H_MAX 60 // 湿度最大值
#define H_MIN 10 // 湿度最小值
#define LIGHT_MAX 4
#define LIGHT_MIN 1
#define AIR_MAX 4
#define AIR_MIN 1
static StaticTask_t BlueRxThread; // 接收代码块
static StaticTask_t BlueTxThread; // 发送代码块
static StaticTask_t LCDThread;    // LCD代码块

StaticTask_t Task_MainCode; // 主任务句柄

static StackType_t MainCode_Thread_Buffer[128]; // 任务存储数组

static QueueHandle_t BlueRx_Queue = NULL; // 蓝牙读取队列句柄
static QueueHandle_t BlueTx_Queue = NULL; // 蓝牙读取队列句柄
static QueueHandle_t LCD_Queue = NULL;    // LCD队列句柄
static QueueHandle_t KEY_Queue = NULL;    // KEY队列句柄
static QueueHandle_t g_xQueueStrInput;    // 队列集句柄



uint8_t air = 0;      // 空气质量
uint8_t light = 0;    // 亮度

uint8_t H = 0;        // 湿度
uint8_t T = 0;        // 温度
uint8_t led = 0;      // LED灯状态
uint8_t car_modl = 0; // 小车状态

uint8_t new_car_modl = 0;
uint8_t new_directive = 0;

uint8_t air_Max = 2;                // 预警最大空气质量
uint8_t air_Min = 1;                // 预警最小空气质量
uint8_t light_Max = 2;              // 预警最大亮度
uint8_t light_Min = 1;              // 预警最小亮度
uint8_t H_Max = 50;                 // 预警最大湿度
uint8_t H_Min = 10;                 // 预警最小湿度
uint8_t T_Max = 50;                 // 预警最大温度
uint8_t T_Min = 10;                 // 预警最小温度
uint8_t location[4] = {1, 0, 0, 0}; //
uint8_t position = 0;               // 层数


void Updata_LCD_Data(uint8_t Set);
void Set_LCDQueue(void)
{
    LCD_API_DATA LCD_Data;
    LCD_Data.LCD_Mode[0] = location[0];
    LCD_Data.LCD_Mode[1] = location[1];
    LCD_Data.LCD_Mode[2] = location[2];
    LCD_Data.LCD_Mode[3] = location[3];
    LCD_Data.grade = position;
    xQueueSend(LCD_Queue, &LCD_Data, 0);
}
void Read_KEYRx_Queue(void)
{
    Key_str KEY;
    xQueueReceive(KEY_Queue, &KEY, 0);
    for (int i = 0; i < key_num; i++)
    {
        if (KEY.Read_Key[i] == 1)
            Updata_LCD_Data(i);
    }

    Set_LCDQueue();
}
void Read_BlueRx_Queue(void)
{
    BlueRx_Struct BlueRx_Data = {0};
    xQueueReceive(BlueRx_Queue, &BlueRx_Data, 0);
    air = (BlueRx_Data.air * 100 / 3.3 / 20);
    light = (BlueRx_Data.light * 100 / 3.3 / 20);
    H = BlueRx_Data.H;
    T = BlueRx_Data.T;
    led = BlueRx_Data.led;
    if(car_modl != BlueRx_Data.car_modl )
    new_car_modl = BlueRx_Data.car_modl;
    car_modl = BlueRx_Data.car_modl;
}
void Write_BlueTx(uint8_t directive,uint8_t mod)
{
    BlueTx_Struct BlueTx_Data;
    BlueTx_Data.directive = directive;
    BlueTx_Data.mod = mod;
	BlueTx_Data.led = 0;
	BlueTx_Data.music = 0;
    xQueueSend(BlueTx_Queue, &BlueTx_Data, 0);

}
void MainCode_Thread(void *argument)
{
    Blue_Init();
    LCDapi_init();
    KEY_Init();
    BlueRx_Queue = ReBlueRx_QueueStruct();
    BlueTx_Queue = ReBlueTx_QueueStruct();
    LCD_Queue = ReLCD_QueueStruct();
    KEY_Queue = ReKEY_QueueStruct();

    g_xQueueStrInput = xQueueCreateSet(BlueRxQueueLenght + KEYQueueLenght);
    xQueueAddToSet(BlueRx_Queue, g_xQueueStrInput); // 将队列添加进队列集
    xQueueAddToSet(KEY_Queue, g_xQueueStrInput);    // 将队列添加进队列集
    // 开启蓝牙接收
    BlueRx_Thread_Start(&BlueRxThread, NULL);
    // 开启蓝牙发送
    BlueTx_Thread_Start(&BlueTxThread, NULL);

    LCD_Thread_Start(&LCDThread, NULL);

    while (1)
    {
        QueueSetMemberHandle_t IntupQueue;

        // 读队列集得到句柄
        IntupQueue = xQueueSelectFromSet(g_xQueueStrInput, portMAX_DELAY);
        if (IntupQueue == BlueRx_Queue)
        {

            Read_BlueRx_Queue();
        }
        if (IntupQueue == KEY_Queue)
        {

            Read_KEYRx_Queue();
        }
        /* code */
    }
}

void MainCode_Thread_Start(StaticTask_t *PThread, void *Parameters)
{
    uint32_t BlueRx_Stack_L = sizeof(MainCode_Thread_Buffer) / sizeof(MainCode_Thread_Buffer[0]);
    xTaskCreateStatic(MainCode_Thread,
                      "BlueRx",
                      BlueRx_Stack_L,
                      Parameters,
                      (osPriority_t)osPriorityNormal,
                      MainCode_Thread_Buffer,
                      PThread);
}
void UpdateBoundary(uint8_t *value, uint8_t step, uint8_t min, uint8_t max, uint8_t increase)
{
    if (increase)
    {
        *value = (*value + step) > max ? max : (*value + step);
    }
    else
    {
        *value = (*value - step) < min ? min : (*value - step);
    }
}
void Updata_LCD_Data(uint8_t Set) // 修改页面
{
    switch (Set)
    {
    case 0: // 上
        if (location[0] == 3 && position > 2)
        {
            if (location[1] == 1)
            {
                if (location[2] == 1)
                    UpdateBoundary(&T_Max, 10, T_Min, T_MAX, true);
                if (location[2] == 2)
                    UpdateBoundary(&T_Min, 10, 10, T_Max, true);
            }
            else if (location[1] == 2)
            {
                if (location[2] == 1)
                    UpdateBoundary(&H_Max, 10, H_Min, H_MAX, true);
                if (location[2] == 2)
                    UpdateBoundary(&H_Min, 10, 10, H_Max, true);
            }
            else if (location[1] == 3)
            {
                if (location[2] == 1)
                    UpdateBoundary(&air_Max, 1, air_Min, AIR_MAX, true);
                if (location[2] == 2)
                    UpdateBoundary(&air_Min, 1, 1, air_Max, true);
            }
            else if (location[1] == 4)
            {
                if (location[2] == 1)
                    UpdateBoundary(&light_Max, 1, light_Min, LIGHT_MAX, true);
                if (location[2] == 2)
                    UpdateBoundary(&light_Min, 1, 1, light_Max, true);
            }
            else if (location[1] == 5)
            {
                location[2] = 1;
                new_car_modl = car_modl >= 4 ? 4 : (car_modl + 1);
            }
        }else if(location[0] == 2 && position > 0)
        {
            new_directive = 2;
            Write_BlueTx(new_directive,0);
        }

        else
        {
            location[position]++;
        }
        break;

    case 1: // 下
        if (location[0] == 3 && position > 2)
        {
            if (location[1] == 1)
            {
                if (location[2] == 1)
                    UpdateBoundary(&T_Max, 10, T_Min, T_MAX, false);
                if (location[2] == 2)
                    UpdateBoundary(&T_Min, 10, 10, T_Max, false);
            }
            else if (location[1] == 2)
            {
                if (location[2] == 1)
                    UpdateBoundary(&H_Max, 10, H_Min, H_MAX, false);
                if (location[2] == 2)
                    UpdateBoundary(&H_Min, 10, 10, H_Max, false);
            }
            else if (location[1] == 3)
            {
                if (location[2] == 1)
                    UpdateBoundary(&air_Max, 1, air_Min, AIR_MAX, false);
                if (location[2] == 2)
                    UpdateBoundary(&air_Min, 1, 1, air_Max, false);
            }
            else if (location[1] == 4)
            {
                if (location[2] == 1)
                    UpdateBoundary(&light_Max, 1, light_Min, LIGHT_MAX, false);
                if (location[2] == 2)
                    UpdateBoundary(&light_Min, 1, 1, light_Max, false);
            }
            else if (location[1] == 5)
            {
                location[2] = 1;
                new_car_modl = car_modl == 0 ? 0 : (car_modl - 1);
            }
        }else  if(location[0] == 2 && position > 0)
        {
            new_directive = 3;
            Write_BlueTx(new_directive,0);
        }
        else
        {
            location[position]--;
        }
        break;

        // 左
    case 2:
    if(location[0] == 2 && position > 0)
        {
            new_directive = 4;
            Write_BlueTx(new_directive,0);
        }else
        location[position]--;
        break;
        // 右
    case 3:
    if(location[0] == 2 && position > 0)
        {
            new_directive = 5;
            Write_BlueTx(new_directive,0);
        }else
        location[position]++;
        break;
        // 确定
    case 4:
        position = (position + 1) > 3 ? 3 : (position + 1);

        
        break;
        // 返回
    case 5:
        if(position == 3 && location[position] == 5)
        {
            Write_BlueTx(0,new_car_modl);
        }
        location[position] = 0;

        position = position == 0 ? 0 : --position;

        break;
    }

    if (position == 0)
    {
        location[0] = location[0] == 0 ? 3 : location[0];
        location[0] %= 4;
        location[0] = location[0] == 0 ? 1 : location[0];
    }
    else
    {
        if (location[0] == 1)
        {
            location[1] %= 2;
            position = position > 1 ? 1 : position;
        }
        if (location[0] == 2)
        {
            location[1] %= 2;
            position = position > 1 ? 1 : position;
        }
        if (location[0] == 3)
        {
            position = position > 3 ? 3 : position;
            location[1] = location[1] == 0 ? 5 : location[1];
            location[1] %= 6;
            location[1] = location[1] == 0 ? 1 : location[1];
            location[2] = location[2] == 0 ? 2 : location[2];
            location[2] %= 3;
            location[2] = location[2] == 0 ? 1 : location[2];
        }
    }
}
void MY_MainCode_Init(void)
{

    MainCode_Thread_Start(&Task_MainCode, NULL);
}
