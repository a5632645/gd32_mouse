#include <stdlib.h>
#include <stdio.h>
#include "mouse_fop_handler.h"
#include "mouse_button.h"
#include "mouse_encoder.h"
#include "paw3205.h"
#include "uart_printf.h"
#include "leds.h"
#include "util/my_timer.h"
#include "util/my_button.h"
#include "gd32f10x_wwdgt.h"

volatile int32_t ITM_RxBuffer;

// ---------------------------------------- 按键联合 ----------------------------------------
typedef struct {
    uint8_t autoclick_press : 1;
    uint8_t autoclickEnable : 1;
    uint8_t normal_press : 1;
    uint8_t autopress_press : 1;
} ButtonMixStruct;

uint8_t ButtonMix_IsPressed(ButtonMixStruct* state) {
    if (state->autoclick_press) {
        return 1;
    }
    if (state->normal_press) {
        return 1;
    }
    if (state->autoclickEnable) {
        return state->autoclick_press;
    }
    else {
        return state->normal_press;
    }
}

static ButtonMixStruct gLeftBtnMix = {};
static ButtonMixStruct gRightBtnMix = {};
static ButtonMixStruct gCenterBtnMix = {};

// ---------------------------------------- usb ----------------------------------------
static MoudeReportStruct gMouseReport = {};
static void UsbTimer(uint32_t escape, void* userdata) {
    if (!MouseUsb_IsReady()) {
        return;
    }
    gMouseReport.bits.left = ButtonMix_IsPressed(&gLeftBtnMix);
    gMouseReport.bits.right = ButtonMix_IsPressed(&gRightBtnMix);
    gMouseReport.bits.center = ButtonMix_IsPressed(&gCenterBtnMix);
    MouseUsb_Send(&gMouseReport);
    MouseUsb_ResetReport(&gMouseReport);
    UartPrintf_Puts("[usb] send mouse report\n");
}

// ---------------------------------------- paw3205 ----------------------------------------
static MotionStruct gMotionStruct;
static void Paw3205Timer(uint32_t escape, void* userdata) {
    if(Paw3205_GetMotion(&gMotionStruct) == 0) {
        return;
    }
    if (!gMotionStruct.motionStatus.motion) {
        return;
    }

    gMouseReport.dx = gMotionStruct.dx & 0x7f;
    gMouseReport.dy = gMotionStruct.dy & 0x7f;
    gMouseReport.bits.dxSign = gMotionStruct.dx >> 7;
    gMouseReport.bits.dySign = gMotionStruct.dy >> 7;
    gMouseReport.bits.dxOverflow = gMotionStruct.motionStatus.dxOverflow;
    gMouseReport.bits.dyOverflow = gMotionStruct.motionStatus.dyOverflow;
}

// ---------------------------------------- 硬件 ----------------------------------------
static struct {
    uint8_t autoclickLeft : 1;
    uint8_t autoclickRight : 1;
    uint8_t autoclickCenter : 1;
    uint8_t autopressLeft : 1;
    uint8_t autopressRight : 1;
    uint8_t autopressCenter : 1;
    int32_t autoclickRate;
} gAutoParams = {};

static struct {
    MyButtonStruct autoclick;
    MyButtonStruct autopress;
    MyButtonStruct left;
    MyButtonStruct right;
    MyButtonStruct center;
} gButtonState = {};

static void HwTimer(uint32_t escape, void* userdata) {
    uint8_t leftPress = MouseButton_IsPressed(eMouseButtonLeft);
    uint8_t rightPress = MouseButton_IsPressed(eMouseButtonRight);
    uint8_t centerPress = MouseButton_IsPressed(eMouseButtonCenter);
    uint8_t autoclickPress = MouseButton_IsPressed(eMouseButtonAutoClick);
    uint8_t autopressPress = MouseButton_IsPressed(eMouseButtonAutoPress);

    MyButton_Tick(&gButtonState.autoclick, autoclickPress, escape);
    MyButton_Tick(&gButtonState.autopress, autopressPress, escape);
    MyButton_Tick(&gButtonState.left, leftPress, escape);
    MyButton_Tick(&gButtonState.right, rightPress, escape);
    MyButton_Tick(&gButtonState.center, centerPress, escape);

    // 普通赋值
    gLeftBtnMix.normal_press = leftPress;
    gRightBtnMix.normal_press = rightPress;
    gCenterBtnMix.normal_press = centerPress;

    bool ledUpdate = FALSE;
    // 如果按键弹起，清除自动按下
    if (gButtonState.left.state == eButtonState_Release) {
        gAutoParams.autopressLeft = 0;
        ledUpdate = TRUE;
    }
    if (gButtonState.right.state == eButtonState_Release) {
        gAutoParams.autopressRight = 0;
        ledUpdate = TRUE;
    }
    if (gButtonState.center.state == eButtonState_Release) {
        gAutoParams.autopressCenter = 0;
        ledUpdate = TRUE;
    }

    // 计算鼠标轮
    int32_t dWheel = MouseEncoder_Read(eMouseEncoder_Wheel);
    if (dWheel > 0)
        gMouseReport.wheel = 0xff;
    else if (dWheel < 0)
        gMouseReport.wheel = 0x00;
    else
        gMouseReport.wheel = 0x80;

    // 自动点击开关
    if (autoclickPress) {
        if (gButtonState.left.state == eButtonState_Click) {
            gAutoParams.autoclickLeft = !gAutoParams.autoclickLeft;
            gLeftBtnMix.autoclickEnable = gAutoParams.autoclickLeft;
            ledUpdate = TRUE;
            UartPrintf_Puts("autoclickLeft: ");UartPrintf_PrintNum(gAutoParams.autoclickLeft, 1);
        }
        if (gButtonState.right.state == eButtonState_Click) {
            gAutoParams.autoclickRight = !gAutoParams.autoclickRight;
            gRightBtnMix.autoclickEnable = gAutoParams.autoclickRight;
            ledUpdate = TRUE;
            UartPrintf_Puts("autoclickRight: ");UartPrintf_PrintNum(gAutoParams.autoclickRight, 1);
        }
        if (gButtonState.center.state == eButtonState_Click) {
            gAutoParams.autoclickCenter = !gAutoParams.autoclickCenter;
            gCenterBtnMix.autoclickEnable = gAutoParams.autoclickCenter;
            ledUpdate = TRUE;
            UartPrintf_Puts("autoclickCenter: ");UartPrintf_PrintNum(gAutoParams.autoclickCenter, 1);
        }
    }

    // 自动按下开关
    if (autopressPress) {
        if (gButtonState.left.state == eButtonState_Click) {
            gAutoParams.autopressLeft = !gAutoParams.autopressLeft;
            gLeftBtnMix.autopress_press = gAutoParams.autopressLeft;
            ledUpdate = TRUE;
            UartPrintf_Puts("autopressLeft: ");UartPrintf_PrintNum(gAutoParams.autopressLeft, 1);
        }
        if (gButtonState.right.state == eButtonState_Click) {
            gAutoParams.autopressRight = !gAutoParams.autopressRight;
            gRightBtnMix.autopress_press = gAutoParams.autopressRight;
            ledUpdate = TRUE;
            UartPrintf_Puts("autopressRight: ");UartPrintf_PrintNum(gAutoParams.autopressRight, 1);
        }
        if (gButtonState.center.state == eButtonState_Click) {
            gAutoParams.autopressCenter = !gAutoParams.autopressCenter;
            gCenterBtnMix.autopress_press = gAutoParams.autopressCenter;
            ledUpdate = TRUE;
            UartPrintf_Puts("autopressCenter: ");UartPrintf_PrintNum(gAutoParams.autopressCenter, 1);
        }
    }

    // leds
    if (TRUE == ledUpdate) {
        Leds_Set(eLed_APL, gAutoParams.autopressLeft);
        Leds_Set(eLed_APR, gAutoParams.autopressRight);
        Leds_Set(eLed_APC, gAutoParams.autopressCenter);
        Leds_Set(eLed_ACL, gAutoParams.autoclickLeft);
        Leds_Set(eLed_ACR, gAutoParams.autoclickRight);
        Leds_Set(eLed_ACC, gAutoParams.autoclickCenter);
        Led_Update();
    }

    MyTimerStruct* autoclickTask = (MyTimerStruct*)userdata;
    gAutoParams.autoclickRate += MouseEncoder_Read(eMouseEncoder_ClickRate);
    if (gAutoParams.autoclickRate < 1)
        gAutoParams.autoclickRate = 1;
    autoclickTask->period = gAutoParams.autoclickRate;
}

// ---------------------------------------- 自动点击 ----------------------------------------
static struct {
    uint8_t left : 1;
    uint8_t right : 1;
    uint8_t center : 1;
} gAutoClickState = {};
static void AutoClickTimer(uint32_t escape, void* userdata) {
    if (gAutoParams.autoclickLeft) {
        gAutoClickState.left = !gAutoClickState.left;
        gLeftBtnMix.autoclick_press = gAutoClickState.left;
    }
    if (gAutoParams.autoclickRight) {
        gAutoClickState.right = !gAutoClickState.right;
        gRightBtnMix.autoclick_press = gAutoClickState.right;
    }
    if (gAutoParams.autoclickCenter) {
        gAutoClickState.center = !gAutoClickState.center;
        gCenterBtnMix.autoclick_press = gAutoClickState.center;
    }
}

static void UartTimer(uint32_t escape, void* userdata);
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
        .period = 1000,
        .callback = UartTimer,
        .userdata = NULL,
    },
    {
        .period = 10,
        .callback = Paw3205Timer,
        .userdata = NULL,
    },
    {
        .period = 20,
        .callback = HwTimer,
        .userdata = &tasks[4],
    },
    {
        .period = 10,
        .callback = UsbTimer,
        .userdata = NULL,
    },
    {
        .period = 100,
        .callback = AutoClickTimer,
        .userdata = NULL,
    },
};
const uint32_t tasksLen = sizeof(tasks) / sizeof(MyTimerStruct);

// ---------------------------------------- debug ----------------------------------------
static void UartTimer(uint32_t escape, void* userdata) {
    UartPrintf_Puts("click rate: ");UartPrintf_PrintNum(gAutoParams.autoclickRate, 1);
}

int main(void) {
    NVIC_EnableIRQ(MemoryManagement_IRQn);
    NVIC_EnableIRQ(BusFault_IRQn);
    NVIC_EnableIRQ(UsageFault_IRQn);

    Systick_Init();
    
    UartPrintf_Init();

    MY_LOG(eLogLevel_Info, "usb init\n");
    MouseUsb_Init();

    MY_LOG(eLogLevel_Info, "init wwdgt\n");

    MY_LOG(eLogLevel_Info, "paw3205 init\n");
    Paw3205_Init();

    MY_LOG(eLogLevel_Info, "hw timer init\n");
    MouseButton_Init();
    MouseEncoder_Init();

    MY_LOG(eLogLevel_Info, "leds init\n");
    Leds_Init();

    MY_LOG(eLogLevel_Info, "start\n");
    MyTimer_Reset(tasks, tasksLen);
    
    for (;;) {
        while (gTick < 1);
        uint32_t t = gTick;
        gTick = 0;
        MyTimer_Tick(tasks, tasksLen, t);
    }
}

// --------------------------------------------------------------------------------
// isr
// --------------------------------------------------------------------------------
static void Systick_Init(void) {
    SysTick_Config(MS_TO_TICK(1));
    nvic_irq_enable(SysTick_IRQn, 0, 0);
}

void SysTick_Handler(void) {
    ++gTick;
}