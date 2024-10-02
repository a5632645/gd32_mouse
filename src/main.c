#include "mouse_fop_handler.h"
#include "mouse_button.h"
#include "mouse_encoder.h"
#include "paw3205.h"
#include "uart_printf.h"
#include "my_timer.h"
#include <stdio.h>

// --------------------------------------------------------------------------------
// extern uint32_t SystemCoreClock;
#define SystemCoreClock (96*1000*1000U)
#define SYSTICK_HZ (1000U)
#define MS_TO_TICK(ms) ((ms) * (SystemCoreClock / SYSTICK_HZ))
#define TICK_TO_MS(tick) ((tick) * (SYSTICK_HZ / SystemCoreClock))
volatile uint32_t gTick = 0;
static void Systick_Init(void);

// --------------------------------------------------------------------------------
static void UartTask(MyTimerParamStruct* ctx) {
    printf("UartTask\n");
}

// --------------------------------------------------------------------------------
MyTimerStruct task = {
    .node = {
        .prev = NULL,
        .next = NULL,
    },
    .period = MS_TO_TICK(100),
    .callback = UartTask,
    .context = NULL,
};

int main(void) {
    UartPrintf_Init();
    Paw3205_Init();
    MouseUsb_Init();
    MouseButton_Init();
    MouseEncoder_Init();

    MyTimer_Reset(&task);
    for (;;) {
        uint32_t t = gTick;
        gTick = 0;
        MyTimer_Tick(&task, t);
    }
}

// --------------------------------------------------------------------------------
static void Systick_Init(void) {
    SysTick_Config(MS_TO_TICK(SYSTICK_HZ));
    nvic_irq_enable(SysTick_IRQn, 0, 0);
}

void SysTick_Handler(void) {
    ++gTick;
}