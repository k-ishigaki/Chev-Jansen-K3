#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define FOSC 20000000
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

void printstr(char *str) {
	while (*str != '\0') {
		uart_putchar(*str);
		str++;
	}
}
void printlnstr(char *str) {
	printstr(str);
	uart_putchar('\n');
	uart_putchar('\r');
}
void printint(int16_t num, uint16_t radix) {
	// 文字列格納する変数(2byteは最大5桁、符号一桁、終端文字1文字)
	char str[7];
	// ポインタ
	char *ptr;

	// 一応初期化
	for (int i=0; i<7; i++) {
		str[i] = '\0';
	}
	// ポインタも
	ptr = str;

	if (num < 0) {
		// 符号
		*ptr = '-';
		// ポインタ進める
		ptr ++;
		// 正規化
		num = -num;
	}

	for (int i=4; i>=0; i--) {
		// 最大5桁

		// その桁の文字取得(右詰め)
		*(ptr+i) = '0' + (num%radix);
		// 桁をずらす
		num /= radix;

		if (num == 0) {
			// もう終わり

			// 右詰めしたものをずらして行く
			for (int j=0; j<=4-i; j++) {
				*(ptr+j) = *(ptr+j+i);
			}

			// 最後はゼロ埋め．
			*(ptr+5-i) = '\0';

			// 出る
			break;
		}
	}

	// 作った文字をプリント
	printstr(str);
}
void printlnint(int16_t num, uint16_t radix) {
	printint(num, radix);
	uart_putchar('\n');
	uart_putchar('\r');
}
char getch() {
	// return dequeue();
	while( !(UCSR0A & 0b10000000) );
	return UDR0;
}
void putch(char c) {
	uart_putchar(c);
}
