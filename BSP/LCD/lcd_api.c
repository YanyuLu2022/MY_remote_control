#include "lcd_api.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"     // ARM.FreeRTOS::RTOS:Core
#include "task.h"         // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h" // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"       // ARM.FreeRTOS::RTOS:Core
#include "bmp.h"
#define LCD_Thread_lenght 128 // 任务堆栈大小

static StackType_t LCD_Thread_Buffer[LCD_Thread_lenght]; // 任务存储数组
uint8_t LCD_Mode[4] = {1, 0, 0, 0};
uint8_t grade = 0;
extern float air;            // 空气质量
extern float light;          // 亮度
extern uint8_t H;            // 湿度
extern uint8_t T;            // 温度
extern uint8_t led;          // LED灯状态
extern uint8_t car_modl;     // 小车模式
extern uint8_t new_car_modl; // 小车模式
extern uint8_t new_directive;

extern uint8_t air_Max;   // 空气质量
extern uint8_t air_Min;   // 空气质量
extern uint8_t light_Max; // 亮度
extern uint8_t light_Min; // 亮度
extern uint8_t H_Max;     // 湿度
extern uint8_t H_Min;     // 湿度
extern uint8_t T_Max;     // 温度
extern uint8_t T_Min;     // 温度

static QueueHandle_t LCD_Queue = NULL; // 队列句柄
void *ReLCD_QueueStruct(void)          // 返回队列句柄
{
      return LCD_Queue;
}

void LCDapi_init(void)
{
      LCD_Init();
      LCD_Queue = xQueueCreate(LCDQueueLenght, sizeof(LCD_API_DATA));
}
/******************************************************************************
      函数说明：清屏
      入口数据：color   清屏的颜色
      返回值：  无
******************************************************************************/
void LCD_Clear(uint16_t color)
{
      LCD_Fill(0, 0, LCD_W, LCD_H, color);
}
void Update_LCD_Install_2(void)
{
      if (LCD_Mode[1] == 5)
      {

            if (LCD_Mode[2] != 0)
            {

                  LCD_ShowString(0, 16 * 0, "MOD", BLACK, BROWN, 16, 0);
				  LCD_ShowString(24, 16 * 0, ":", BLACK, BROWN, 16, 0);

                  LCD_ShowIntNum(32, 16 * 0, new_car_modl, 2, BLACK, BROWN, 16);
            }
      }
      else
      {
            if (LCD_Mode[2] == 1)
            {
                  LCD_ShowString(0, 16 * 0, "Max", BLACK, BROWN, 16, 0);
				                  LCD_ShowString(24, 16 * 0, ":", BLACK, BROWN, 16, 0);

                  LCD_ShowString(0, 16 * 1, "Min", BROWN, BLACK, 16, 0);
                  LCD_ShowString(24, 16 * 1, ":", BROWN, BLACK, 16, 0);

            }
            else if (LCD_Mode[2] == 2)
            {
                  LCD_ShowString(0, 16 * 0, "Max", BROWN, BLACK, 16, 0);
				                  LCD_ShowString(24, 16 * 0, ":", BROWN, BLACK, 16, 0);

                  LCD_ShowString(0, 16 * 1, "Min", BLACK, BROWN, 16, 0);
				                  LCD_ShowString(24, 16 * 1, ":", BLACK, BROWN, 16, 0);

            }

            switch (LCD_Mode[1])
            {
            case 1:
                  if (LCD_Mode[2] == 1)
                  {
                        LCD_ShowIntNum(32, 16 * 0, T_Max, 2, BLACK, BROWN, 16);
                        LCD_ShowIntNum(32, 16 * 1, T_Min, 2, BROWN, BLACK, 16);
                  }
                  else if (LCD_Mode[2] == 2)
                  {
                        LCD_ShowIntNum(32, 16 * 0, T_Max, 2, BROWN, BLACK, 16);
                        LCD_ShowIntNum(32, 16 * 1, T_Min, 2, BLACK, BROWN, 16);
                  }

                  break;
            case 2:
                  if (LCD_Mode[2] == 1)
                  {
                        LCD_ShowIntNum(32, 16 * 0, H_Max, 2, BLACK, BROWN, 16);
                        LCD_ShowIntNum(32, 16 * 1, H_Min, 2, BROWN, BLACK, 16);
                  }
                  else if (LCD_Mode[2] == 2)
                  {
                        LCD_ShowIntNum(32, 16 * 0, H_Max, 2, BROWN, BLACK, 16);
                        LCD_ShowIntNum(32, 16 * 1, H_Min, 2, BLACK, BROWN, 16);
                  }

                  break;
            case 3:
                  if (LCD_Mode[2] == 1)
                  {
                        LCD_ShowIntNum(32, 16 * 0, air_Max, 2, BLACK, BROWN, 16);
                        LCD_ShowIntNum(32, 16 * 1, air_Min, 2, BROWN, BLACK, 16);
                  }
                  else if (LCD_Mode[2] == 2)
                  {

                        LCD_ShowIntNum(32, 16 * 0, air_Max, 2, BROWN, BLACK, 16);
                        LCD_ShowIntNum(32, 16 * 1, air_Min, 2, BLACK, BROWN, 16);
                  }

                  break;
            case 4:
                  if (LCD_Mode[2] == 1)
                  {
                        LCD_ShowIntNum(32, 16 * 0, light_Max, 2, BLACK, BROWN, 16);
                        LCD_ShowIntNum(32, 16 * 1, light_Min, 2, BROWN, BLACK, 16);
                  }
                  else if (LCD_Mode[2] == 2)
                  {
                        LCD_ShowIntNum(32, 16 * 0, light_Max, 2, BROWN, BLACK, 16);
                        LCD_ShowIntNum(32, 16 * 1, light_Min, 2, BLACK, BROWN, 16);
                  }

                  break;

            default:
                  break;
            }
      }
}
void Update_LCD_Environment(void)
{

      LCD_ShowChinese(0, 16 * 0, 3, BROWN, BLACK, 16, 0); // 温度
      LCD_ShowChinese(16, 16 * 0, 4, BROWN, BLACK, 16, 0);
      LCD_ShowString(32, 16 * 0, ":", BROWN, BLACK, 16, 0);
      if (T > 10)
            LCD_ShowIntNum(40, 16 * 0, T, 2, BROWN, BLACK, 16);
      else
            LCD_ShowIntNum(40, 16 * 0, T, 1, BROWN, BLACK, 16);
      LCD_ShowChinese(0, 16 * 1, 5, BROWN, BLACK, 16, 0); // 湿度
      LCD_ShowChinese(16, 16 * 1, 4, BROWN, BLACK, 16, 0);
      LCD_ShowString(32, 16 * 1, ":", BROWN, BLACK, 16, 0);
      if (H > 10)
            LCD_ShowIntNum(40, 16 * 1, H, 2, BROWN, BLACK, 16);
      else
            LCD_ShowIntNum(40, 16 * 1, H, 1, BROWN, BLACK, 16);

      LCD_ShowChinese(0, 16 * 2, 10, BROWN, BLACK, 16, 0); // 空气
      LCD_ShowChinese(16, 16 * 2, 11, BROWN, BLACK, 16, 0);
      LCD_ShowString(32, 16 * 2, ":", BROWN, BLACK, 16, 0);
      LCD_ShowIntNum(40, 16 * 2, air, 1, BROWN, BLACK, 16);

      LCD_ShowChinese(0, 16 * 3, 12, BROWN, BLACK, 16, 0); // 光照
      LCD_ShowChinese(16, 16 * 3, 13, BROWN, BLACK, 16, 0);
      LCD_ShowString(32, 16 * 3, ":", BROWN, BLACK, 16, 0);
      LCD_ShowIntNum(40, 16 * 3, light, 1, BROWN, BLACK, 16);

      LCD_ShowString(0, 16 * 4, "CAR", BROWN, BLACK, 16, 0); // 小车模式
      LCD_ShowString(24, 16 * 4, ":", BROWN, BLACK, 16, 0); // 小车模式

      LCD_ShowIntNum(32, 16 * 4, car_modl, 1, BROWN, BLACK, 16);
}
void Update_LCD_CAR(void)
{
      // LCD_Fill(0, 72, 16, 23, BLACK);
      // LCD_Fill(110, 72, 16, 23, BLACK);
      switch (new_directive)
      {
      case 1:
            /* code */
            LCD_ShowString(28, 66, "Stop", BROWN, BLACK, 32, 0);
            break;
      case 2:
            /* code */
            LCD_ShowString(45, 66, "Up", BROWN, BLACK, 32, 0);
            break;
      case 3:
            /* code */
            LCD_ShowString(28, 66, "Down", BROWN, BLACK, 32, 0);
            break;
      case 4:
            /* code */
            LCD_ShowString(28, 66, "Left", BROWN, BLACK, 32, 0);
            break;
      case 5:
            /* code */
            LCD_ShowString(22, 66, "Right", BROWN, BLACK, 32, 0);
            break;
      default:
            LCD_ShowString(45, 66, "NO", BROWN, BLACK, 32, 0);
            break;
      }
      

      // 

      // 
      // 
      // 

      // LCD_ShowChinese(28, 66, 3,  BLACK,BROWN, 32, 0);
      // LCD_ShowChinese(64, 66, 4,  BLACK,BROWN, 32, 0); // 小车
}

/****
 * 1:温度
 * 2:湿度
 * 3:空气
 * 4:光照
 * 5:小车模式
 */
void Update_LCD_Install(void)
{
      if (grade > 1)
      {
            Update_LCD_Install_2();
            return;
      }
      if (LCD_Mode[1] == 1)
      {
            LCD_ShowChinese(0, 16 * 0, 3, BLACK, BROWN, 16, 0); // 温度
            LCD_ShowChinese(16, 16 * 0, 4, BLACK, BROWN, 16, 0);
      }
      else
      {
            LCD_ShowChinese(0, 16 * 0, 3, BROWN, BLACK, 16, 0); // 温度
            LCD_ShowChinese(16, 16 * 0, 4, BROWN, BLACK, 16, 0);
      }
      if (LCD_Mode[1] == 2)
      {
            LCD_ShowChinese(0, 16 * 1, 5, BLACK, BROWN, 16, 0); // 湿度
            LCD_ShowChinese(16, 16 * 1, 4, BLACK, BROWN, 16, 0);
      }
      else
      {
            LCD_ShowChinese(0, 16 * 1, 5, BROWN, BLACK, 16, 0); // 湿度
            LCD_ShowChinese(16, 16 * 1, 4, BROWN, BLACK, 16, 0);
      }
      if (LCD_Mode[1] == 3)
      {
            LCD_ShowChinese(0, 16 * 2, 10, BLACK, BROWN, 16, 0); // 空气
            LCD_ShowChinese(16, 16 * 2, 11, BLACK, BROWN, 16, 0);
      }
      else
      {
            LCD_ShowChinese(0, 16 * 2, 10, BROWN, BLACK, 16, 0); // 空气
            LCD_ShowChinese(16, 16 * 2, 11, BROWN, BLACK, 16, 0);
      }
      if (LCD_Mode[1] == 4)
      {
            LCD_ShowChinese(0, 16 * 3, 12, BLACK, BROWN, 16, 0); // 光照
            LCD_ShowChinese(16, 16 * 3, 13, BLACK, BROWN, 16, 0);
      }
      else
      {
            LCD_ShowChinese(0, 16 * 3, 12, BROWN, BLACK, 16, 0); // 光照
            LCD_ShowChinese(16, 16 * 3, 13, BROWN, BLACK, 16, 0);
      }
      if (LCD_Mode[1] == 5)
      {

            LCD_ShowString(0, 16 * 4, "CAR", BLACK, BROWN, 16, 0); // 小车模式
      }
      else
      {
            LCD_ShowString(0, 16 * 4, "CAR", BROWN, BLACK, 16, 0); // 小车模式
      }
}
void Update_LCD(void)
{
      if (LCD_Mode[0] != 1 && LCD_Mode[0] != 2 && LCD_Mode[0] != 3)
            return;
      switch (LCD_Mode[0])
      {
      case 1:
            // 环境图像1
            if (grade > 0)
            {
                  Update_LCD_Environment();
                  return;
            }
            LCD_ShowPicture(0, 72, 16, 23, gImage_left);
            LCD_ShowPicture(110, 72, 16, 23, gImage_right);
            LCD_ShowChinese(28, 66, 5, BLACK, WHITE, 32, 0);
            LCD_ShowChinese(64, 66, 6, BLACK, WHITE, 32, 0); // 环境
            break;

      case 2:
            // 小车界面1
            if (grade > 0)
            {
                  Update_LCD_CAR();
                  return;
            }
            LCD_ShowPicture(0, 72, 16, 23, gImage_left);
            LCD_ShowPicture(110, 72, 16, 23, gImage_right);
            LCD_ShowChinese(28, 66, 4, BLACK, WHITE, 32, 0);
            LCD_ShowChinese(64, 66, 3, BLACK, WHITE, 32, 0); // 小车
            break;
      case 3:
            // 设置
            if (grade > 0)
            {
                  Update_LCD_Install();
                  return;
            }
            LCD_ShowPicture(0, 72, 16, 23, gImage_left);
            LCD_ShowPicture(110, 72, 16, 23, gImage_right);
            LCD_ShowChinese(28, 66, 1, BLACK, WHITE, 32, 0);
            LCD_ShowChinese(64, 66, 2, BLACK, WHITE, 32, 0); // 设置
            break;
      default:
            break;
      }
}

void LCD_Thread(void *argument)
{

      LCD_Clear(WHITE);
      LCD_ShowPicture(0, 72, 16, 23, gImage_left);
      LCD_ShowPicture(110, 72, 16, 23, gImage_right);
      while (1)
      {
            LCD_API_DATA lcd_data;
            if (pdPASS == xQueueReceive(LCD_Queue, &lcd_data, 0))
            {

                  LCD_Mode[0] = lcd_data.LCD_Mode[0];
                  LCD_Mode[1] = lcd_data.LCD_Mode[1];
                  LCD_Mode[2] = lcd_data.LCD_Mode[2];
                  LCD_Mode[3] = lcd_data.LCD_Mode[3];
                  if (lcd_data.grade != grade)
                  {
                        if (lcd_data.grade == 0)
                        {
                              LCD_Clear(WHITE);
                              LCD_ShowPicture(0, 72, 16, 23, gImage_left);
                              LCD_ShowPicture(110, 72, 16, 23, gImage_right);
                        }

                        if (lcd_data.grade != 0)
                              LCD_Clear(BLACK);
                        grade = lcd_data.grade;
                  }
                  if (lcd_data.grade == 1 && lcd_data.LCD_Mode[0] == 2)
                  {
                        LCD_Fill(0, 66, LCD_W, 66+32 , BLACK);
                  }
            }
            Update_LCD();
            vTaskDelay(39);
      }
}
void LCD_Thread_Start(void *PThread, void *Parameters)
{
      uint32_t LCD_Stack_L = sizeof(LCD_Thread_Buffer) / sizeof(LCD_Thread_Buffer[0]);
      xTaskCreateStatic(LCD_Thread,
                        "LCD_Thread",
                        LCD_Stack_L,
                        Parameters,
                        (osPriority_t)osPriorityNormal,
                        LCD_Thread_Buffer,
                        PThread);
}
