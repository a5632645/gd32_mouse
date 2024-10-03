#include "mouse_fop_handler.h"
#include "mouse_button.h"
#include "mouse_encoder.h"
#include "paw3205.h"
#include "uart_printf.h"
#include "my_timer.h"
#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------------------------
// debug task
// --------------------------------------------------------------------------------
static void UartTimer(uint32_t escape, void* userdata) {
    printf("UartTask\n");
}

// --------------------------------------------------------------------------------
// paw3205 task
// --------------------------------------------------------------------------------
#define MY_ABS(x) ((x) < 0 ? -(x) : (x))
#define MY_SIGNBIT(x) ((x) < 0 ? 1 : 0)
#define MY_MIN(x, y) ((x) < (y) ? (x) : (y))
#define MY_MAX(x, y) ((x) > (y) ? (x) : (y))
#define MY_CLAMP(x, min, max) MY_MAX(MY_MIN(x, max), min) 

static MotionStruct gMotionStruct;
static void Paw3205Timer(uint32_t escape, void* userdata) {
    Paw3205_GetMotion(&gMotionStruct);
    gMouseReport->dx = MY_ABS(gMotionStruct.dx);
    gMouseReport->dy = MY_ABS(gMotionStruct.dy);
    gMouseReport->bits.dxSign = MY_SIGNBIT(gMotionStruct.motionStatus.dxOverflow);
    gMouseReport->bits.dySign = MY_SIGNBIT(gMotionStruct.motionStatus.dyOverflow);
    gMouseReport->bits.dxOverflow = gMotionStruct.motionStatus.dxOverflow;
    gMouseReport->bits.dyOverflow = gMotionStruct.motionStatus.dyOverflow;
}

static void Paw3205SyncTimer(uint32_t escape, void* userdata) {
    Paw3205_TrySync();
}

// --------------------------------------------------------------------------------
// hw task
// --------------------------------------------------------------------------------
struct {
    uint8_t autoclickLeft : 1;
    uint8_t autoclickRight : 1;
    uint8_t autoclickCenter : 1;
    uint8_t autopressLeft : 1;
    uint8_t autopressRight : 1;
    uint8_t autopressCenter : 1;
    int32_t autoclickRate;
}gAutoParams = {};

typedef enum {
    eButton_Idel = 0,
    eButton_Click = 1,
    eButton_Press = 2,
    eButton_Release = 3
}ButtonStateEnum;
struct {
    ButtonStateEnum autoclick;
    ButtonStateEnum autopress;
    ButtonStateEnum left;
    ButtonStateEnum right;
    ButtonStateEnum center;
}gButtonState = {};
__STATIC_INLINE void ButtonStateTick(ButtonStateEnum* state, uint8_t press) {
    switch (*state) {
    case eButton_Idel:
        if (press)
            *state = eButton_Click;
        break;
    case eButton_Click:
        if (press)
            *state = eButton_Press;
        else
            *state = eButton_Release;
        break;
    case eButton_Press:
        if (!press)
            *state = eButton_Release;
        break;
    case eButton_Release:
        if (press)
            *state = eButton_Click;
        else
            *state = eButton_Idel;
        break;
    default:
        break;
    }
}

static void HwTimer(uint32_t escape, void* userdata) {
    uint8_t leftPress = MouseButton_IsPressed(eMouseButtonLeft);
    uint8_t rightPress = MouseButton_IsPressed(eMouseButtonRight);
    uint8_t centerPress = MouseButton_IsPressed(eMouseButtonCenter);
    uint8_t autoclickPress = MouseButton_IsPressed(eMouseButtonAutoClick);
    uint8_t autopressPress = MouseButton_IsPressed(eMouseButtonAutoPress);

    ButtonStateTick(&gButtonState.autoclick, autoclickPress);
    ButtonStateTick(&gButtonState.autopress, autopressPress);
    ButtonStateTick(&gButtonState.left, leftPress);
    ButtonStateTick(&gButtonState.right, rightPress);
    ButtonStateTick(&gButtonState.center, centerPress);

    if (centerPress) {
        gMouseReport->bits.center = 1;
    }
    if (leftPress) {
        gMouseReport->bits.left = 1;
    }
    if (rightPress) {
        gMouseReport->bits.right = 1;
    }

    int32_t dWheel = MouseEncoder_Read(eMouseEncoder_Wheel);
    gMouseReport->wheel = MY_CLAMP(dWheel, INT8_MIN, INT8_MAX);

    if (autoclickPress) {
        if (gButtonState.left == eButton_Click) {
            gAutoParams.autoclickLeft = !gAutoParams.autoclickLeft;
        }
        if (gButtonState.right == eButton_Click) {
            gAutoParams.autoclickRight = !gAutoParams.autoclickRight;
        }
        if (gButtonState.center == eButton_Click) {
            gAutoParams.autoclickCenter = !gAutoParams.autoclickCenter;
        }
    }
    if (autopressPress) {
        if (gButtonState.left == eButton_Click) {
            gAutoParams.autopressLeft = !gAutoParams.autopressLeft;
        }
        if (gButtonState.right == eButton_Click) {
            gAutoParams.autopressRight = !gAutoParams.autopressRight;
        }
        if (gButtonState.center == eButton_Click) {
            gAutoParams.autopressCenter = !gAutoParams.autopressCenter;
        }
    }

    MyTimerStruct* autoclickTask = (MyTimerStruct*)userdata;
    gAutoParams.autoclickRate += MouseEncoder_Read(eMouseEncoder_ClickRate);
    if (gAutoParams.autoclickRate < 1)
        gAutoParams.autoclickRate = 1;
    autoclickTask->period = gAutoParams.autoclickRate;
}

// --------------------------------------------------------------------------------
// usb task
// --------------------------------------------------------------------------------
static void UsbTimer(uint32_t escape, void* userdata) {
    if (!MouseUsb_IsReady())
        return;

    // autopress强制覆盖
    if (gAutoParams.autoclickLeft) {
        gMouseReport->bits.left = 1;
    }
    if (gAutoParams.autoclickRight) {
        gMouseReport->bits.right = 1;
    }
    if (gAutoParams.autoclickCenter) {
        gMouseReport->bits.center = 1;
    }

    MouseUsb_Send();
    MouseUsb_ResetReport();
}

// --------------------------------------------------------------------------------
// auto click timer
// --------------------------------------------------------------------------------
struct {
    uint8_t left : 1;
    uint8_t right : 1;
    uint8_t center : 1;
}gAutoClickState;
static void AutoClickTimer(uint32_t escape, void* userdata) {
    if (gAutoParams.autoclickLeft) {
        gAutoClickState.left = !gAutoClickState.left;
        gMouseReport->bits.left = gAutoClickState.left;
    }
    if (gAutoParams.autoclickRight) {
        gAutoClickState.right = !gAutoClickState.right;
        gMouseReport->bits.right = gAutoClickState.right;
    }
    if (gAutoParams.autoclickCenter) {
        gAutoClickState.center = !gAutoClickState.center;
        gMouseReport->bits.center = gAutoClickState.center;
    }
}

// --------------------------------------------------------------------------------
// main
// --------------------------------------------------------------------------------
// extern uint32_t SystemCoreClock;
#define SystemCoreClock (96*1000*1000U)
#define SYSTICK_HZ (1000U)
#define MS_TO_TICK(ms) ((ms) * (SystemCoreClock / SYSTICK_HZ))
volatile uint32_t gTick = 0;
static void Systick_Init(void);

MyTimerStruct tasks[] = {
    {
        .period = 100,
        .callback = UartTimer,
        .userdata = NULL,
    },
    {
        .period = 10,
        .callback = Paw3205Timer,
        .userdata = NULL,
    },
    {
        .period = 10,
        .callback = HwTimer,
        .userdata = &tasks[4],
    },
    {
        .period = 1,
        .callback = UsbTimer,
        .userdata = NULL,
    },
    {
        .period = 100,
        .callback = AutoClickTimer,
        .userdata = NULL,
    },
    {
        .period = 1000,
        .callback = Paw3205SyncTimer,
        .userdata = NULL
    }
};

int main(void) {
    UartPrintf_Init();
    Paw3205_Init();
    MouseUsb_Init();
    MouseButton_Init();
    MouseEncoder_Init();

    MyTimer_Reset(tasks, sizeof(tasks) / sizeof(MyTimerStruct));
    for (;;) {
        uint32_t t = gTick;
        gTick = 0;
        MyTimer_Tick(tasks, sizeof(tasks) / sizeof(MyTimerStruct), t);
    }
}

// --------------------------------------------------------------------------------
// isr
// --------------------------------------------------------------------------------
static void Systick_Init(void) {
    SysTick_Config(MS_TO_TICK(SYSTICK_HZ));
    nvic_irq_enable(SysTick_IRQn, 0, 0);
}

void SysTick_Handler(void) {
    ++gTick;
}