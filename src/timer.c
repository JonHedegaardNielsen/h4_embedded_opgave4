#include "../include/ProjectDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../include/timer.h"
#include "../include/uart.h"

/*
 * Timer1 i CTC-mode med prescaler 1024.
 * OCR1A beregnes ud fra F_CPU, så der går præcis 1 sekund mellem
 * interrupts - både ved 8 MHz (7812) og 16 MHz (15624).
 */
#define TIMER1_PRESCALER 1024UL
#define TIMER1_COMPARE_VALUE ((F_CPU / TIMER1_PRESCALER) - 1UL)

static volatile uint16_t seconds = 0;

void timer1_init(void)
{
    DDRB |= (1 << PB0);                      // LED på PB0 som output

    TCCR1A = 0x00;                           // CTC mode (WGM12 sættes i TCCR1B)
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);     // 1024 prescaler
    OCR1A = TIMER1_COMPARE_VALUE;             // tid = 1 sekund
    TIMSK1 |= (1 << OCIE1A);                  // interrupt når OCR1A nås
}

uint16_t timer1_get_seconds(void)
{
    return seconds;
}

ISR(TIMER1_COMPA_vect)
{
    seconds++;
    PORTB ^= (1 << PB0);                      // blink LED'en på PB0

    /* UART0 (mEDBG) - så sekunderne kan ses direkte på PC'en. */
    uart0_puts("sekund: ");
    uart0_put_uint(seconds);
    uart0_puts("\r\n");

    /* UART1 (CP2102) - som opgaven kræver. */
    uart1_puts("sekund: ");
    uart1_put_uint(seconds);
    uart1_puts("\r\n");
}
