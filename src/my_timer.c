#include "my_timer.h"
#include <stddef.h>

void MyTimer_Reset(MyTimerStruct* task, uint32_t len) {
    while (len--)
        task[len].tickLeft = task[len].period;
}

void MyTimer_Tick(MyTimerStruct* task, uint32_t len, uint32_t tickEscape) {
    while (len--) {
        task->tickLeft -= tickEscape;
        if (task->tickLeft <= 0) {
            task->callback(tickEscape, task->userdata);
        }
        while (task->tickLeft <= 0) {
            task->tickLeft += task->period;
        }
        ++task;
    }
}