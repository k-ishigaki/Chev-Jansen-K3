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

// 動作単位を表す構造体
typedef struct {
	int8_t r_spd;// [cnt]
	int8_t l_spd;// [cnt]
	int16_t cycle;// -1の時はforce_to_nextが来るまでそのまま
} MotionUnit;

// キューのサイズ
#define QUEUE_SIZE 32
// 連続した動作をスケジュールしておくための配列
volatile MotionUnit schedule_queue[QUEUE_SIZE];
// キュー末尾の次のインデックス
volatile int8_t enqueue_index;
// キュー先頭のインデックス
volatile int8_t dequeue_index;
// 今の動作単位をすてて強制的に次に進むか
volatile bool force_to_next;

// 動作単位実行中？
volatile bool moving_now;

// 目標値
// int8_t target_cnt_r = 0;
// int8_t target_cnt_l = 0;

/**
 * 目標値から、モータのPID制御を行います．
 */
void pid_controll(int8_t cnt_r_now, int8_t cnt_l_now);
/**
 * 動作単位を動作スケジュールに追加．
 * @param mu 動作単位を表す構造体．
 */
void enqueue(MotionUnit mu);
/**
 * スケジュールされた次の動作に対応する動作単位を返します．
 * @return スケジュールされた次の動作に対応する動作単位．
 */
MotionUnit dequeue(void);
/**
 * キューをクリアする
 */
void clear_queue(void);
/**
 * 引数で与えられた数値の絶対値を返します．
 * @param num 数値
 * @return 引数の絶対値
 */
static int16_t abs(int16_t num);

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
	// 各原点も全て初期化
	for (int i=0; i<COOD_LIST_SIZE; i++) {
		set_checkpoint(i);
	}

	// キューのリセット
	enqueue_index = 0;
	dequeue_index = 0;
	for (int i=0; i<QUEUE_SIZE; i++) {
		MotionUnit mu = {0, 0, 0};
		schedule_queue[i] = mu;
	}
	// フラグクリア
	force_to_next = false;
	moving_now = false;

	// 割り込み許可
	sei();
}
void move(int curvature, int distance, int velocity) {
	// この動作を表す動作単位
	MotionUnit mu;
	// 目標とする左右のモータスピード
	int16_t r_spd;
	int16_t l_spd;

	// [mm/s] -> [cnt/s] -> [cnt/cycle]
	float spd = velocity * CNT_PER_MM * SEC_PER_CYCLE;

	// 左右の速度の差[cnt/cycle] (ちなみに、MACHINE_RADIUS_MM[mm], curvature[/mm])
	float diff_v = MACHINE_RADIUS_MM * spd * curvature * 0.5 * 0.000001;

	// 左右の速度差から目標値を設定
	r_spd = (spd + diff_v);
	l_spd = (spd - diff_v);

	// 移動に必要な時間[s] (= [mm]/[mm/s])
	float req_time = ((velocity==0) ? 0 : ((float)distance) / velocity);
	// 移動に必要な制御回数[cycle]
	req_time *= CYCLE_PER_SEC;

	// 動作単位を記録
	mu.r_spd = r_spd;
	mu.l_spd = l_spd;
	mu.cycle = req_time;
	// サイクル数の正規化
	mu.cycle = abs(mu.cycle);

	// まずはキューをクリア
	clear_queue();
	// 動作単位をキューに追加
	enqueue(mu);
	// 次は強制的にキューから読み出し
	force_to_next = true;
	// 動作中よ
	moving_now = true;
}
void move_to_pole(PoleCood pc, int velocity) {
	// この動作に必要な3つの動作単位
	MotionUnit mu[3];

	// 速度方向(+/-)
	int8_t spd_sign = (velocity>0) - (velocity<0);
	// 速度([mm/s] -> [cnt/cycle])の絶対値
	int16_t spd_abs = velocity * CNT_PER_MM * SEC_PER_CYCLE * spd_sign;

	// 各移動距離(回転/直進/回転)
	int16_t len[3];
	len[0] = pc.phi1 / 360.0 * CIRC_CNT;
	len[1] = pc.distance * CNT_PER_MM;
	len[2] = pc.phi2 / 360.0 * CIRC_CNT;

	for (int phase=0; phase<3; phase++) {
		// 進む向き
		int8_t len_sign = (len[phase]>0) - (len[phase]<0);

		// 右のスピード設定
		mu[phase].r_spd = len_sign * spd_sign * spd_abs;
		// 左のスピード設定(直進時だけは左右一緒の速度)
		mu[phase].l_spd = ((phase==1) ? mu[phase].r_spd : -mu[phase].r_spd);

		// 必要な時間
		mu[phase].cycle = ((spd_abs==0) ? 0 : len_sign * len[phase] / spd_abs);
	}

	// まずはキューをクリア
	clear_queue();
	// 動作単位をキューに追加
	for (int phase=0; phase<3; phase++) {
		enqueue(mu[phase]);
	}
	// 次は強制的にキューから読み出し
	force_to_next = true;
	// 動作中よ
	moving_now = true;
}
void move_to_rect(RectCood rc, int velocity) {
	// PoleCoodのほうが計算しやすいので変換してから
	PoleCood pc;
	pc = rect2pole(rc);
	// いけー
	move_to_pole(pc, velocity);
}
void move_turn(int degree, enum TURN_DIRECTION turn_direction, int velocity) {
	// この動作を表す動作単位
	MotionUnit mu;

	// [mm/s] -> [cnt/s] -> [cnt/cycle]
	float spd = velocity * CNT_PER_MM * SEC_PER_CYCLE;

	// 指定された角度を進むのに必要な距離[cnt]
	float req_distance = degree / 180.0 * CIRC_CNT;

	// 動作単位を記録
	if (turn_direction == RIGHT_TURN) {
		mu.r_spd = 0;
		mu.l_spd = ((degree>0)-(degree<0)) * spd * 2.0;
	} else {
		mu.r_spd = ((degree>0)-(degree<0)) * spd * 2.0;
		mu.l_spd = 0;
	}
	mu.cycle = ((spd==0) ? 0 : req_distance / (spd * 2.0));
	// サイクル数の正規化
	mu.cycle = abs(mu.cycle);

	// まずはキューをクリア
	clear_queue();
	// 動作単位をキューに追加
	enqueue(mu);
	// 次は強制的にキューから読み出し
	force_to_next = true;
	// 動作中よ
	moving_now = true;
}
void move_forward(int distance, int velocity) {
	// この動作を表す動作単位
	MotionUnit mu;

	// [mm/s] -> [cnt/s] -> [cnt/cycle]
	float spd = velocity * CNT_PER_MM * SEC_PER_CYCLE;

	// 動作単位を記録
	mu.r_spd = ((distance>0)-(distance<0)) * spd * 2.0;
	mu.l_spd = mu.r_spd;
	mu.cycle = ((spd==0) ? 0 : distance * CNT_PER_MM / spd);
	// サイクル数の正規化
	mu.cycle = abs(mu.cycle);

	// まずはキューをクリア
	clear_queue();
	// 動作単位をキューに追加
	enqueue(mu);
	// 次は強制的にキューから読み出し
	force_to_next = true;
	// 動作中よ
	moving_now = true;
}
bool is_moving() {
	return moving_now;
}
void wait_completion(void) {
	while (is_moving()) ;
}
void set_checkpoint(uint8_t cood_index) {
	set_rel_origin(cood_index);
}
PoleCood get_pole_cood(uint8_t cood_index) {
	return get_rel_polecood(cood_index);
}
RectCood get_rect_cood(uint8_t cood_index) {
	return get_rel_rectcood(cood_index);
}

void pid_controll(int8_t cnt_r_now, int8_t cnt_l_now) {
	// 積分項計算に使用．
	static int16_t r_I = 0;
	static int16_t l_I = 0;
	// 微分項計算に使用．
	int16_t r_P;
	int16_t l_P;
	// 目標値
	static int8_t trg_spd_r = 0;
	static int8_t trg_spd_l = 0;
	// 現在実行中の動作単位
	static MotionUnit current_mu = {0, 0, 0};

	// 補償計算
	cnt_l_now *= CNT_COMPENSATION;

	// 座標計算
	move_one_step(cnt_r_now, cnt_l_now);

	// まずは現在実行中の動作単位について、その実行持続ステップ数をデクリメント
	if (current_mu.cycle-- == 0 || force_to_next) {
		// もし予定されたステップ数を実行し終えた場合 or 強制的に次の動作へ

		// 強制フラグ解除
		force_to_next = false;

		// 次の動作単位をキューからロード
		current_mu = dequeue();
		// 目標速度をセット
		trg_spd_r = current_mu.r_spd;
		trg_spd_l = current_mu.l_spd;

		// 指定された動作をしている最中か？
		moving_now = (current_mu.cycle != 0);

		// 継続動作単位の場合は減りすぎないように
		if (current_mu.cycle < 0) {
			current_mu.cycle = -1;
		}
	}

	// 目標値と現在値との偏差
	int8_t delta_R = trg_spd_r - cnt_r_now;
	int8_t delta_L = trg_spd_l - cnt_l_now;

	// P項(微分制御項)
	r_P = delta_R * Kp_R;
	l_P = delta_L * Kp_L;

	// I項(積分制御項)
	r_I += delta_R * Ki_R;
	l_I += delta_L * Ki_L;

	// モータの出力を決定
	int16_t pwm_r = trg_spd_r + r_P + r_I;
	int16_t pwm_l = trg_spd_l + l_P + l_I;

	// PWM設定
	set_motor_pwm(pwm_r, pwm_l);
}
void enqueue(MotionUnit mu) {
	// 次の追加インデックス
	int8_t next_enqueue_index = (enqueue_index+1) % QUEUE_SIZE;

	if (next_enqueue_index == dequeue_index) {
		// 入れる場所がない．
		return;
	}

	// まだ余裕ある場合，入れる．
	schedule_queue[enqueue_index] = mu;

	// インデックスを進める
	enqueue_index = next_enqueue_index;
}
MotionUnit dequeue(void) {
	// 返すやつ
	MotionUnit mu = {0, 0, 0};

	if (dequeue_index == enqueue_index) {
		// 出すデータがない．
		return mu;
	}

	// 出すデータが有るときはそれを返す．
	mu = schedule_queue[dequeue_index];

	// インデックスを進める
	dequeue_index = (dequeue_index+1) % QUEUE_SIZE;

	return mu;
}
void clear_queue(void) {
	dequeue_index = enqueue_index = 0;
}

static int16_t abs(int16_t num) {
	return ((num > 0) - (num < 0)) * num;
}
