#ifndef PROJECTDEFINES_H_
#define PROJECTDEFINES_H_

#include <avr/io.h>

/*
 * Projektet kræver ATmega328PB: UART1/USART1 og TWI0-TWBR0 registrene
 * findes ikke på ATmega328P.
 * XC8: -mcpu=ATmega328PB   avr-gcc: -mmcu=atmega328pb
 */
#if !defined(__AVR_ATmega328PB__) && !defined(__ATmega328PB__)
#error "Vælg ATmega328PB som mål (-mmcu=atmega328pb / -mcpu=ATmega328PB); ATmega328P har ingen UART1."
#endif

#if defined (_AVR_IOM168_H_) || defined (_AVR_IOM328P_H_)
#define USR0_Vect_Num USART_RX_vect
#endif

#if defined (_AVR_IOM2560_H_) || defined (_AVR_ATMEGA328PB_H_INCLUDED)
#define USR0_Vect_Num USART0_RX_vect
#endif

#if defined(_AVR_IOM2560_H_) || defined (_AVR_ATMEGA328PB_H_INCLUDED)
#define GreenLedBit 1
#define YellowLedBit 2
#define RedLedBit 3
#endif

typedef void (*Function_Pointer_With_One_CharPointer_Parameter)(char *);

/*
 * Xplained Mini leverer 16 MHz ved 5 V, men 8 MHz ved 3,3 V.
 * Dette board måler 8 MHz. Kan overskrives med -DF_CPU=16000000UL.
 */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#endif /* PROJECTDEFINES_H_ */
