#include <avr/interrupt.h>
#include "../peripheral/motor.h"
#include "motion.h"
#include "jansenmodel.h"

#include "serial.h"

/* memo
ロータリエンコーダカウント値:距離 = 1000:220mm
	条件:target_cnt = 15
ロータリエンコーダカウント値/回転 = 1700/回転
	条件:超信地回転
*/


// PID制御用係数
#define Kp_R 1000		//比例制御係数
#define Kp_L 1000
#define Ki_R 10		//積分制御系数
#define Ki_L 10

// 動作単位を表す構造体
typedef struct {
	int8_t r_spd;// [cnt]
	int8_t l_spd;// [cnt]
	int16_t cycle;// -1の時はforce_to_nextが来るまでそのまま
} MotionUnit;

// 目標とする左右のモータスピード
int16_t r_spd;
int16_t l_spd;

/**
 * 目標値から、モータのPID制御を行います．
 */
static void pid_controll(int cnt_r_now, int cnt_l_now);

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

	// [mm/s] -> [cnt/s] -> [cnt/cycle]
	float spd = (float)velocity * CNT_PER_MM * SEC_PER_CYCLE;

	// 左右の速度の差[cnt/cycle] (ちなみに、MACHINE_RADIUS_MM[mm], curvature[/mm])
	float diff_v = (float)MACHINE_RADIUS_MM * spd * curvature * 0.5 * 0.000001;

	// 左右の速度差から目標値を設定
	r_spd = (spd + diff_v);
	l_spd = (spd - diff_v);
}

void move_to_pole(PoleCood pc, int velocity) {
}

void move_to_rect(RectCood rc, int velocity) {
}

void move_turn(int degree, enum TURN_DIRECTION turn_direction, int velocity) {
}

void move_forward(int distance, int velocity) {
}

bool is_moving() {
//	return moving_now;
	return false;
}

void wait_completion(void) {
//	while (is_moving()) ;
}

void set_checkpoint(uint8_t cood_index) {
//	set_rel_origin(cood_index);
}

static PoleCood pole = {
	0,0,0,
};

static RectCood rect = {
	0,0,0,
};

PoleCood get_pole_cood(uint8_t cood_index) {
	return pole;
}
RectCood get_rect_cood(uint8_t cood_index) {
	return rect;
}

static void pid_controll(int cnt_r_now, int cnt_l_now) {
	// 目標値
	static int16_t trg_spd_r = 0;
	static int16_t trg_spd_l = 0;

	trg_spd_r = r_spd;
	trg_spd_l = l_spd;
	// 目標値と現在値との偏差
	int8_t delta_R = trg_spd_r - cnt_r_now;
	int8_t delta_L = trg_spd_l - cnt_l_now;

	// I項(積分制御項)
	static long r_I = 0;
	static long l_I = 0;
	r_I += delta_R;
	l_I += delta_L;

	// モータの出力を決定
	int16_t pwm_r = 3 * ((long)trg_spd_r + (delta_R * Kp_R + r_I * Ki_R) / 1000) / 4 - 100;
	int16_t pwm_l = 3 * ((long)trg_spd_l + (delta_L * Kp_L + l_I * Ki_L) / 1000) / 4 - 100;

	printf("%d\t%d\r\n", trg_spd_r, cnt_r_now);
	// PWM設定
	set_motor_pwm(pwm_r, pwm_l);
}

