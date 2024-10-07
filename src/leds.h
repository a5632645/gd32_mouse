#pragma once
#include <stdint.h>

typedef enum {
    eLed_APL = 0,
    eLed_APR,
    eLed_APC,
    eLed_ACL,
    eLed_ACR,
    eLed_ACC,
} LedEnum;

void Leds_Init(void);
void Leds_Set(LedEnum led, uint8_t state);
void Led_Update(void);