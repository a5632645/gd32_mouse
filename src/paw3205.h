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
    uint8_t dx;
    uint8_t dy;
    struct {
        uint8_t resolution : 3;
        uint8_t dxOverflow : 1;
        uint8_t dyOverflow : 1;
        uint8_t reserved : 2;
        uint8_t motion : 1;
    } motionStatus;
} MotionStruct;

void Paw3205_Init(void);
/**
 * @return 0 error, other ok
 */
uint8_t Paw3205_GetMotion(MotionStruct* motion);

/**
 * @brief 尝试同步传感器
 * @param  
 * @return 1 ok, 0 error
 */
uint8_t Paw3205_TrySync(void);

/**
 * @brief 重置传感器，不改变寄存器内容
 * @param  
 */
void Paw3205_ResetChip(void);

/**
 * @brief  通过motion引脚检测是否有移动
 * @param  
 * @return 
 */
uint8_t Paw3205_HasMotion(void); 

/**
 * @brief 设置 CPI
 * @param cpi
 */
void Paw3205_SetCPI(Paw3205CPIEnum cpi);

/**
 * @brief 打印传感器寄存器
 * @param  
 */
void Paw3205_DumpReg(void);

/**
 * @brief 补码到源码转换 
 * @param code 
 * @return 
 */
int8_t Paw3205_NumConvert(uint8_t code);