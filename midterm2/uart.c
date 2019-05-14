#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

void init_UART(void){
	//Set baud rate
	uint16_t baud_rate = BRGVAL;
	UBRR0H = baud_rate >> 8;
	UBRR0L = baud_rate & 0xFF;
	
	//Enable receiver and transmitter
	UCSR0B = ( 1 <<RXEN0)|( 1 <<TXEN0);
	
	// Set frame format: 8data, 1stop bit
	UCSR0C = (3 <<UCSZ00);
}

int uart_putchar(char c, FILE *stream){
	//wait until buffer empty
	while ( !( UCSR0A & ( 1 <<UDRE0)) );
	
	//Put data into buffer
	UDR0 = c;
	return 0;
}
