#include "gd32f10x_it.h"
#include "usbd_lld_int.h"
#include "uart_printf.h"
#include "CMSIS/core_cm3.h"
#include "gd32f10x_wwdgt.h"

#define CrashReport()\
do {\
    uint32_t* sp;\
    asm volatile ("mrs %0, msp" : "=r" (sp));\
    uint32_t lr;\
    asm volatile ("mov %0, lr" : "=r" (lr));\
    uint32_t pc;\
    asm volatile ("mov %0, pc" : "=r" (pc));\
    PrintStack(lr, sp, pc);\
} while (0)

static void PrintStack(uint32_t lr, uint32_t* sp, uint32_t pc) {
    uint32_t* sp_end = (uint32_t*)(*(uint32_t*)SCB->VTOR);
    UartPrintf_Puts("Stack:\n");
    for (; sp < sp_end; ++sp) {
        UartPrintf_PrintHex((uint32_t)sp, 0);
        UartPrintf_Puts(": ");
        UartPrintf_PrintHex(*sp, 1);
    }
    UartPrintf_Puts("End of stack\n");

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

    UartPrintf_Puts("PC: "); UartPrintf_PrintHex(pc, 1);
    UartPrintf_Puts("LR: "); UartPrintf_PrintHex(lr, 1);

    UartPrintf_Puts("End of registers\n");
    while (1);
}

void HardFault_Handler(void) {
    UartPrintf_Init();
    UartPrintf_Puts("HardFault_Handler: ");
    UartPrintf_PrintHex(SCB->CFSR, 1);
    CrashReport();
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
