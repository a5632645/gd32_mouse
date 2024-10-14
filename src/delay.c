#include "delay.h"
#include "gd32f10x_timer.h"
#include <stdio.h>

#define DELAY_TIMER TIMER0
#define TIMER_CLK_HZ (96*1000*1000)

void Delay_Init(void) {
    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(DELAY_TIMER);
    timer_parameter_struct init = {
        .alignedmode = TIMER_COUNTER_EDGE,
        .clockdivision = TIMER_CKDIV_DIV1,
        .counterdirection = TIMER_COUNTER_UP,
        .period = 65535,
        .prescaler = 65535,
        .repetitioncounter = 0
    };
    timer_init(DELAY_TIMER, &init);
}

void Delay_Ms(uint32_t ms) {
    timer_disable(DELAY_TIMER);
    timer_prescaler_config(DELAY_TIMER, ms * 2 - 1, TIMER_PSC_RELOAD_NOW);
    timer_autoreload_value_config(DELAY_TIMER, TIMER_CLK_HZ / 2000 - 1);
    timer_counter_value_config(DELAY_TIMER, 0);
    timer_flag_clear(DELAY_TIMER, TIMER_FLAG_UP);
    timer_enable(DELAY_TIMER);
    while (SET != timer_flag_get(DELAY_TIMER, TIMER_FLAG_UP));
}

void Delay_Us(uint32_t us) {
    timer_disable(DELAY_TIMER);
    timer_prescaler_config(DELAY_TIMER, us - 1, TIMER_PSC_RELOAD_NOW);
    timer_autoreload_value_config(DELAY_TIMER, TIMER_CLK_HZ / 1000000 - 1);
    timer_counter_value_config(DELAY_TIMER, 0);
    timer_flag_clear(DELAY_TIMER, TIMER_FLAG_UP);
    timer_enable(DELAY_TIMER);
    while (SET != timer_flag_get(DELAY_TIMER, TIMER_FLAG_UP));
}