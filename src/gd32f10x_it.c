#include "gd32f10x_it.h"
#include "usbd_lld_int.h"
#include "uart_printf.h"

extern uint32_t _estack;

static void PrintStack(void) {
    UartPrintf_Init();
    uint32_t* sp = 0;
    asm volatile ("mov %0, sp" : "=r" (sp));
    uint32_t* sp_end = &_estack;

    UartPrintf_Puts("Stack:\n");
    for (; sp < sp_end; ++sp) {
        UartPrintf_PrintHex((uint32_t)sp, 0);
        UartPrintf_Puts(": ");
        UartPrintf_PrintHex(*sp, 1);
    }
    UartPrintf_Puts("stack end\n");

    uint32_t val = 0;
    asm volatile ("mov %0, r0" : "=r" (val));
    UartPrintf_Puts("R0: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r1" : "=r" (val));
    UartPrintf_Puts("R1: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r2" : "=r" (val));
    UartPrintf_Puts("R2: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r3" : "=r" (val));
    UartPrintf_Puts("R3: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r4" : "=r" (val));
    UartPrintf_Puts("R4: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r5" : "=r" (val));
    UartPrintf_Puts("R5: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r6" : "=r" (val));
    UartPrintf_Puts("R6: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r7" : "=r" (val));
    UartPrintf_Puts("R7: ");UartPrintf_PrintHex(val, 1);

    asm volatile ("mov %0, r8" : "=r" (val));
    UartPrintf_Puts("R8: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r9" : "=r" (val));
    UartPrintf_Puts("R9: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r10" : "=r" (val));
    UartPrintf_Puts("R10: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r11" : "=r" (val));
    UartPrintf_Puts("R11: ");UartPrintf_PrintHex(val, 1);
    asm volatile ("mov %0, r12" : "=r" (val));
    UartPrintf_Puts("R12: ");UartPrintf_PrintHex(val, 1);

    asm volatile ("mov %0, r15" : "=r" (val)); // PC 通常是 r15
    UartPrintf_Puts("PC: "); UartPrintf_PrintHex(val, 1);
    
    asm volatile ("mov %0, r14" : "=r" (val)); // LR
    UartPrintf_Puts("LR: "); UartPrintf_PrintHex(val, 1);

    UartPrintf_Puts("End of registers\n");
    while (1);
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void) {
    UartPrintf_Init();
    UartPrintf_Puts("HardFault_Handler\n");
    PrintStack();
}

void MemManage_Handler(void)
{
    UartPrintf_Init();
    UartPrintf_Puts("MemManage_Handler\n");
    PrintStack();
}

void BusFault_Handler(void)
{
    UartPrintf_Init();
    UartPrintf_Puts("BusFault_Handler\n");
    PrintStack();
}


void UsageFault_Handler(void)
{
    UartPrintf_Init();
    UartPrintf_Puts("UsageFault_Handler\n");
    PrintStack();
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

// void SysTick_Handler(void)
// {
// }

// 外设中断
void USBD_LP_CAN0_RX0_IRQHandler(void) {
    usbd_isr();
}