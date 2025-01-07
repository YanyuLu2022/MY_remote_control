#include "stm32f1xx_hal.h"

#define key_num 6
#define KEYQueueLenght 10

typedef struct
{
    
    uint8_t Read_Key[key_num];

}Key_str;
void KEY_Init(void);
void * ReKEY_QueueStruct(void);  // 返回队列句柄
void Update_Key(void);
