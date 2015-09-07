#include <avr/interrupt.h>
#include "motion.h"

// モータ制御に使用するピン
#define motorR_PIN PD7
#define motorL_PIN PB0
// モータPWM値を代入するレジスタ(0~255，後はマイコンのモジュールが処理してくれる)
#define motorR_spd OCR0A
#define motorL_spd OCR0B

// PID制御用係数
#define mK_r 3>>1	//全体制御系数
#define mK_l 3>>1
#define mP_r 2		//比例制御係数
#define mP_l 2
#define mI_r 5		//積分制御系数
#define mI_l 5
// PID制御周期定数，(T2OVF_NUM_MAX * 13)mSec周期でPID制御
#define T2OVF_NUM_MAX 4

#define R_FORWARD PORTD|=_BV(motorR_PIN)	//右脚前進回転方向
#define L_FORWARD PORTB|=_BV(motorL_PIN)	//左脚前進回転方向
#define R_BACK PORTD&=~_BV(motorR_PIN)		//右脚後進回転方向
#define L_BACK PORTB&=~_BV(motorL_PIN)		//左脚後進回転方向


// モータ制御用変数
volatile uint8_t motorR_cnt=0;
volatile uint8_t motorL_cnt=0;
// 目標値
uint8_t target_cnt_r = 0;
uint8_t target_cnt_l = 0;

// プロトタイプ宣言
static void init_timer0(void);
static void init_timer2(void);
static void init_intr(void);
static uint8_t limit(int16_t x);

void init_motion() {
	// pwmに関する初期化
	init_timer0();
	// pid制御割り込みに関する初期化
	init_timer2();
	// ロータリエンコーダに関する初期化
	init_intr();
	
	R_FORWARD;
	L_FORWARD;

	target_cnt_r=30;
	target_cnt_l=30;

	sei();
}
void move(int curvature, int distance, int velocity) {
}
void move_to_pole(PoleCood* pc, int velocity) {
}
void move_to_rect(RectCood* rc, int velocity) {
}
bool is_moving() {
	return false;
}
void set_checkpoint(int num) {
}
PoleCood* get_pole_cood(int num) {
	PoleCood pc = {0, 0, 0};
	return &pc;
}
RectCood* get_rect_cood(int num) {
	RectCood rc = {0, 0, 0};
	return &rc;
}
void test(int hoge) {
	target_cnt_l = hoge;
	target_cnt_r = hoge;
}


/**
 * モータPWMを発生させるためのTimer0初期設定を行います．
 */
static void init_timer0() {
	TCCR0A = 0b11110011;	//反転動作, 8ビット高速PWM動作, TOP:0xFF
	TCCR0B = 0b00000101;	//8ビット高速PWM動作, 8分周(約19kHz,0.1msec周期)
	TIFR0 = 0;	//割り込みなし要求フラグリセット
	TIMSK0 = 0;	//割り込みなし

	DDRD |= _BV(motorR_PIN)|_BV(motorL_PIN);//0b0qq00000;	//PWMPin出力設定
}
/**
 * モータPID制御を行うためのTimer2初期設定を行います．
 * これにより，割り込み関数{@code ISR(TIMER2_OVF_vect)}は、約13msec周期(76.3Hz)で呼びだされます．
 * また，PID制御については，
 */
static void init_timer2() {
	TCCR2A = 0b00000000;	//TOP: OVF
	TCCR2B = 0b00000111;	//1024分周（約13msec,76.3Hz割り込み）
	TIMSK2 = 0b00000001;	//OVF割り込みのみ許可
	TIFR2  = 0;
}
/**
 * ロータリエンコーダ割り込みを制御するための初期化を行います．
 */
static void init_intr() {
	EICRA = 0b00001010;	//INT0,1立下り割り込み
	EIMSK = 0b00000011;	//INT0,1割り込み許可
	DDRD &= ~_BV(PD2);
	DDRD &= ~_BV(PD3);
}
/**
 * 引数で指定された値を0~255の範囲に制限して返します．
 * @param x 制限したい値
 */
static uint8_t limit(int16_t x){
	if (x > 255) {
		return 255;
	} else if (x < 0) {
		return 0;
	} else {
		return x;
	}
}

/**
 * 右モータのロータリエンコーダ割り込み関数
 */
ISR(INT0_vect) {
	motorR_cnt++;	//ロータリエンコーダRカウントアップ
}
/**
 * 右モータのロータリエンコーダ割り込み関数
 */
ISR(INT1_vect) {
	motorL_cnt++;	//ロータリエンコーダLカウントアップ
}
/**
 * 目標値から、モータのPID制御を行います．
 */
ISR(TIMER2_OVF_vect) {
	// 積分項計算に使用．
	static int16_t r_I = 0;
	static int16_t l_I = 0;
	// 割り込み回数カウントに使用
	static uint8_t T2OVF_cnt = 0;

	if(T2OVF_cnt<T2OVF_NUM_MAX){//周期これでいいのかな？
		T2OVF_cnt++;
	}else{
		T2OVF_cnt=0;

		// ロータリエンコーダカウント値を記録/リセット
		uint8_t cnt_r_now = motorR_cnt;
		uint8_t cnt_l_now = motorL_cnt;
		motorR_cnt = 0;
		motorL_cnt = 0;

		// 目標値と現在値との偏差を取得
		int8_t deltaR = target_cnt_r - cnt_r_now;
		int8_t deltaL = target_cnt_l - cnt_l_now;

		// P制御(比例制御)
		int16_t r_P = deltaR*mP_r;
		int16_t l_P = deltaL*mP_l;

		// I制御(積分制御)
		r_I+= deltaR;
		l_I+= deltaL;

		// モータの出力を決定
		motorR_spd = limit( ( target_cnt_r + r_P + r_I ) *mK_r);
		motorL_spd = limit( ( target_cnt_l + l_P + l_I ) *mK_l);
		// motorR_spd = limit( ( target_cnt_r + r_P + r_I/3 ) *mK_r);
		// motorL_spd = limit( ( target_cnt_l + l_P + l_I/3 ) *mK_l);
	}
}