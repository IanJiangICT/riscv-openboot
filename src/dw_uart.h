#ifndef DW_UART_H
#define DW_UART_H
void dw_uart_init(volatile unsigned char *uart_base, unsigned int baud);
void dw_uart_put_byte(volatile unsigned char *uart_base, unsigned char data);
#endif
