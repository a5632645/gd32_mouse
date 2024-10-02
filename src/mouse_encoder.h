#pragma once
#include <stdint.h>

typedef enum {
    eMouseEncoder_Wheel = 0,
    eMouseEncoder_ClickRate = 1,
} MouseEncoderEnum;

void MouseEncoder_Init(void);
int32_t MouseEncoder_Read(MouseEncoderEnum encoder);