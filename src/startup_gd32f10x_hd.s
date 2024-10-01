.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global g_pfnVectors
.global Default_Handler

// 初始化数据在flash的起始位置
.word _sidata
// 初始化数据在ram的起始位置
.word _sdata
// 初始化数据在ram的结束位置
.word _edata
// 未初始化数据(填充0)在ram的起始位置
.word _sbss
// 未初始化数据(填充0)在ram的结束位置
.word _ebss

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */
/* Call the clock system initialization function.*/
  bl  SystemInit

// 牛皮，加载段还要自己写
/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call static constructors */
  bl __libc_init_array
/* Call the application's entry point.*/
  bl main
  b .

  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object

g_pfnVectors:
    .word _estack              // Top of Stack
    .word Reset_Handler              // Reset Handler
    .word NMI_Handler                // NMI Handler
    .word HardFault_Handler          // Hard Fault Handler
    .word MemManage_Handler          // MPU Fault Handler
    .word BusFault_Handler           // Bus Fault Handler
    .word UsageFault_Handler         // Usage Fault Handler
    .word 0                          // Reserved
    .word 0                          // Reserved
    .word 0                          // Reserved
    .word 0                          // Reserved
    .word SVC_Handler                // SVCall Handler
    .word DebugMon_Handler           // Debug Monitor Handler
    .word 0                          // Reserved
    .word PendSV_Handler             // PendSV Handler
    .word SysTick_Handler            // SysTick Handler

    // External interrupts handlers
    .word     WWDGT_IRQHandler                  // 16:Window Watchdog Timer
    .word     LVD_IRQHandler                    // 17:LVD through EXTI Line detect
    .word     TAMPER_IRQHandler                 // 18:Tamper Interrupt   
    .word     RTC_IRQHandler                    // 19:RTC through EXTI Line
    .word     FMC_IRQHandler                    // 20:FMC
    .word     RCU_IRQHandler                    // 21:RCU
    .word     EXTI0_IRQHandler                  // 22:EXTI Line 0
    .word     EXTI1_IRQHandler                  // 23:EXTI Line 1
    .word     EXTI2_IRQHandler                  // 24:EXTI Line 2
    .word     EXTI3_IRQHandler                  // 25:EXTI Line 3
    .word     EXTI4_IRQHandler                  // 26:EXTI Line 4
    .word     DMA0_Channel0_IRQHandler          // 27:DMA0 Channel 0
    .word     DMA0_Channel1_IRQHandler          // 28:DMA0 Channel 1
    .word     DMA0_Channel2_IRQHandler          // 29:DMA0 Channel 2
    .word     DMA0_Channel3_IRQHandler          // 30:DMA0 Channel 3
    .word     DMA0_Channel4_IRQHandler          // 31:DMA0 Channel 4
    .word     DMA0_Channel5_IRQHandler          // 32:DMA0 Channel 5 
    .word     DMA0_Channel6_IRQHandler          // 33:DMA0 Channel 6
    .word     ADC0_1_IRQHandler                 // 34:ADC0 and ADC1
    .word     USBD_HP_CAN0_TX_IRQHandler        // 35:USBD and CAN0 TX
    .word     USBD_LP_CAN0_RX0_IRQHandler       // 36:USBD and CAN0 RX0
    .word     CAN0_RX1_IRQHandler               // 37:CAN0 RX1
    .word     CAN0_EWMC_IRQHandler              // 38:CAN0 EWMC
    .word     EXTI5_9_IRQHandler                // 39:EXTI Line 5 to EXTI Line 9
    .word     TIMER0_BRK_IRQHandler             // 40:TIMER0 Break
    .word     TIMER0_UP_IRQHandler              // 41:TIMER0 Update
    .word     TIMER0_TRG_CMT_IRQHandler         // 42:TIMER0 Trigger and Commutation
    .word     TIMER0_Channel_IRQHandler         // 43:TIMER0 Channel Capture Compare
    .word     TIMER1_IRQHandler                 // 44:TIMER1
    .word     TIMER2_IRQHandler                 // 45:TIMER2
    .word     TIMER3_IRQHandler                 // 46:TIMER3
    .word     I2C0_EV_IRQHandler                // 47:I2C0 Event
    .word     I2C0_ER_IRQHandler                // 48:I2C0 Error
    .word     I2C1_EV_IRQHandler                // 49:I2C1 Event
    .word     I2C1_ER_IRQHandler                // 50:I2C1 Error
    .word     SPI0_IRQHandler                   // 51:SPI0
    .word     SPI1_IRQHandler                   // 52:SPI1
    .word     USART0_IRQHandler                 // 53:USART0
    .word     USART1_IRQHandler                 // 54:USART1
    .word     USART2_IRQHandler                 // 55:USART2
    .word     EXTI10_15_IRQHandler              // 56:EXTI Line 10 to EXTI Line 15
    .word     RTC_Alarm_IRQHandler              // 57:RTC Alarm through EXTI Line
    .word     USBD_WKUP_IRQHandler              // 58:USBD WakeUp from suspend through EXTI Line
    .word     TIMER7_BRK_IRQHandler             // 59:TIMER7 Break Interrupt
    .word     TIMER7_UP_IRQHandler              // 60:TIMER7 Update Interrupt
    .word     TIMER7_TRG_CMT_IRQHandler         // 61:TIMER7 Trigger and Commutation Interrupt
    .word     TIMER7_Channel_IRQHandler         // 62:TIMER7 Channel Capture Compare 
    .word     ADC2_IRQHandler                   // 63:ADC2
    .word     EXMC_IRQHandler                   // 64:EXMC
    .word     SDIO_IRQHandler                   // 65:SDIO
    .word     TIMER4_IRQHandler                 // 66:TIMER4
    .word     SPI2_IRQHandler                   // 67:SPI2
    .word     UART3_IRQHandler                  // 68:UART3
    .word     UART4_IRQHandler                  // 69:UART4
    .word     TIMER5_IRQHandler                 // 70:TIMER5
    .word     TIMER6_IRQHandler                 // 71:TIMER6
    .word     DMA1_Channel0_IRQHandler          // 72:DMA1 Channel0
    .word     DMA1_Channel1_IRQHandler          // 73:DMA1 Channel1
    .word     DMA1_Channel2_IRQHandler          // 74:DMA1 Channel2
    .word     DMA1_Channel3_4_IRQHandler        // 75:DMA1 Channel3 and Channel4
  .size g_pfnVectors, .-g_pfnVectors

.type NMI_Handler, %function
NMI_Handler:
    .weak NMI_Handler
    b .

.type HardFault_Handler, %function
HardFault_Handler:
    .weak HardFault_Handler
    b .

.type MemManage_Handler, %function
MemManage_Handler:
    .weak MemManage_Handler
    b .

.type BusFault_Handler, %function
BusFault_Handler:
    .weak BusFault_Handler
    b .

.type UsageFault_Handler, %function
UsageFault_Handler:
    .weak UsageFault_Handler
    b .

.type SVC_Handler, %function
SVC_Handler:
    .weak SVC_Handler
    b .

.type DebugMon_Handler, %function
DebugMon_Handler:
    .weak DebugMon_Handler
    b .

.type PendSV_Handler, %function
PendSV_Handler:
    .weak PendSV_Handler
    b .

.type SysTick_Handler, %function
SysTick_Handler:
    .weak SysTick_Handler
    b .

.type Default_Handler, %function
Default_Handler:
    .weak Default_Handler
    b .

// weak外设中断
.type WWDGT_IRQHandler, %function
WWDGT_IRQHandler:
    .weak WWDGT_IRQHandler
    b .
    
.type LVD_IRQHandler, %function
LVD_IRQHandler:
    .weak LVD_IRQHandler
    b .
    
.type TAMPER_IRQHandler, %function
TAMPER_IRQHandler:
    .weak TAMPER_IRQHandler
    b .
    
.type RTC_IRQHandler, %function
RTC_IRQHandler:
    .weak RTC_IRQHandler
    b .
    
.type FMC_IRQHandler, %function
FMC_IRQHandler:
    .weak FMC_IRQHandler
    b .
    
.type RCU_IRQHandler, %function
RCU_IRQHandler:
    .weak RCU_IRQHandler
    b .
    
.type EXTI0_IRQHandler, %function
EXTI0_IRQHandler:
    .weak EXTI0_IRQHandler
    b .
    
.type EXTI1_IRQHandler, %function
EXTI1_IRQHandler:
    .weak EXTI1_IRQHandler
    b .
    
.type EXTI2_IRQHandler, %function
EXTI2_IRQHandler:
    .weak EXTI2_IRQHandler
    b .
    
.type EXTI3_IRQHandler, %function
EXTI3_IRQHandler:
    .weak EXTI3_IRQHandler
    b .
    
.type EXTI4_IRQHandler, %function
EXTI4_IRQHandler:
    .weak EXTI4_IRQHandler
    b .
    
.type DMA0_Channel0_IRQHandler, %function
DMA0_Channel0_IRQHandler:
    .weak DMA0_Channel0_IRQHandler
    b .
    
.type DMA0_Channel1_IRQHandler, %function
DMA0_Channel1_IRQHandler:
    .weak DMA0_Channel1_IRQHandler
    b .
    
.type DMA0_Channel2_IRQHandler, %function
DMA0_Channel2_IRQHandler:
    .weak DMA0_Channel2_IRQHandler
    b .
    
.type DMA0_Channel3_IRQHandler, %function
DMA0_Channel3_IRQHandler:
    .weak DMA0_Channel3_IRQHandler
    b .
    
.type DMA0_Channel4_IRQHandler, %function
DMA0_Channel4_IRQHandler:
    .weak DMA0_Channel4_IRQHandler
    b .
    
.type DMA0_Channel5_IRQHandler, %function
DMA0_Channel5_IRQHandler:
    .weak DMA0_Channel5_IRQHandler
    b .
    
.type DMA0_Channel6_IRQHandler, %function
DMA0_Channel6_IRQHandler:
    .weak DMA0_Channel6_IRQHandler
    b .
    
.type ADC0_1_IRQHandler, %function
ADC0_1_IRQHandler:
    .weak ADC0_1_IRQHandler
    b .
    
.type USBD_HP_CAN0_TX_IRQHandler, %function
USBD_HP_CAN0_TX_IRQHandler:
    .weak USBD_HP_CAN0_TX_IRQHandler
    b .
    
.type USBD_LP_CAN0_RX0_IRQHandler, %function
USBD_LP_CAN0_RX0_IRQHandler:
    .weak USBD_LP_CAN0_RX0_IRQHandler
    b .
    
.type CAN0_RX1_IRQHandler, %function
CAN0_RX1_IRQHandler:
    .weak CAN0_RX1_IRQHandler
    b .
    
.type CAN0_EWMC_IRQHandler, %function
CAN0_EWMC_IRQHandler:
    .weak CAN0_EWMC_IRQHandler
    b .
    
.type EXTI5_9_IRQHandler, %function
EXTI5_9_IRQHandler:
    .weak EXTI5_9_IRQHandler
    b .
    
.type TIMER0_BRK_IRQHandler, %function
TIMER0_BRK_IRQHandler:
    .weak TIMER0_BRK_IRQHandler
    b .
    
.type TIMER0_UP_IRQHandler, %function
TIMER0_UP_IRQHandler:
    .weak TIMER0_UP_IRQHandler
    b .
    
.type TIMER0_TRG_CMT_IRQHandler, %function
TIMER0_TRG_CMT_IRQHandler:
    .weak TIMER0_TRG_CMT_IRQHandler
    b .
    
.type TIMER0_Channel_IRQHandler, %function
TIMER0_Channel_IRQHandler:
    .weak TIMER0_Channel_IRQHandler
    b .
    
.type TIMER1_IRQHandler, %function
TIMER1_IRQHandler:
    .weak TIMER1_IRQHandler
    b .
    
.type TIMER2_IRQHandler, %function
TIMER2_IRQHandler:
    .weak TIMER2_IRQHandler
    b .
    
.type TIMER3_IRQHandler, %function
TIMER3_IRQHandler:
    .weak TIMER3_IRQHandler
    b .
    
.type I2C0_EV_IRQHandler, %function
I2C0_EV_IRQHandler:
    .weak I2C0_EV_IRQHandler
    b .
    
.type I2C0_ER_IRQHandler, %function
I2C0_ER_IRQHandler:
    .weak I2C0_ER_IRQHandler
    b .
    
.type I2C1_EV_IRQHandler, %function
I2C1_EV_IRQHandler:
    .weak I2C1_EV_IRQHandler
    b .
    
.type I2C1_ER_IRQHandler, %function
I2C1_ER_IRQHandler:
    .weak I2C1_ER_IRQHandler
    b .
    
.type SPI0_IRQHandler, %function
SPI0_IRQHandler:
    .weak SPI0_IRQHandler
    b .
    
.type SPI1_IRQHandler, %function
SPI1_IRQHandler:
    .weak SPI1_IRQHandler
    b .
    
.type USART0_IRQHandler, %function
USART0_IRQHandler:
    .weak USART0_IRQHandler
    b .
    
.type USART1_IRQHandler, %function
USART1_IRQHandler:
    .weak USART1_IRQHandler
    b .
    
.type USART2_IRQHandler, %function
USART2_IRQHandler:
    .weak USART2_IRQHandler
    b .
    
.type EXTI10_15_IRQHandler, %function
EXTI10_15_IRQHandler:
    .weak EXTI10_15_IRQHandler
    b .
    
.type RTC_Alarm_IRQHandler, %function
RTC_Alarm_IRQHandler:
    .weak RTC_Alarm_IRQHandler
    b .
    
.type USBD_WKUP_IRQHandler, %function
USBD_WKUP_IRQHandler:
    .weak USBD_WKUP_IRQHandler
    b .
    
.type TIMER7_BRK_IRQHandler, %function
TIMER7_BRK_IRQHandler:
    .weak TIMER7_BRK_IRQHandler
    b .
    
.type TIMER7_UP_IRQHandler, %function
TIMER7_UP_IRQHandler:
    .weak TIMER7_UP_IRQHandler
    b .
    
.type TIMER7_TRG_CMT_IRQHandler, %function
TIMER7_TRG_CMT_IRQHandler:
    .weak TIMER7_TRG_CMT_IRQHandler
    b .
    
.type TIMER7_Channel_IRQHandler, %function
TIMER7_Channel_IRQHandler:
    .weak TIMER7_Channel_IRQHandler
    b .
    
.type ADC2_IRQHandler, %function
ADC2_IRQHandler:
    .weak ADC2_IRQHandler
    b .
    
.type EXMC_IRQHandler, %function
EXMC_IRQHandler:
    .weak EXMC_IRQHandler
    b .
    
.type SDIO_IRQHandler, %function
SDIO_IRQHandler:
    .weak SDIO_IRQHandler
    b .
    
.type TIMER4_IRQHandler, %function
TIMER4_IRQHandler:
    .weak TIMER4_IRQHandler
    b .
    
.type SPI2_IRQHandler, %function
SPI2_IRQHandler:
    .weak SPI2_IRQHandler
    b .
    
.type UART3_IRQHandler, %function
UART3_IRQHandler:
    .weak UART3_IRQHandler
    b .
    
.type UART4_IRQHandler, %function
UART4_IRQHandler:
    .weak UART4_IRQHandler
    b .
    
.type TIMER5_IRQHandler, %function
TIMER5_IRQHandler:
    .weak TIMER5_IRQHandler
    b .
    
.type TIMER6_IRQHandler, %function
TIMER6_IRQHandler:
    .weak TIMER6_IRQHandler
    b .
    
.type DMA1_Channel0_IRQHandler, %function
DMA1_Channel0_IRQHandler:
    .weak DMA1_Channel0_IRQHandler
    b .
    
.type DMA1_Channel1_IRQHandler, %function
DMA1_Channel1_IRQHandler:
    .weak DMA1_Channel1_IRQHandler
    b .
    
.type DMA1_Channel2_IRQHandler, %function
DMA1_Channel2_IRQHandler:
    .weak DMA1_Channel2_IRQHandler
    b .
    
.type DMA1_Channel3_4_IRQHandler, %function
DMA1_Channel3_4_IRQHandler:
    .weak DMA1_Channel3_4_IRQHandler
    b .
    

.align

.end
