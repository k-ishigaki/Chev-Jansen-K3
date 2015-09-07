#include <avr/interrupt.h>
#include <stdbool.h>
#include "motor.h"

// モータ制御に使用するピン
#define motorR_PIN PD7
#define motorL_PIN PB0
// モータPWM値を代入するレジスタ(0~255，後はマイコンのモジュールが処理してくれる)
#define motorR_spd OCR0A
#define motorL_spd OCR0B
// 回転方向設定
#define R_FORWARD PORTD|=_BV(motorR_PIN)	//右脚前進回転方向
#define L_FORWARD PORTB|=_BV(motorL_PIN)	//左脚前進回転方向
#define R_BACK PORTD&=~_BV(motorR_PIN)		//右脚後進回転方向
#define L_BACK PORTB&=~_BV(motorL_PIN)		//左脚後進回転方向
// PID制御周期定数，(T2OVF_NUM_MAX * 13)mSec周期でPID制御
#define T2OVF_NUM_MAX 4

// ロータリエンコーダカウント値
volatile int8_t motorR_cnt=0;
volatile int8_t motorL_cnt=0;
// 現在正転/逆転
volatile bool motorR_dir = true;
volatile bool motorL_dir = true;
// 現在停止中華
volatile bool motorR_stop = true;
volatile bool motorL_stop = true;
// Timer2割り込みのたびに呼び出す関数のポインタ，引数は右左のロータリエンコーダの値
void (*t2intr_callback)(int8_t, int8_t);

void init_timer0() {
	TCCR0A = 0b11110011;	//反転動作, 8ビット高速PWM動作, TOP:0xFF
	TCCR0B = 0b00000101;	//8ビット高速PWM動作, 8分周(約19kHz,0.1msec周期)
	TIFR0 = 0;	//割り込みなし要求フラグリセット
	TIMSK0 = 0;	//割り込みなし

	DDRD |= _BV(motorR_PIN)|_BV(motorL_PIN);//0b0qq00000;	//PWMPin出力設定
}
void init_timer2() {
	TCCR2A = 0b00000000;	//TOP: OVF
	TCCR2B = 0b00000111;	//1024分周（約13msec,76.3Hz割り込み）
	TIMSK2 = 0b00000001;	//OVF割り込みのみ許可
	TIFR2  = 0;
}
void init_intr() {
	EICRA = 0b00001010;	//INT0,1立下り割り込み
	EIMSK = 0b00000011;	//INT0,1割り込み許可
	DDRD &= ~_BV(PD2);
	DDRD &= ~_BV(PD3);
}
void set_t2intr_callback(void (*f)(int8_t, int8_t)) {
	t2intr_callback = f;
}
void set_motor_pwm(int16_t r_pwm, int16_t l_pwm) {
	// 範囲制限
	if (r_pwm < -255) {
		r_pwm = -255;
	} else if (r_pwm > 255) {
		r_pwm = 255;
	}
	if (l_pwm < -255) {
		l_pwm = -255;
	} else if (l_pwm > 255) {
		l_pwm = 255;
	}

	// 右モータ前後指定
	if (r_pwm > 0) {
		// 正転
		R_FORWARD;
		motorR_dir = true;
	} else {
		// 反転
		R_BACK;
		r_pwm = -r_pwm;
		motorR_dir = false;
	}

	// 左モータ前後指定
	if (l_pwm > 0) {
		// 正転
		L_FORWARD;
		motorL_dir = true;
	} else {
		// 反転
		L_BACK;
		l_pwm = -l_pwm;
		motorL_dir = false;
	}

	// 停止？
	motorR_stop = (r_pwm == 0);
	motorL_stop = (l_pwm == 0);

	motorR_spd = r_pwm;
	motorL_spd = l_pwm;
}

/**
 * 右モータのロータリエンコーダ割り込み関数
 */
ISR(INT0_vect) {
	if (motorR_dir) {
		motorR_cnt++;
	} else if (!motorR_stop) {
		motorR_cnt--;
	}
}
/**
 * 左モータのロータリエンコーダ割り込み関数
 */
ISR(INT1_vect) {
	if (motorL_dir) {
		motorL_cnt++;
	} else if (!motorL_stop) {
		motorL_cnt--;
	}
}
/**
 * Timer2割り込み関数．
 * この関数が呼ばれる際、T2OVF_NUM_MAX回はスルーして、その後{@code t2intr_callback(int,int)}を呼びます
 */
ISR(TIMER2_OVF_vect) {
	// 割り込み回数カウントに使用
	static uint8_t T2OVF_cnt = 0;

	if(T2OVF_cnt<T2OVF_NUM_MAX){
		// 割り込みカウンタインクリメント
		T2OVF_cnt++;
	}else{
		// 割り込みカウンタリセット
		T2OVF_cnt=0;
		// 指定された関数呼び出し
		t2intr_callback(motorR_cnt, motorL_cnt);
		// ロータリエンコーダカウント値をリセット
		motorR_cnt = 0;
		motorL_cnt = 0;
	}
}
