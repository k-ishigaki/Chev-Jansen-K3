#include <avr/interrupt.h>
#include "../peripheral/motor.h"
#include "motion.h"

// PID制御用係数
#define mK_r 3>>1	//全体制御系数
#define mK_l 3>>1
#define mP_r 2		//比例制御係数
#define mP_l 2
#define mI_r 5		//積分制御系数
#define mI_l 5

// 目標値
uint8_t target_cnt_r = 0;
uint8_t target_cnt_l = 0;

/**
 * 目標値から、モータのPID制御を行います．
 */
void pid_controll(uint8_t cnt_r_now, uint8_t cnt_l_now);
/**
 * 引数で指定された値を0~255の範囲に制限して返します．
 * @param x 制限したい値
 */
static uint8_t limit(int16_t x);

void init_motion() {
	// (一応)割り込み禁止
	cli();

	// pwmに関する初期化
	init_timer0();
	// pid制御割り込みに関する初期化
	init_timer2();
	// ロータリエンコーダに関する初期化
	init_intr();
	// Timer2割り込みのコールバック設定
	set_t2intr_callback(pid_controll);

	// 割り込み許可
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
PoleCood get_pole_cood(int num) {
	PoleCood pc;
	pc.distance = 100;
	pc.phi1 = 100;
	pc.phi2 = 100;
	return pc;
}
RectCood get_rect_cood(int num) {
	RectCood rc;
	rc.x = 100;
	rc.y = 100;
	rc.theta = 100;
	return rc;
}
void test(int hoge) {
	target_cnt_l = hoge;
	target_cnt_r = hoge;
}

void pid_controll(uint8_t cnt_r_now, uint8_t cnt_l_now) {
	// 積分項計算に使用．
	static int16_t r_I = 0;
	static int16_t l_I = 0;

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
	int16_t motorR_spd = limit( ( target_cnt_r + r_P + r_I ) *mK_r);
	int16_t motorL_spd = limit( ( target_cnt_l + l_P + l_I ) *mK_l);
	// motorR_spd = limit( ( target_cnt_r + r_P + r_I/3 ) *mK_r);
	// motorL_spd = limit( ( target_cnt_l + l_P + l_I/3 ) *mK_l);

	// PWM設定
	set_motor_pwm(motorR_spd, motorL_spd);
}
static uint8_t limit(int16_t x) {
	if (x > 255) {
		return 255;
	} else if (x < 0) {
		return 0;
	} else {
		return x;
	}
}

