#include "uart_printf.h"
#include "gd32f10x_usart.h"
#include "gd32f10x_gpio.h"
#include "gd32f10x_rcu.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

void UartPrintf_Init(void) {
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    /* connect port to USARTx_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}

// https://moker.blog/archives/_close-is-not-implemented-and-will-always-fail#:~:text=#%20include
__attribute__((weak)) int _close(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 0;
 
    errno = EBADF;
    return -1;
}

__attribute__((weak)) int _lseek(int fd, int ptr, int dir)
{
    (void)fd;
    (void)ptr;
    (void)dir;
 
    errno = EBADF;
    return -1;
}

__attribute__((weak)) int _isatty_r(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 1;
 
    errno = EBADF;
    return 0;
}

__attribute__((weak)) int _fstat_r(int fd, struct stat *st)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
    {
        st->st_mode = S_IFCHR;
        return 0;
    }
 
    errno = EBADF;
    return 0;
}

int __io_getchar(void)
{
	uint8_t ch = 0;

	// Clear the Overrun flag just before receiving the first character
    usart_flag_clear(USART0, USART_FLAG_ORERR);
    // 等待RX不为空
    while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE));
	return usart_data_receive(USART0);
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
    (void)file;
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
        *ptr++ = __io_getchar();
    }
    return len;
}

int _write(int file, char *ptr, int len)
{
    while (len--) {
        usart_data_transmit(USART0, *ptr++);
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    }
    return len;
}
