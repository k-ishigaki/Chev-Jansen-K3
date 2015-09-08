#include <avr/interrupt.h>
#include "../peripheral/motor.h"
#include "motion.h"
#include "jansenmodel.h"

/* memo
ロータリエンコーダカウント値:距離 = 1000:220mm
	条件:target_cnt = 15
ロータリエンコーダカウント値/回転 = 1700/回転
	条件:超信地回転
*/

// PID制御用係数
#define Kp_R 2		//比例制御係数
#define Kp_L 2
#define Ki_R 1		//積分制御系数
#define Ki_L 1

// 目標値
int8_t target_cnt_r = 0;
int8_t target_cnt_l = 0;

/**
 * 目標値から、モータのPID制御を行います．
 */
void pid_controll(int8_t cnt_r_now, int8_t cnt_l_now);

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

	// 座標系初期化
	set_origin();

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
void set_checkpoint(uint8_t cood_index) {
	set_rel_origin(cood_index)
}
PoleCood get_pole_cood(uint8_t cood_index) {
	return get_rel_polecood(cood_index);
}
RectCood get_rect_cood(uint8_t cood_index) {
	return get_rel_rectcood(cood_index);
}
void test(int hoge) {
	target_cnt_l = hoge;
	target_cnt_r = -hoge;
}

#define HOGE (1700*4)
void pid_controll(int8_t cnt_r_now, int8_t cnt_l_now) {
	// 積分項計算に使用．
	static int16_t r_I = 0;
	static int16_t l_I = 0;
	// 微分項計算に使用．
	int16_t r_P;
	int16_t l_P;

	// 目標値と現在値との偏差
	int8_t delta_R = target_cnt_r - cnt_r_now;
	int8_t delta_L = target_cnt_l - cnt_l_now;

	// P項(微分制御項)
	r_P = delta_R * Kp_R;
	l_P = delta_L * Kp_L;

	// I項(積分制御項)
	r_I += delta_R * Ki_R;
	l_I += delta_L * Ki_L;

	// モータの出力を決定
	int16_t motorR_spd = target_cnt_r + r_P + r_I;
	int16_t motorL_spd = target_cnt_l + l_P + l_I;

	// PWM設定
	set_motor_pwm(motorR_spd, motorL_spd);
}
