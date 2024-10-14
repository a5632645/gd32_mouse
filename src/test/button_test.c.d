#include "../mouse_button.h"
#include "../util/my_button.h"
#include "../uart_printf.h"
#include "../delay.h"

static void DebugButton(MyButtonStruct* state, MouseButtonEnum button, const char* name) {
    uint8_t press = MouseButton_IsPressed(button);
    MyButton_Tick(state, press, 0);

    switch (state->state) {
    case eButtonState_Click:
    UartPrintf_Puts(name);
    UartPrintf_Puts(" click\n");
        break;
    case eButtonState_Release:
    UartPrintf_Puts(name);
    UartPrintf_Puts(" release\n");
        break;
    case eButtonState_Press:
    UartPrintf_Puts(name);
    UartPrintf_Puts(" press\n");
        break;
    case eButtonState_Idel:
    UartPrintf_Puts(name);
    UartPrintf_Puts(" idel\n");
        break;
    default:
        break;
    }
}

void main(void) {
    MouseButton_Init();
    UartPrintf_Init();
    Delay_Init();

    MyButtonStruct left = {};
    MyButtonStruct right = {};
    MyButtonStruct center = {};
    MyButtonStruct autoclick = {};
    MyButtonStruct autopress = {};
    while (1) {
        DebugButton(&left, eMouseButtonLeft, "left");
        DebugButton(&right, eMouseButtonRight, "right");
        DebugButton(&center, eMouseButtonCenter, "center");
        DebugButton(&autoclick, eMouseButtonAutoClick, "autoclick");
        DebugButton(&autopress, eMouseButtonAutoPress, "autopress");
        Delay_Ms(100);
    }
}