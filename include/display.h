#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

/* SSD1306 på I2C-adresse 0x3C, tilsluttet TWI0 (PC4/PC5). */
void display_init(void);
void display_show_count(uint16_t count);

#endif
