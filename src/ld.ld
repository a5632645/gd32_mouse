/* linker_script.ld */

MEMORY
{
    /* 定义内存区域 */
    FLASH (rx)   : ORIGIN = 0x08000000, LENGTH = 64K  /* Flash memory */
    RAM (rw)     : ORIGIN = 0x20000000, LENGTH = 20K   /* SRAM memory */
}

/* 定义段 */
SECTIONS
{
    /* 启动代码段 */
    .text : {
        KEEP(*(.isr_vector))   /* 保留中断向量表 */
        *(.text*)               /* 所有代码 */
        _etext = .;             /* 代码段结束位置 */
    } > FLASH

    /* 初始化数据段 */
    .rodata : { 
        *(.rodata*)             /* 只读数据 */
    } > FLASH

    _sidata = LOADADDR(.data);
    /* 数据段 */
    .data : {
        _sdata = .;             /* 数据段开始位置 */
        *(.data*)               /* 所有初始化过的数据 */
        _edata = .;             /* 数据段结束位置 */
    } > RAM AT> FLASH

    /* BSS 段（未初始化数据） */
    .bss : {
        _sbss = .;              /* BSS 段开始位置 */
        *(.bss*)                /* 所有未初始化的数据 */
        _ebss = .;              /* BSS 段结束位置 */
    } > RAM

    /* 堆栈 */
    .stack (NOLOAD) : {
        _estack = ORIGIN(RAM) + LENGTH(RAM); /* 堆栈结束位置 */
        . = . + 0x400; /* 分配 1KB 堆栈空间 */
    } > RAM
}