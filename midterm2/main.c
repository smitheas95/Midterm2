/*
midterm2.c
Created: 5/11/2019 3:37:39 PM
Author : Eric Smith

Q: Write, simulate, and demonstrate using Atmel Studio 7 a C code for the AVR
ATMEGA328p microcontroller that performs the following functions:
1. Program the I2C of ATmega328/p to read RGB/Ambient Light data from APDS
9960 sensor.
2. Display the value to UART.
3. Make sure the AT Firmware is downloaded into the ESP-01/ESP32 module.
4. Register for a free Thingspeak account with MATHWORK. Setup and get the
channel Key.
5. Transmit Lux sensor value to ESP-01/ESP32 through UART port using AT
Commands.
6. Display the Lux sensor value as a graph in Thingspeak
*/

#define F_CPU 16000000UL // DEFINE 16MHz

// NEEDED LIBRARIES
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "uart.h"
#include "apds.h"
#include "i2c_master.h"

// BAUDRATE DEFINITIONS 
#define BRGVAL (F_CPU/16/BAUD) - 1
#define BAUD 9600

// APDS DEFINITIONS 
#define APDS_WRITE	(0x39 << 1) | 0
#define APDS_READ	(0x39 << 1) | 1

#ifndef APDS_H
#define APDS_H

#define APDS9960_I2C_ADDR       0x39 // I2C ADDRESS FOR APDS-9960
#define ERROR                   0xFF // RETURNED VALUES ERROR CODE 

// MISC PARAMETERS
#define FIFO_PAUSE_TIME         30 // FIFO READS, WAIT PERIOD IN ms

// ACCEPTABLE DEVICE IDs
#define APDS9960_ID_1    0xAB
#define APDS9960_ID_2    0x9C

// BIT FIELDS
#define APDS9960_PON     0b00000001
#define APDS9960_AEN     0b00000010
#define APDS9960_PEN     0b00000100
#define APDS9960_WEN     0b00001000
#define APSD9960_AIEN    0b00010000
#define APDS9960_PIEN    0b00100000
#define APDS9960_GEN     0b01000000
#define APDS9960_GVALID  0b00000001

// APDS-9960 REGISTER ADDRESSES 
#define APDS9960_ENABLE         0x80
#define APDS9960_ATIME          0x81
#define APDS9960_WTIME          0x83
#define APDS9960_PERS           0x8C
#define APDS9960_CONFIG1        0x8D
#define APDS9960_PPULSE         0x8E
#define APDS9960_CONFIG2        0x90
#define APDS9960_ID             0x92
#define APDS9960_RDATAL         0x96
#define APDS9960_RDATAH         0x97
#define APDS9960_GDATAL         0x98
#define APDS9960_GDATAH         0x99
#define APDS9960_BDATAL         0x9A
#define APDS9960_BDATAH         0x9B
#define APDS9960_POFFSET_UR     0x9D
#define APDS9960_POFFSET_DL     0x9E
#define APDS9960_CONFIG3        0x9F

// ON/OFF DEFINITIONS
#define OFF                     0
#define ON                      1

// ACCEPTABLE PARAMTERS FOR SETMODE 
#define POWER                   0
#define AMBIENT_LIGHT           1

// DEFINE PROXIMITY 
#define WAIT                    3
#define AMBIENT_LIGHT_INT       4
#define ALL                     7

// LED BOOST VALUES
#define LED_BOOST_100           0
#define LED_BOOST_150           1
#define LED_BOOST_200           2
#define LED_BOOST_300           3

// LED DRIVE VALUES 
#define LED_DRIVE_100MA         0
#define LED_DRIVE_50MA          1
#define LED_DRIVE_25MA          2
#define LED_DRIVE_12_5MA        3

// DEFAULT VALUES 
#define DEFAULT_ATIME           219 // 103 ms VALUE 
#define DEFAULT_WTIME           246 // 27 ms VALUE 
#define DEFAULT_PROX_PPULSE     0x87 // 16 us, PULSE OF 8 
#define DEFAULT_POFFSET_UR      0 // OFFSET 0
#define DEFAULT_POFFSET_DL      0 // OFFSET 0
#define DEFAULT_CONFIG1         0x60 // WAIT FACTOR FOR NO 12x
#define DEFAULT_LDRIVE          LED_DRIVE_100MA
#define DEFAULT_PGAIN           PGAIN_4X
#define DEFAULT_AGAIN           AGAIN_4X
#define DEFAULT_AILT            0xFFFF // CALIBRATION FORCE INTERRUPT
#define DEFAULT_AIHT            0
#define DEFAULT_PERS            0x11 // 2 CONSECUTIVE PROX OR ALS FOR INT. 
#define DEFAULT_CONFIG2         0x01 // SATURATION INTERRUPTS, LED BOOST NONE 
#define DEFAULT_CONFIG3         0  // ENABLE ALL PHOTODIODES, NO SAI ENABLE ALL PHOTODIODES 
#define DEFAULT_GLDRIVE         LED_DRIVE_100MA
#define DEFAULT_GWTIME          GWTIME_2_8MS

void apds_init(); // VOID FUNCTION FOR APDS INIT 
void colorRead(); // VOID FUNCTION FOR READING COLOR

#endif

// FILE 
FILE string_uart = FDEV_SETUP_STREAM(uart_putchar, NULL , _FDEV_SETUP_WRITE );
char results[256]; // 256 

void init_UART(); // VOID FUNCTION FOR INIT UART 
int uart_putchar( char c, FILE *stream); // VARIABLE INT FOR PUTTING CHARACTERS UART 


int main(void) // INT MAIN & CONTAINS THINGSPEAK PORTION 
{
	uint16_t red = 0, green = 0, blue = 0; // SET RED, GREEN, BLUE TO ZERO 
	
	i2c_init();
	init_UART();
	
	stdout = &string_uart; // STRING UART 

	apds_init();
	
	// RESPECTIVE DELAYS PROVIDED 
 	_delay_ms(2500);
	printf("AT\r\n");	
	_delay_ms(4500);
	printf("AT+CWMODE=1\r\n"); 	
	_delay_ms(4500);
	printf("AT+CWJAP=\"n/aVerizon-SM-G950U-748C\",\"n/anifj003)\"\r\n"); // WHERE I PUT MY HOTSPOT AND PASSWORD FOR HOTSPOT (ALTERED FOR SECURITY REASONS)	
    while (1) // KEY WHILE LOOP 
    {
		_delay_ms(4500);
		printf("AT+CIPMUX=0\r\n");
		
		_delay_ms(4500);
		printf("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
		
		_delay_ms(4500);
		colorRead(&red, &green, &blue);
		printf("AT+CIPSEND=104\r\n");
		
		// API KEY WITH RESPECTIVE FIELDS FOR 3 COLORS 
		printf("GET https://api.thingspeak.com/update?api_key=PRH9WSZ8I5Y3A3LT&field1=%05u&field2=%05u&field3=%05u\r\n", red, green, blue);
		_delay_ms(4500);
    }
}

void init_UART(void){
	
	// BAUD RATE SET
	uint16_t baud_rate = BRGVAL;
	UBRR0H = baud_rate >> 8;
	UBRR0L = baud_rate & 0xFF;
	
	// RECEIVER AND TRANSMITTER ENABLED 
	UCSR0B = ( 1 <<RXEN0)|( 1 <<TXEN0);
	
	// FRAME FORMAT SET: 8 DATA, 1 STOP BIT
	UCSR0C = (3 <<UCSZ00);
}

int uart_putchar(char c, FILE *stream){
	
	// BUFFER IS EMPTY, WAIT TIL THEN 
	while ( !( UCSR0A & ( 1 <<UDRE0)) );
	
	// BUFFER GETS DATA 
	UDR0 = c;
	return 0;
}

void apds_init(){
	
	uint8_t setup;
	
	i2c_readReg(APDS_WRITE, APDS9960_ID, &setup,1);
	
	if(setup != APDS9960_ID_1) while(1);
	
	setup = 1 << 1 | 1<<0 | 1<<3 | 1<<4;
	
	// I2C
	i2c_writeReg(APDS_WRITE, APDS9960_ENABLE, &setup, 1);
	setup = DEFAULT_ATIME;
	i2c_writeReg(APDS_WRITE, APDS9960_ATIME, &setup, 1);
	setup = DEFAULT_WTIME;
	i2c_writeReg(APDS_WRITE, APDS9960_WTIME, &setup, 1);
	setup = DEFAULT_PROX_PPULSE;
	i2c_writeReg(APDS_WRITE, APDS9960_PPULSE, &setup, 1);
	setup = DEFAULT_POFFSET_UR;
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_UR, &setup, 1);
	setup = DEFAULT_POFFSET_DL;
	i2c_writeReg(APDS_WRITE, APDS9960_POFFSET_DL, &setup, 1);
	setup = DEFAULT_CONFIG1;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG1, &setup, 1);
	setup = DEFAULT_PERS;
	i2c_writeReg(APDS_WRITE, APDS9960_PERS, &setup, 1);
	setup = DEFAULT_CONFIG2;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG2, &setup, 1);
	setup = DEFAULT_CONFIG3;
	i2c_writeReg(APDS_WRITE, APDS9960_CONFIG3, &setup, 1);
}

void colorRead(uint16_t *red, uint16_t *green, uint16_t *blue){
	
	uint8_t redl, redh; // RED LOW, RED HIGH 
	uint8_t greenl, greenh; // GREEN LOW, GREEN HIGH 
	uint8_t bluel, blueh; // BLUE LOW, BLUE HIGH 
	
	i2c_readReg(APDS_WRITE, APDS9960_RDATAL, &redl, 1);
	i2c_readReg(APDS_WRITE, APDS9960_RDATAH, &redh, 1);
	i2c_readReg(APDS_WRITE, APDS9960_GDATAL, &greenl, 1);
	i2c_readReg(APDS_WRITE, APDS9960_GDATAH, &greenh, 1);
	i2c_readReg(APDS_WRITE, APDS9960_BDATAL, &bluel, 1);
	i2c_readReg(APDS_WRITE, APDS9960_BDATAH, &blueh, 1);
	
	*red = redh << 8 | redl;
	*green = greenh << 8 | greenl;
	*blue = blueh << 8 | bluel;
}

