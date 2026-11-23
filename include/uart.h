#ifndef UART_H
#define UART_H

#include <stdint.h>

/* UART0 er forbundet til CP2102 (USB). UART1 bruges til sekund-værdier. */
void uart0_init(void);
void uart1_init(void);

void uart0_putchar(char c);
void uart1_putchar(char c);
void uart0_puts(const char *s);
void uart1_puts(const char *s);
void uart0_put_uint(uint16_t value);
void uart1_put_uint(uint16_t value);

#endif
