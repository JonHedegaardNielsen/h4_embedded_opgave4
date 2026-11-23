#include "../include/ProjectDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../include/timer.h"
#include "../include/uart.h"

/* 16 MHz / 1024 = 15625 ticks/sekund -> CTC-værdi 15624 giver 1 Hz. */
#define TIMER1_COMPARE_VALUE 15624UL

static volatile uint16_t seconds = 0;

void timer1_init(void)
{
    TCCR1A = 0x00;              /* CTC-tilstand (WGM12 sættes i TCCR1B) */
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); /* prescaler 1024 */
    OCR1A = TIMER1_COMPARE_VALUE;
    TCNT1 = 0;
    TIMSK1 |= (1 << OCIE1A);    /* interrupt ved compare-match på OCR1A */
}

uint16_t timer1_get_seconds(void)
{
    return seconds;
}

ISR(TIMER1_COMPA_vect)
{
    seconds++;

    uart1_puts("sekund: ");
    uart1_put_uint(seconds);
    uart1_puts("\r\n");
}
