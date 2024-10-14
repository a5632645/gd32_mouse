#include "../uart_printf.h"
#include "../mouse_encoder.h"
#include "../delay.h"
#include <stdio.h>

static void DebugEncoder(MouseEncoderEnum encoder, int32_t* value, const char* name) {
    *value += MouseEncoder_Read(encoder);

    printf("%s: %d\n", name, *value);
}

void main(void) {
    MouseEncoder_Init();
    UartPrintf_Init();
    Delay_Init();
    
    int32_t wheel = 0;
    int32_t clickRate = 0;
    while (1) {
        DebugEncoder(eMouseEncoder_Wheel, &wheel, "Wheel");
        DebugEncoder(eMouseEncoder_ClickRate, &clickRate, "ClickRate");
        Delay_Ms(100);
    }
}