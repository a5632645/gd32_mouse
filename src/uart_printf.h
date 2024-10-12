#pragma
#include <stdint.h>

void UartPrintf_Init();
void UartPrintf_Puts(const char* str);
void UartPrintf_PrintNum(uint32_t num, uint8_t newline);
void UartPrintf_PrintHex(uint32_t num, uint8_t newline);