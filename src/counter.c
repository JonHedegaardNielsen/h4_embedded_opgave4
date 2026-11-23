#include "../include/ProjectDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../include/counter.h"
#include "../include/uart.h"

/*
 * Timer1 kører med prescaler 1024 -> 61 us pr. tick.
 * 330 ticks svarer til ca. 20 ms og bruges som debounce-vindue.
 */
#define BUTTON_DEBOUNCE_TICKS 330UL

static volatile uint16_t ext_interrupt_count = 0;
static volatile uint8_t display_update_pending = 0;
static volatile uint16_t last_edge_ticks = (uint16_t)(0UL - BUTTON_DEBOUNCE_TICKS);

uint16_t counter_get_value(void)
{
    return ext_interrupt_count;
}

uint8_t counter_display_pending(void)
{
    return display_update_pending;
}

void counter_clear_display_pending(void)
{
    display_update_pending = 0;
}

void setupCounter(void)
{
    /* PD2/INT0 som input med intern pull-up: knappen trækker til GND. */
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    /* Start med interrupt på nedadgående flanke (ISC01=1, ISC00=0). */
    EICRA &= ~((1 << ISC01) | (1 << ISC00));
    EICRA |= (1 << ISC01);

    EIFR |= (1 << INTF0); /* ryd en gammel hændelse */
    EIMSK |= (1 << INT0); /* aktiver INT0 */
}

ISR(INT0_vect)
{
    uint16_t now = TCNT1;

    /* Ignorer flanker der ligger for tæt på den foregående (kontaktprell). */
    if ((uint16_t)(now - last_edge_ticks) >= BUTTON_DEBOUNCE_TICKS) {
        last_edge_ticks = now;

        if (EICRA & (1 << ISC00)) {
            /* Opadgående flanke (knappen slippes): skift tilbage, tæl ikke med. */
            EICRA &= ~(1 << ISC00);
        } else {
            /* Nedadgående flanke (knappen trykkes): ét gyldigt tryk. */
            ext_interrupt_count++;

            uart0_puts("Eksterne interrupts: ");
            uart0_put_uint(ext_interrupt_count);
            uart0_puts("\r\n");

            uart1_puts("Eksterne interrupts: ");
            uart1_put_uint(ext_interrupt_count);
            uart1_puts("\r\n");

            display_update_pending = 1;

            /* Næste gyldige interrupt skal være på den opadgående flanke. */
            EICRA |= (1 << ISC00);
        }
    }

    /*
     * Ryd flags der måtte være sat af flanker under skiftet af ISC00,
     * så vi ikke får et ekstra (forkert) kald til ISR'en.
     */
    EIFR |= (1 << INTF0);
}
