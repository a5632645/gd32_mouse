#include "mouse_encoder.h"
#include "gd32f10x_timer.h"
#include "gd32f10x_gpio.h"
#include "gd32f10x_rcu.h"

#define WHEEL_ENCODER TIMER1
#define CLICK_RATE_ENCODER TIMER2

typedef struct {
    uint32_t : 2;
    uint32_t : 2;
    uint32_t ch0Filt : 4;
    uint32_t : 2;
    uint32_t : 2;
    uint32_t ch1Filt : 4;
} TimerChCfg1Struct;

void MouseEncoder_Init(void) {
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(WHEEL_ENCODER);
    timer_deinit(CLICK_RATE_ENCODER);

    timer_parameter_struct initStruct = {
        .prescaler = 4,
        .alignedmode = TIMER_COUNTER_EDGE,
        .clockdivision = TIMER_CKDIV_DIV1,
        .counterdirection = TIMER_COUNTER_UP,
        .repetitioncounter = 0,
        .period = 65535
    };
    timer_init(WHEEL_ENCODER, &initStruct);
    initStruct.period = 8;
    timer_init(CLICK_RATE_ENCODER, &initStruct);

    timer_quadrature_decoder_mode_config(WHEEL_ENCODER, TIMER_QUAD_DECODER_MODE2, TIMER_IC_POLARITY_RISING, TIMER_IC_POLARITY_RISING);
    timer_quadrature_decoder_mode_config(CLICK_RATE_ENCODER, TIMER_QUAD_DECODER_MODE2, TIMER_IC_POLARITY_RISING, TIMER_IC_POLARITY_RISING);

    timer_auto_reload_shadow_enable(WHEEL_ENCODER);
    timer_auto_reload_shadow_enable(CLICK_RATE_ENCODER);

    timer_counter_value_config(WHEEL_ENCODER, 32768);
    timer_counter_value_config(CLICK_RATE_ENCODER, 32768);

    // ic滤波
    ((volatile TimerChCfg1Struct*)WHEEL_ENCODER)->ch0Filt = 0b1111;
    ((volatile TimerChCfg1Struct*)WHEEL_ENCODER)->ch1Filt = 0b1111;
    ((volatile TimerChCfg1Struct*)CLICK_RATE_ENCODER)->ch0Filt = 0b1111;
    ((volatile TimerChCfg1Struct*)CLICK_RATE_ENCODER)->ch1Filt = 0b1111;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

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
