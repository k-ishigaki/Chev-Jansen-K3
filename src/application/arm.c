#include "arm.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../peripheral/gpio.h"

/*
 * アームの稼働範囲について
 *
 * 下側アームは可動範囲の中で自由に動かせる
 * 上側アームを下げすぎると下側のアームに付いている板と干渉する．
 *
 * 下側アームの可動範囲：
 *  下限：ほぼ鉛直状態（歩いている時の足にぶつからない位置）
 *  上限：ほぼ水平状態（ギヤ・側面アクリルにぶつからない位置）
 *
 * 上側アームの可動範囲：
 *  下限：下側アームよりも少し上
 *  上限：側面アクリルにぶつからない位置
 *
 */

#define SERVO_UPPER OCR1A	//上側サーボPWM指定  //書き込む手前でcliすること
#define SERVO_LOWER OCR1B	//下側サーボPWM指定 	// main boad MAX3000 //1400 ~ 6200

// アームの可動範囲に対応するPWMの値
#define SERVO_UPPER_BOTTOM 5760	// 値を小さくするとアームが上がる

#define SERVO_LOWER_BOTTOM 1790 // 値を大きくするとアームが上がる
#define SERVO_LOWER_TOP 3200	// 下側アーム上げ幅の上限値


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
	SERVO_UPPER = SERVO_UPPER_BOTTOM;
	SERVO_LOWER = SERVO_LOWER_BOTTOM;
}



// アームの可動範囲を制限した上でアームを動かす
void move_arms(int arm_lower, int arm_upper) {
	// 下側アームのPWM値を適用
	arm_lower += SERVO_LOWER_BOTTOM; // += 1790

	// 下側アームの可動範囲を制限
	if(arm_lower < SERVO_LOWER_BOTTOM){
		arm_lower = SERVO_LOWER_BOTTOM;
	}else if(arm_lower > SERVO_LOWER_TOP){
		arm_lower = SERVO_LOWER_TOP;
	}

	// 上側アームのPWM値を適用
	arm_upper = SERVO_UPPER_BOTTOM - arm_upper;

	// 上側アームの可動範囲を制限（範囲は下側アームの位置に依存）
	if(arm_upper > (6655 - arm_lower + 900)){
		arm_upper = 6655 - arm_lower + 900;
	}else if(arm_upper < (6655 - arm_lower -100)){
		arm_upper = 6655 - arm_lower -100;
	}

	//	Servo用レジスタにPWM値をセット
	cli();
		SERVO_UPPER = arm_upper;//= (SERVO_UPPER_BOTTOM - (adBox[0]<<4));
		SERVO_LOWER = arm_lower;// = (SERVO_LOWER_BOTTOM + (adBox[1]<<4));
	sei();
}

int get_upperArm_position(void){
	return 0;
}

int get_lowerArm_position(void){
	return SERVO_LOWER;
}