#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/* Hardware-TWI0: SDA = PC4, SCL = PC5 (100 kHz ved 16 MHz). */
void i2c_init(void);
void i2c_start(uint8_t address);
void i2c_write(uint8_t data);
void i2c_stop(void);

#endif
