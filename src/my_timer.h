#pragma once
#include <stdint.h>

struct __LinkListNode {
    struct __LinkListNode* prev;
    struct __LinkListNode* next;
};

typedef struct {
    uint32_t escape;
    struct __MyTimerStruct* head;
    struct __MyTimerStruct* self;
}MyTimerParamStruct;

typedef struct __MyTimerStruct {
    struct __LinkListNode node;
    uint32_t tickLeft;
    uint32_t period;
    void(*callback)(MyTimerParamStruct* ctx);
    void* context;
}MyTimerStruct;

void MyTimer_Reset(MyTimerStruct* task);
void MyTimer_Tick(MyTimerStruct* task, uint32_t tickEscape);