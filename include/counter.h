#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

void setupCounter(void);
uint16_t counter_get_value(void);
uint8_t counter_display_pending(void);
void counter_clear_display_pending(void);

#endif
