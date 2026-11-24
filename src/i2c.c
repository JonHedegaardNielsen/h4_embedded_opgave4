#include "../include/ProjectDefines.h"

#include <avr/io.h>
#include <stdint.h>

#include "../include/i2c.h"

void i2c_init(void)
{
    /* Træk-op på TWI-benene (SDA0/PC4 og SCL0/PC5). */
    PORTC |= (1 << PC4) | (1 << PC5);

    TWSR0 = 0x00;                                  /* prescaler 1 */
    TWBR0 = (uint8_t)((F_CPU / 100000UL - 16UL) / 2UL); /* ca. 100 kHz */
    TWCR0 = (1 << TWEN);
}

static void i2c_wait_for_complete(void)
{
    /* Begrænset ventetid, så firmwaren ikke hænger hvis displayet mangler. */
    uint16_t timeout = 10000;
    while (!(TWCR0 & (1 << TWINT)) && timeout > 0)
        timeout--;
}

void i2c_start(uint8_t address)
{
    TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    i2c_wait_for_complete();

    TWDR0 = address;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    i2c_wait_for_complete();
}

void i2c_write(uint8_t data)
{
    TWDR0 = data;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    i2c_wait_for_complete();
}

void i2c_stop(void)
{
    TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}
