#pragma once
#include <stdint.h>

typedef enum {
    ePaw3205CPI_400 = 0b000,
    ePaw3205CPI_500 = 0b001,
    ePaw3205CPI_600 = 0b010,
    ePaw3205CPI_800 = 0b011,
    ePaw3205CPI_1000 = 0b100,
    ePaw3205CPI_1200 = 0b101,
    ePaw3205CPI_1600 = 0b110,
} Paw3205CPIEnum;

typedef struct {
    int8_t dx;
    int8_t dy;
    struct {
        uint8_t motion : 1;
        uint8_t reserved : 2;
        uint8_t dyOverflow : 1;
        uint8_t dxOverflow : 1;
        uint8_t resolution : 3;
    } motionStatus;
} MotionStruct;

void Paw3205_Init(void);
void Paw3205_GetMotion(MotionStruct* motion);
bool Paw3205_TrySync(void);
void Paw3205_ResetChip(void);
bool Paw3205_HasMotion(void);
void Paw3205_SetCPI(Paw3205CPIEnum cpi);