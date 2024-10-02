#include "mouse_encoder.h"
#include "gd32f10x_timer.h"
#include "gd32f10x_gpio.h"
#include "gd32f10x_rcu.h"

#define WHEEL_ENCODER TIMER3
#define CLICK_RATE_ENCODER TIMER2

void MouseEncoder_Init(void) {
    rcu_periph_clock_enable(RCU_TIMER3);
    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(WHEEL_ENCODER);
    timer_deinit(CLICK_RATE_ENCODER);

    timer_parameter_struct initStruct = {
        .prescaler = 0,
        .alignedmode = TIMER_COUNTER_EDGE,
        .clockdivision = TIMER_CKDIV_DIV1,
        .counterdirection = TIMER_COUNTER_UP,
        .repetitioncounter = 0,
        .period = 65535
    };
    timer_init(WHEEL_ENCODER, &initStruct);
    timer_init(CLICK_RATE_ENCODER, &initStruct);

    timer_quadrature_decoder_mode_config(WHEEL_ENCODER, TIMER_QUAD_DECODER_MODE2, TIMER_IC_POLARITY_RISING, TIMER_IC_POLARITY_RISING);
    timer_quadrature_decoder_mode_config(CLICK_RATE_ENCODER, TIMER_QUAD_DECODER_MODE2, TIMER_IC_POLARITY_RISING, TIMER_IC_POLARITY_RISING);

    timer_auto_reload_shadow_enable(WHEEL_ENCODER);
    timer_auto_reload_shadow_enable(CLICK_RATE_ENCODER);

    timer_counter_value_config(WHEEL_ENCODER, 32768);
    timer_counter_value_config(CLICK_RATE_ENCODER, 32768);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    timer_enable(WHEEL_ENCODER);
    timer_enable(CLICK_RATE_ENCODER);
}

static int32_t MouseGetEncoderValue(uint32_t encoder) {
    uint32_t val = timer_counter_read(encoder);
    timer_counter_value_config(encoder, 32768);
    return (int32_t)(val - 32768);
}

int32_t MouseEncoder_Read(MouseEncoderEnum encoder) {
    switch (encoder) {
        case eMouseEncoder_Wheel:
            return MouseGetEncoderValue(WHEEL_ENCODER);
        case eMouseEncoder_ClickRate:
            return MouseGetEncoderValue(CLICK_RATE_ENCODER);
    }
    return 0;
}
