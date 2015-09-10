#include "arm.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../peripheral/gpio.h"

#define ServoSub OCR1A	//サーボ角度指定 ?~?    //書き込む手前でcliすること
#define ServoMain OCR1B	//サーボ角度指定 ?~?	// main boad MAX3000 //1400 ~ 6200
#define Servo1_Default 5760	//min, 5760-4870(S=2176),4300good~5150(S2=2400) MAX 4600 ~ 3600(S2=3000)
#define Servo_Default 1790	//for main boad arm

void init_arm() {
	const struct DigitalPin* mainPin = DigitalPins.PB2Pin(); // OC1B
	const struct DigitalPin* subPin  = DigitalPins.PB1Pin(); // OC1A

	// 使用するピンを出力に設定しておく
	// PWMを利用するには必須
	mainPin->setDirection(DigitalPin_Direction.OUTPUT);
	subPin->setDirection(DigitalPin_Direction.OUTPUT);

	// タイマ1初期化
	TCCR1A = 0b10100010; //高速PWM, TOP:ICR1
	TCCR1B = 0b00011010; //高速PWM, 8分周
	//TCCR1C PWMなのでデフォルト値
	ICR1  = 0xFFF0;      //周期,約26msec
	TIFR1 = 0;           //割り込み要求フラグ
	TIMSK1 = 0;          //割り込みなし

	DDRB |= 0b00000110;		//PWMPin出力設定

	//サーボ設定
	ServoSub = Servo1_Default;
	ServoMain = Servo_Default;
}

void move_arms(int s_main, int s_sub) {
	s_main += Servo_Default;
	//max and min
	if(s_main < 1760){
		s_main = 1760;
	}else if(s_main >4250){
		s_main = 4250;
	}

	s_sub = Servo1_Default - s_sub;

	if(s_sub > (6655 - s_main + 900)){
		s_sub = 6655 - s_main + 900;
		//uart_putchar('a');
	}else if(s_sub < (6655 - s_main -100)){//-1000)){
		s_sub = 6655 - s_main -100;
		//uart_putchar('b');
	}

	//	Servo
	cli();
		ServoSub  = s_sub;//= (Servo1_Default - (adBox[0]<<4));
		ServoMain = s_main;// = (Servo_Default + (adBox[1]<<4));
	sei();
}

