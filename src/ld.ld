ENTRY(Reset_Handler)

_Min_Heap_Size = 0x200; /* required amount of heap */
_Min_Stack_Size = 0x400; /* required amount of stack */

MEMORY
{
    /* 定义内存区域 */
    FLASH (rx)   : ORIGIN = 0x08000000, LENGTH = 64K  /* Flash memory */
    RAM (rw)     : ORIGIN = 0x20000000, LENGTH = 20K   /* SRAM memory */
}

/* 定义段 */
SECTIONS
{
    /* 代码 */
    .text : {
        KEEP(*(.isr_vector))   /* 保留中断向量表 */
        *(.text*)               /* 所有代码 */
    } > FLASH

    /* flash里的只读数据 */
    .rodata : { 
        *(.rodata*)             /* 只读数据 */
    } > FLASH

    .ARM.extab   : {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } >FLASH

    .ARM : {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(4);
    } >FLASH

    _sidata = LOADADDR(.data);
    /* 已初始化的数据 */
    .data : {
        _sdata = .;             /* 数据段开始位置 */
        *(.data*)               /* 所有初始化过的数据 */
        _edata = .;             /* 数据段结束位置 */
    } > RAM AT> FLASH

    /* 未初始化数据 */
    .bss : {
        _sbss = .;              /* BSS 段开始位置 */
        *(.bss*)                /* 所有未初始化的数据 */
        _ebss = .;              /* BSS 段结束位置 */
    } > RAM

    /* C++ 初始化段 */
    .preinit_array     :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
    } >FLASH

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
    } >FLASH

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    } >FLASH

    /* 栈 */
    .stack (NOLOAD) : {
        _estack = ORIGIN(RAM) + LENGTH(RAM); /* 堆栈结束位置 */
        . = . + _Min_Stack_Size;
    } > RAM

    /* 堆 */
    .heap (NOLOAD) : {
        . = ALIGN(8);
        ._heap_start = .;
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        ._heap_end = .;
        . = ALIGN(8);
    } >RAM

    /* Remove information from the compiler libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}