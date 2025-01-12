#include "main.h"
#include "key.h"
#include "FreeRTOS.h"     // ARM.FreeRTOS::RTOS:Core
#include "task.h"         // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h" // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"       // ARM.FreeRTOS::RTOS:Core
#include "cmsis_os.h"
typedef struct
{
    uint8_t state;
    uint8_t Get_Key;

}Buffer_str;

static Buffer_str Key[key_num];
static QueueHandle_t KEY_Queue = NULL; // 队列句柄
void * ReKEY_QueueStruct(void)  // 返回队列句柄
{
    return KEY_Queue;
}

void Update_Key(void)
{
    GPIO_PinState key_state[key_num];

    key_state[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
    key_state[1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
    key_state[2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
    key_state[3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
    key_state[4] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
    key_state[5] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);
    key_state[6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
    for (int i = 0; i < key_num; i++)
    {

        switch (Key[i].state)
        {
        case 0:
            if (key_state[i] == GPIO_PIN_RESET)
            {
                Key[i].state = 1;
            }
            break;
        case 1:
            if (key_state[i] == GPIO_PIN_RESET)
            {
                // 按键首次按下
                Key[i].Get_Key = 1;
                Key[i].state = 2;
            }
            else
            {

                Key[i].state = 0;
            }
            break;
        case 2:
            if (key_state[i] == GPIO_PIN_RESET)
            {
                
                // 按键长安
                
            }
            else
            {
                // 松开按键
                                
                Key[i].state = 0;
            }
            break;
        }
    }
    uint8_t k = 0;
    Key_str get_key;
    for(int i = 0; i < key_num; i++)
    {
        if(Key[i].Get_Key == 1)
        {   get_key.Read_Key[i] = 1;
        Key[i].Get_Key = 0;
            k = 1;
        }else
        {
            get_key.Read_Key[i] = 0;

        }
        
    }
    if(k == 1)
    {
        xQueueSendToBackFromISR(KEY_Queue,&get_key,0);
        k = 0;
    }
}
void KEY_Init(void)
{
   KEY_Queue = xQueueCreate(KEYQueueLenght, sizeof(Key_str));
}

