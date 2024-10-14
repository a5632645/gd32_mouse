#include "leds.h"
#include <gd32f10x_gpio.h>

#define LED_SERIAL_GPIO GPIOA
#define LED_SERIAL_PIN GPIO_PIN_2

#define LED_RCK_GPIO GPIOA
#define LED_RCK_PIN GPIO_PIN_3

#define LED_SCK_GPIO GPIOA
#define LED_SCK_PIN GPIO_PIN_4

typedef struct {
    uint8_t : 1;
    uint8_t acl : 1;
    uint8_t apl : 1;
    uint8_t acc : 1;
    uint8_t apc : 1;
    uint8_t acr : 1;
    uint8_t apr : 1;
    uint8_t : 1;
} LedStruct;
static LedStruct sLed;

void Leds_Init(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(LED_SERIAL_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_SERIAL_PIN);
    gpio_init(LED_RCK_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_RCK_PIN);
    gpio_init(LED_SCK_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_SCK_PIN);
    gpio_bit_reset(LED_SERIAL_GPIO, LED_SERIAL_PIN);
    gpio_bit_reset(LED_RCK_GPIO, LED_RCK_PIN);
    gpio_bit_reset(LED_SCK_GPIO, LED_SCK_PIN);
    Led_Send(0);
}

void Leds_Set(LedEnum led, uint8_t state) {
    switch (led) {
        case eLed_APL:
            sLed.apl = state;
            break;
        case eLed_APR:
            sLed.apr = state;
            break;
        case eLed_APC:
            sLed.apc = state;
            break;
        case eLed_ACL:
            sLed.acl = state;
            break;
        case eLed_ACR:
            sLed.acr = state;
            break;
        case eLed_ACC:
            sLed.acc = state;
            break;
        default:
            break;
    }
}

void Led_Update(void) {
    Led_Send(*(uint8_t *)&sLed);
}

void Led_Send(uint8_t code) {
    gpio_bit_reset(LED_RCK_GPIO, LED_RCK_PIN);
    for (uint8_t i = 0; i < 8; ++i) {
        gpio_bit_reset(LED_SCK_GPIO, LED_SCK_PIN);
        if (code & 0x01) {
            gpio_bit_set(LED_SERIAL_GPIO, LED_SERIAL_PIN);
        } else {
            gpio_bit_reset(LED_SERIAL_GPIO, LED_SERIAL_PIN);
        }
        code >>= 1;
        gpio_bit_set(LED_SCK_GPIO, LED_SCK_PIN);
    }
    gpio_bit_set(LED_RCK_GPIO, LED_RCK_PIN);
}

void Led_SimpleTest(uint8_t* state) {
    if (*state == 0) {
        *state = 1;
    }
    Led_Send(*state);
    *state <<= 1;
}
