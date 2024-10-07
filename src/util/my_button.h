#pragma once
#include <stdint.h>

typedef enum {
    eButtonState_Idel = 0,
    eButtonState_Click = 1,
    eButtonState_Press = 2,
    eButtonState_Release = 3
} ButtonStateEnum;

typedef struct __ButtonStateStruct {
    ButtonStateEnum state;
    uint32_t pressTime;
} MyButtonStruct;

void MyButton_Tick(MyButtonStruct* state, uint8_t press, uint32_t escape);
