#pragma once
#include <stdint.h>

typedef struct __MyTimerStruct {
    uint32_t tickLeft;
    uint32_t period;
    void(*callback)(uint32_t escape, void* userdata);
    void* userdata;
}MyTimerStruct;

void MyTimer_Reset(MyTimerStruct* task, uint32_t len);
void MyTimer_Tick(MyTimerStruct* task, uint32_t len, uint32_t tickEscape);