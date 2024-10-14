#include "gd32f10x_gpio.h"
#include "gd32f10x_usart.h"
#include "gd32f10x_fmc.h"
#include "CMSIS/core_cm3.h"
#include <string.h>

// ---------------------------------------- CIRCLE ----------------------------------------
#define CIRCLE_BUF_SIZE 2048
typedef struct {
    volatile uint8_t circle[CIRCLE_BUF_SIZE];
    volatile uint32_t wp;
    volatile uint32_t rp;
} CircleStruct;

static CircleStruct recvCircle = {
    .wp = 0,
    .rp = 0
};
static CircleStruct sendCircle = {
    .wp = 0,
    .rp = 0
};

inline static uint32_t CircleGetCount(CircleStruct* c) {
    return (c->wp + CIRCLE_BUF_SIZE - c->rp) & (CIRCLE_BUF_SIZE - 1);
}

inline static uint32_t CircleGetSpace(CircleStruct* c) {
    return CIRCLE_BUF_SIZE - CircleGetCount(c);
}

static uint32_t CircleRead(CircleStruct* c, uint8_t* dst, uint32_t size) {
    __disable_irq();
    uint32_t have = CircleGetCount(c);
    __enable_irq();

    if (size > have) {
        size = have;
    }
    uint8_t* src = (uint8_t*)c->circle + c->rp;
    uint8_t* end = (uint8_t*)c->circle + CIRCLE_BUF_SIZE;
    __disable_irq();
    c->rp += size;
    c->rp &= (CIRCLE_BUF_SIZE - 1);
    __enable_irq();
    if (src + size > end) {
        // 分开读
        uint32_t canRead = end - src;
        memcpy(dst, src, canRead);
        src = (uint8_t*)c->circle;
        dst += canRead;
        size -= canRead;
        memcpy(dst, src, size);
    }
    else{
        // 直接读完
        memcpy(dst, src, size);
    }
    return have;
}

static uint8_t CircleReadOne(CircleStruct* c) {
    uint8_t ret = c->circle[c->rp];
    __disable_irq();
    ++c->rp;
    c->rp &= (CIRCLE_BUF_SIZE - 1);
    __enable_irq();
    return ret;
}

static void CircleWrite(CircleStruct* c, uint8_t* src, uint32_t size) {
    uint8_t* dst = (uint8_t*)c->circle + c->wp;
    uint8_t* end = (uint8_t*)c->circle + CIRCLE_BUF_SIZE;
    __disable_irq();
    c->wp += size;
    c->wp &= (CIRCLE_BUF_SIZE - 1);
    __enable_irq();
    if (dst + size > end) {
        uint32_t canWrite = end - dst;
        memcpy(dst, src, canWrite);
        src += canWrite;
        size -= canWrite;
        dst = (uint8_t*)c->circle;
        memcpy(dst, src, size);
    }
    else {
        memcpy(dst, src, size);
    }
}

// ---------------------------------------- print ----------------------------------------
static void Puts(const char* str, uint32_t len) {
    CircleWrite(&sendCircle, (uint8_t*)str, len);
    usart_interrupt_enable(USART0, USART_INT_TBE);
}
void PutHex(uint32_t num, uint8_t newline) {
    char buff[32] = {
        [31] = 0,
        [30] = '\n'
    };
    char* end = buff + 31;
    if (newline) {
        end = buff + 30;
    }

    do {
        *--end = (num % 16) < 10 ? (num % 16) + '0' : (num % 16) - 10 + 'A';
        num /= 16;
    } while (num);
    *--end = 'x';
    *--end = '0';

    Puts(end, buff + 32 - end);
}
#define PUTS(STR) do {\
    Puts(STR, sizeof(STR) - 1);\
} while (0);

// ---------------------------------------- flash ----------------------------------------
extern uint32_t _app_start;
#define FLASH_PAGE_SIZE (2*1024)

#define CLK_HZ (96*1000*1000)
volatile uint32_t gWaitTickLeft = 0;
volatile uint8_t gWaitCompleted = 0;
/**
 * @brief 
 * @param ms 
 * @param predicate 
 * @param arg 
 * @return FALSE: 未完成; TRUE: 完成
 */
static bool TryWaitUntil(uint32_t ms, bool(*predicate)(void*), void* arg) {
    SysTick_Config(CLK_HZ / 250);
    gWaitTickLeft = ms >> 2;
    gWaitCompleted = 0;
    bool ret = FALSE;
    while (1) {
        if (predicate(arg) == TRUE) {
            ret = TRUE;
            break;
        }
        if (gWaitCompleted == 1) {
            ret = FALSE;
            break;
        }
    }
    nvic_irq_disable(SysTick_IRQn);
    SysTick->CTRL = 0;
    return ret;
}

bool WaitFunc(void* arg) {
    if (CircleGetCount(&recvCircle) > 127) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

static void BeginFlash(void) {
    fmc_bank0_unlock();
    PUTS("unlock flash\n");

    __attribute__((aligned(4))) uint8_t buff[256];
    uint32_t writeAdrr = (uint32_t)&_app_start;
    uint32_t eraseAdrr = (uint32_t)&_app_start;    uint8_t* temp = (uint8_t*)buff;
    uint32_t still = 0;
    uint32_t totalRecv = 0;

    while (1) {
        bool retAfterFlash = TryWaitUntil(5000, WaitFunc, NULL);
        uint32_t tmp = CircleRead(&recvCircle, temp, sizeof(buff) - still);
        totalRecv += tmp;
        uint32_t have = still + tmp;
        if (retAfterFlash == FALSE && (have & 1) == 1) {
            buff[have] = 0xff;
            ++have;
        }

        PUTS("recv pack size: ");
        PutHex(have, 1);

        {
            uint32_t writeEnd = have + writeAdrr;
            while (writeEnd >= eraseAdrr) {
                PUTS("erase: ");
                PutHex(eraseAdrr, 1);

                fmc_page_erase(eraseAdrr);
                eraseAdrr += FLASH_PAGE_SIZE;
            }
        }
        {
            uint32_t fourHave = have >> 2;
            uint32_t* p = (uint32_t*)buff;
            for (uint32_t i = 0; i < fourHave; ++i) {
                fmc_word_program(writeAdrr, p[i]);
                writeAdrr += 4;
            }
        }
        {
            uint32_t twoHave = have & 3;
            uint16_t* p = (uint16_t*)buff;
            for (uint32_t i = 0; i < twoHave; ++i) {
                fmc_halfword_program(writeAdrr, p[i]);
                writeAdrr += 2;
            }
        }
        {
            uint32_t oneHave = have & 1;
            if (oneHave !=0) {
                buff[0] = buff[have - 1];
                temp = &buff[1];
                still = 1;
            }
            else {
                temp = buff;
                still = 0;
            }
        }
        if (retAfterFlash == FALSE) {
            break;
        }
    }

    PUTS("lock flash\n");
    fmc_bank0_lock();

    PUTS("recv total size: ");
    PutHex(totalRecv, 1);
}

// ---------------------------------------- main ----------------------------------------
static bool WaitFunc2(void* arg) {
    if (CircleGetCount(&recvCircle) > 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void main(void) {
    rcu_periph_clock_enable(RCU_USART0);
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
    usart_enable(USART0);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    nvic_irq_enable(USART0_IRQn, 0, 0);

    PUTS("app address: ");
    PutHex((uint32_t)&_app_start, 1);
    while (CircleGetCount(&recvCircle) < 1) {
        TryWaitUntil(1000, WaitFunc2, NULL);
        PUTS("*");
    }
    PUTS("\n");

    BeginFlash();

    while (CircleGetCount(&sendCircle) != 0);
    usart_flag_clear(USART0, USART_FLAG_TBE);
    while (usart_flag_get(USART0, USART_FLAG_TBE) == RESET);

    usart_disable(USART0);
    usart_deinit(USART0);

    uint32_t* sp = &_app_start;
    void(*resetHandler)() = (void(*)())(sp + 1);
    asm volatile ("msr msp, %0"::"r"(sp));
    __DSB();
    __ISB();
    __DMB();
    resetHandler();
}

// ---------------------------------------- irq ----------------------------------------
void USART0_IRQHandler(void) {
    if (SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) {
        uint8_t d = usart_data_receive(USART0);
        recvCircle.circle[recvCircle.wp++] = d;
        recvCircle.wp &= (CIRCLE_BUF_SIZE - 1);
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
    }
    if (SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)) {
        if (CircleGetCount(&sendCircle) < 1) {
            usart_interrupt_disable(USART0, USART_INT_TBE);
        }
        else {
            uint8_t d = sendCircle.circle[sendCircle.rp++];
            sendCircle.rp &= (CIRCLE_BUF_SIZE - 1);
            usart_data_transmit(USART0, d);
        }
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_TBE);
    }
}

void SysTick_Handler(void) {
    --gWaitTickLeft;
    if (gWaitTickLeft == 0 || gWaitTickLeft > (uint32_t)(-10000)) {
        gWaitCompleted = 1;
    }
}