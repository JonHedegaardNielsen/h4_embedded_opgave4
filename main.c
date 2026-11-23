#include "include/ProjectDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "include/counter.h"
#include "include/display.h"
#include "include/timer.h"
#include "include/uart.h"

int main(void)
{
    uart0_init();
    uart1_init();
    timer1_init();
    setupCounter();
    display_init();
    display_show_count(0);

    uart0_puts("Klar - tryk pa knappen (PD2/INT0)\r\n");
    uart1_puts("Klar - sekunder sendes hvert sekund\r\n");

    sei();

    while (1)
    {
        if (counter_display_pending())
        {
            counter_clear_display_pending();
            display_show_count(counter_get_value());
        }
    }

    return 0;
}
