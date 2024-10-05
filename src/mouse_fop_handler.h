#pragma once
#include "mouse_usb.h"

typedef struct {
    struct {
        uint8_t left : 1;
        uint8_t right : 1;
        uint8_t center : 1;
        uint8_t reserve : 1; // 始终为1
        uint8_t dxSign : 1;
        uint8_t dySign : 1;
        uint8_t dxOverflow : 1;
        uint8_t dyOverflow : 1;
    } bits;

    uint8_t dx;
    uint8_t dy;
    int8_t wheel; 
} MoudeReportStruct;

void MouseUsb_Init(void);
void MouseUsb_Send(MoudeReportStruct* report);
void MouseUsb_ResetReport(MoudeReportStruct* report);
uint8_t MouseUsb_IsReady(void);