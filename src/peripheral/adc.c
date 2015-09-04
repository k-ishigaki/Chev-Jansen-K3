#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"

/* 書き換え禁止 */
#define ADC_START	ADCSRA |= 0b01010000
#define ADC_EndWait	loop_until_bit_is_set(ADCSRA,ADIF)  //変換終了時ADIFがセットされる

/*----------------------------------*/
/* AD初期化							*/

void InitADC(void){
	ADMUX  = 0b01100001;	//AVCC基準，左詰め，AD1選択
	ADCSRA = 0b10000110;	//AD許可，CK/64
	ADCSRB = 0b00000000;
}

/*----------------------------------*/
/* AD単一変換						*/

uint8_t ADC_Solo(uint8_t adch){
	ADMUX = ((ADMUX&0xf0) | adch);
	ADC_START;
	ADC_EndWait;
	cli();
	int ad = ADCH;
	sei();
	return ad;
}
