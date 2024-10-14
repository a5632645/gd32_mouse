#pragma
#include <stdint.h>

void UartPrintf_Init();
void UartPrintf_Puts(const char* str);
void UartPrintf_PrintNum(uint32_t num, uint8_t newline);
void UartPrintf_PrintHex(uint32_t num, uint8_t newline);

typedef enum {
    eLogLevel_Info = 0,
    eLogLevel_Warning,
    eLogLevel_Error,
} LogLevelEnum;
void UartPrintf_Log(
    LogLevelEnum level,
    const char* str,
    const char* file,
    uint32_t line);
#define MY_LOG(level, str) UartPrintf_Log(level, str, __FILE__, __LINE__)
#define MY_LOG_INFO(str) UartPrintf_Log(eLogLevel_Info, str, __FILE__, __LINE__)