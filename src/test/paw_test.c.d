#include "../paw3205.h"
#include "../uart_printf.h"
#include "../delay.h"

void main(void) {
    Delay_Init();
    UartPrintf_Init();
    Paw3205_Init();

    while(1) {
        Paw3205_DumpReg();
        Delay_Ms(8);
    }
}