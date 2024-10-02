#include "my_timer.h"
#include <stddef.h>

#define MYTIMER_NODE_TO_STRUCT(node) ((MyTimerStruct*)(node))

void MyTimer_Reset(MyTimerStruct* task) {
    while (task != NULL) {
        task->tickLeft = task->period;
        task = MYTIMER_NODE_TO_STRUCT(task->node.next);
    }
}

void MyTimer_Tick(MyTimerStruct* task, uint32_t tickEscape) {
    MyTimerParamStruct ctx = {
        .escape = tickEscape,
        .head = task,
    };
    while (task != NULL) {
        task->tickLeft -= tickEscape;
        if (task->tickLeft <= 0) {
            ctx.self = task;
            task->callback(&ctx);
        }
        while (task->tickLeft <= 0) {
            task->tickLeft += task->period;
        }
        task = MYTIMER_NODE_TO_STRUCT(task->node.next);
    }
}