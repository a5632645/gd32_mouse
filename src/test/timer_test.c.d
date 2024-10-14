#include "../leds.h"
#include "../util/my_timer.h"
#include <CMSIS/GD/GD32F10x/Include/gd32f10x.h>

// ---------------------------------------- led1 ----------------------------------------
static void Led1() {
    static uint8_t s = 0;
    Leds_Set(eLed_ACC, s);
    ++s;
    s &= 1;
    Led_Update();
}

static void Led2() {
    static uint8_t s = 0;
    Leds_Set(eLed_ACL, s);
    ++s;
    s &= 1;
    Led_Update();
}

static void Led3() {
    static uint8_t s = 0;
    Leds_Set(eLed_ACR, s);
    ++s;
    s &= 1;
    Led_Update();
}

MyTimerStruct tasks[] = {
    {
        .period = 10,
        .callback = Led1,
    },
    {
        .period = 50,
        .callback = Led2,
    },
    {
        .period = 1000,
        .callback = Led3
    }
};
const uint32_t len = sizeof(tasks) / sizeof(MyTimerStruct);

#define COUNT_OF_1MS (96*1000)
volatile static uint32_t g_tick = 0;
void main(void) {
    Leds_Init();
    MyTimer_Reset(tasks, len);
    
    // 1ms
    SysTick_Config(COUNT_OF_1MS);

    while (1) {
        while (g_tick < 1);
        uint32_t escape = g_tick;
        g_tick = 0;
        MyTimer_Tick(tasks, len, escape);
    }
}

void SysTick_Handler(void) {
    ++g_tick;
}
