#include "../include/ProjectDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../include/uart.h"

#define BAUD 9600UL
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1UL)

void uart0_init(void)
{
    /* Asynkron, 8 data bits, 1 stopbit, ingen paritet (8N1). */
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    UCSR0A = 0x00;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    /* RX, TX og receive-interrupt aktiveret. */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

void uart1_init(void)
{
    UBRR1H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR1L = (uint8_t)UBRR_VALUE;
    UCSR1A = 0x00;
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
    UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
}

void uart0_putchar(char c)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = (uint8_t)c;
}

void uart1_putchar(char c)
{
    while (!(UCSR1A & (1 << UDRE1)))
        ;
    UDR1 = (uint8_t)c;
}

void uart0_puts(const char *s)
{
    while (*s)
        uart0_putchar(*s++);
}

void uart1_puts(const char *s)
{
    while (*s)
        uart1_putchar(*s++);
}

void uart0_put_uint(uint16_t value)
{
    char buffer[6]; /* max 5 cifre + terminator */
    uint8_t i = sizeof(buffer) - 1;

    buffer[i] = '\0';
    do {
        buffer[--i] = (char)('0' + (value % 10));
        value /= 10;
    } while (value > 0);

    uart0_puts(&buffer[i]);
}

void uart1_put_uint(uint16_t value)
{
    char buffer[6];
    uint8_t i = sizeof(buffer) - 1;

    buffer[i] = '\0';
    do {
        buffer[--i] = (char)('0' + (value % 10));
        value /= 10;
    } while (value > 0);

    uart1_puts(&buffer[i]);
}

/* UART0: gør alle modtagne bogstaver store og send dem tilbage. */
ISR(USART0_RX_vect)
{
    uint8_t c = UDR0; /* læsning af UDR0 rydder RXC0-flaget */

    if (c >= 'a' && c <= 'z')
        c = (uint8_t)(c - ('a' - 'A'));

    uart0_putchar((char)c);
}

/* UART1: gør alle modtagne bogstaver små og send dem tilbage. */
ISR(USART1_RX_vect)
{
    uint8_t c = UDR1; /* læsning af UDR1 rydder RXC1-flaget */

    if (c >= 'A' && c <= 'Z')
        c = (uint8_t)(c + ('a' - 'A'));

    uart1_putchar((char)c);
}
