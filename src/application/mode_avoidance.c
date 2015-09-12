#include <stdint.h>
#include <stdbool.h>
#include "motion.h"
#include "coordinate.h"
#include "distance.h"
#include "line.h"

// 障害物との距離がこれ以下なら避ける[mm]
#define AVOIDANCE_THRESHOLD 200
// これを超えれば白い線は無い[mm]
#define WHITE_OVER	1500
// 開始時の座標インデックス
#define START_CHECKPOINT 0
// 距離センサ最大値[mm]
#define IR_MAX	300
#define IR_MIN	100

// 避けるときの角度
#define TURN_DEG	60
#define FORWARD_LEN	150


// 4つのセンサのうち、一番障害物までの距離が短いセンサ
enum IRSensorID min_ir;
// 障害物までの距離
int16_t min_distance;
// 障害物避ける必要あり？
bool must_avoid;
// センサー値
int16_t distance_array[SIZE_OF_IRSensorID];

/**
 * 距離センサを読み取り、変数{@code min_ir}と{@code min_distance}に格納します．
 */
void find_obstacle(void);
/**
 * 回避運動をします．
 */
void avoidance_motion(void);

void move_avoidance_loop(void) {
	// ラインを検知した座標に使う
	RectCood rc;
	// ライン復帰動作(90度旋回)
	// PoleCood pc;
	// ラインステータス
	enum LineState ls;

	// まずは初期位置を設定
	set_checkpoint(START_CHECKPOINT);

	// 取り敢えず直進させとく．

	while (1) {
		// 障害物ある？
		find_obstacle();
		if (must_avoid) {
			// あったら避ける
			avoidance_motion();
		} else {// if (!is_moving()) {
			// なけりゃあ直進(距離は適当)
			// ただし、少しでも時間短縮のため、動いてるなら(直進してるなら)そのまま
			move_forward(250, 50);
		}

		// ライン状態取得
		ls = get_line_state();
		if (ls == IN_LINE) {
			// ライン検知！
			// 現在地取得
			rc = get_rect_cood(START_CHECKPOINT);
			// 白線超えてる？
			if (rc.y > WHITE_OVER) {
				// 超えとる！！黒ライン検知！
				// まずは停止
				move_forward(0, 0);
				// そしてループから脱出
				break;
			}
		}
	}

	// 取り敢えず旋回
	move_turn(90, LEFT_TURN, 50);
	wait_completion();
}
void find_obstacle(void) {
	// 取り敢えず整数最大値
	min_distance = INT16_MAX;

	// 範囲内のやつ
	int num = 0;

	// 範囲内のやつ
	for (int i=0; i<SIZE_OF_IRSensorID; i++) {
		// 取り敢えず距離取得し、リミットかける
		distance_array[i] = get_distance(i);
		distance_array[i] = limit(distance_array[i], IR_MIN, IR_MAX);

		if (distance_array[i] < min_distance) {
			// 最小値更新
			min_distance = distance_array[i];
		}

		if (distance_array[i] < IR_MAX && distance_array[i] > IR_MIN) {
			// 暴走防止の為、範囲内のセンサ数取得
			num ++;
		}
	}

	// 避ける必要ある？
	must_avoid = (min_distance < AVOIDANCE_THRESHOLD && num >1);
}
void avoidance_motion(void) {
	// どちらに避けるか
	bool turnside = (((distance_array[LEFT_IR]+distance_array[CENTER_LEFT_IR])/2) < ((distance_array[RIGHT_IR]+distance_array[CENTER_RIGHT_IR])/2));
	if (turnside) {
		// 右よけ
		move_turn(TURN_DEG, RIGHT_TURN, 50);
		wait_completion();
		move_forward(FORWARD_LEN, 50);
		wait_completion();
		move_turn(TURN_DEG, LEFT_TURN, 50);
		wait_completion();
	} else {
		// 左よけ
		move_turn(TURN_DEG, LEFT_TURN, 50);
		wait_completion();
		move_forward(FORWARD_LEN, 50);
		wait_completion();
		move_turn(TURN_DEG, RIGHT_TURN, 50);
		wait_completion();
	}
}
