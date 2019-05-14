#ifndef UART_328P_H
#define UART_328P_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>


void init_UART();
int uart_putchar( char c, FILE *stream);


#endif
