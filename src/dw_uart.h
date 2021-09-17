#ifndef DW_UART_H
#define DW_UART_H
void dw_uart_init(volatile void *uart_base, unsigned int baud);
void dw_uart_put_byte(volatile void *uart_base, unsigned char data);
#endif
