#include <avr/io.h>
#include <stdio.h>

#define FOSC 16000000
#define BAUD 57600
#define USART_BAUD ((FOSC/16)+(BAUD>>1))/BAUD-1

static int uart_putchar(char);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static int uart_putchar(char c){
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

void init_serial() {
	UDR0 = 0;
	UCSR0A = 0b00100000;	//default
	UCSR0B = 0b00001000;	//送信許可
	UCSR0B |= 0b10010000;	//受信許可
	UCSR0C = 0b00000110;	//default
	UBRR0 = USART_BAUD;
	stdout = &mystdout;
}

// printf is declared at stdio
