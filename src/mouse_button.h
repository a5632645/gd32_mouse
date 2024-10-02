#pragma once
#include <stdint.h>

typedef enum {
    eMouseButtonLeft = 0,
    eMouseButtonRight = 1,
    eMouseButtonCenter = 2,
    eMouseButtonAutoClick = 3,
    eMouseButtonAutoPress = 4,
} MouseButtonEnum;

void MouseButton_Init(void);
uint8_t MouseButton_IsPressed(MouseButtonEnum button);