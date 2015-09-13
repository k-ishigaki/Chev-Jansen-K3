#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include "motion.h"
#include "coordinate.h"
#include "distance.h"
#include "line.h"
#include "arm.h"
#include "serial.h"

// 障害物との距離がこれ以下なら避ける[mm]
#define AVOIDANCE_THRESHOLD 250
// これを超えれば白い線は無い[mm]
#define WHITE_OVER	1500
// 開始時の座標インデックス
#define START_CHECKPOINT 3
// 距離センサ最大値[mm]
#define IR_MAX	300
#define IR_MIN	100

// 直進時速度
#define FORWARD_SPD 50

// 避けるときの角度
#define TURN_STEEP_DEG	60
#define TURN_GRAND_DEG	45
#define FORWARD_LEN	120

// ラインの存在履歴の保存件数
#define LINE_HISTORY_SIZE 64
// そのうち、何回ラインがあればラインがあると判断するか
#define LINE_HISTORY_THRESHOLD 60

// 4つのセンサのうち、一番障害物までの距離が短いセンサ
enum IRSensorID min_ir;
// 障害物までの距離
int16_t min_distance;
// 障害物避ける必要あり？
bool must_avoid;
// センサー値
int16_t distance_array[SIZE_OF_IRSensorID];

// line_history[num] が true のとき、num回目にこの関数が呼びだされたときラインを読み取った．
bool line_history[LINE_HISTORY_SIZE];
// ラインステータス
enum LineState ls;

PoleCood return_move = {30, 0, 90};

/**
 * 距離センサを読み取り、変数{@code min_ir}と{@code min_distance}に格納します．
 */
void find_obstacle(void);
/**
 * 回避運動をします．
 */
void avoidance_motion(void);
/**
 * ラインを見つけます
 * @param has_avoided 直前に回避運動をした場合はtrueを指定して下さい．それ以外はfalse．
 * @return ラインがあったらtrue、それ以外はfalse
 */
bool find_line(bool has_avoided);

void print_ir(void);
void print_line(void);

void move_avoidance_loop(void) {
	printlnstr("in_avoidance");

	// アームを動かす(上げる)
	move_arms(5000, 5000);
	// 少し待つ
	_delay_ms(1000);

	printlnstr("fin_arm");

	// まずは初期位置を設定
	set_checkpoint(START_CHECKPOINT);

	printlnstr("checkpoint");


	// 取り敢えず直進させとく．
	move_forward(2000, 50);

	printlnstr("forward");

	while (1) {
		// 障害物ある？
		find_obstacle();
		if (must_avoid) {
			// あったら避ける
			avoidance_motion();
		} else if (!is_moving()) {
			// なけりゃあ直進(距離は適当)
			// ただし、少しでも時間短縮のため、動いてるなら(直進してるなら)そのまま
			move_forward(250, FORWARD_SPD);
		}

		// ライン状態取得
		if (find_line(must_avoid)) {
			// ライン発見！！
			break;
		}
		// print_line();
	}

	// 取り敢えず旋回
	// move_turn(90, LEFT_TURN, 50);
	move_to_pole(return_move, 50);
	wait_completion();

	// アームを動かす(下げる)
	move_arms(0, 0);
}
void find_obstacle(void) {
	// 閾値よりも低いセンサはtrue
	bool th_flag[SIZE_OF_IRSensorID];

	// 取り敢えず整数最大値
	min_distance = INT16_MAX;

	// 範囲内のやつ
	for (int i=0; i<SIZE_OF_IRSensorID; i++) {
		// 取り敢えず距離取得
		distance_array[i] = get_distance(i);

		// 範囲？
		th_flag[i] = (distance_array[i] < AVOIDANCE_THRESHOLD); 

		if (distance_array[i] < min_distance) {
			// 最小値更新
			min_distance = distance_array[i];
		}
	}

	// 避ける必要ある？
	must_avoid = (
		th_flag[LEFT_IR] ||
		// th_flag[RIGHT_IR] ||
		(th_flag[RIGHT_IR] && th_flag[CENTER_RIGHT_IR]) ||
		(th_flag[CENTER_RIGHT_IR] && (th_flag[CENTER_LEFT_IR] || th_flag[RIGHT_IR])) ||
		(th_flag[CENTER_LEFT_IR] && (th_flag[CENTER_RIGHT_IR] || th_flag[LEFT_IR]))
		);

	// 条件により障害物の位置を判断
	if (th_flag[LEFT_IR]) {
		min_ir = LEFT_IR;
	} else if (th_flag[CENTER_LEFT_IR] && (th_flag[CENTER_RIGHT_IR] || th_flag[LEFT_IR])) {
		min_ir = CENTER_LEFT_IR;
	} else if (th_flag[CENTER_RIGHT_IR] && (th_flag[CENTER_LEFT_IR] || th_flag[RIGHT_IR])) {
		min_ir = CENTER_RIGHT_IR;
	} else if (th_flag[RIGHT_IR] && th_flag[CENTER_RIGHT_IR]) {
		min_ir = RIGHT_IR;
	}
}
void avoidance_motion(void) {
	// どちらに避けるか

	// 避ける角度
	int avoidance_degree;
	// 避ける方向
	enum TurnDirection first_direction;
	enum TurnDirection second_direction;

	// 曲がる角度
	switch (min_ir) {
		case CENTER_LEFT_IR:
		case CENTER_RIGHT_IR:
			// 真ん中近くはいっぱい曲がる
			avoidance_degree = TURN_STEEP_DEG;
			break;

		case LEFT_IR:
		case RIGHT_IR:
		default:
			// 端っこ近くはゆるく曲がる
			avoidance_degree = TURN_GRAND_DEG;
			break;
	}

	// 曲がる方向
	switch (min_ir) {
		case RIGHT_IR:
		case CENTER_RIGHT_IR:
			// 右側に障害物
			first_direction = LEFT_TURN;
			second_direction = RIGHT_TURN;
			break;

		case LEFT_IR:
		case CENTER_LEFT_IR:
		default:
			// 右側に障害物
			first_direction = RIGHT_TURN;
			second_direction = LEFT_TURN;
			break;
	}

	// 回避動作
	move_turn(avoidance_degree, first_direction, 50);
	wait_completion();
	move_forward(FORWARD_LEN, 50);
	wait_completion();
	move_turn(avoidance_degree, second_direction, 50);
	wait_completion();
}
bool find_line(bool has_avoided) {
	// ラインを検知した座標に使う
	RectCood rc;
	// find_line()関数が呼ばれた関数
	static int num = -1;
	// ラインを過去何回読んだか
	int line_num = 0;

	num++;

	if (has_avoided || num==0) {
		// この関数が初めて呼ばれた
		// もしくはこの関数が呼ばれる直前に回避運動した
		for (int i=0; i<LINE_HISTORY_SIZE; i++) {
			line_history[i] = false;
		}
	}

	// 現在地取得
	rc = get_rect_cood(START_CHECKPOINT);
	if (rc.y < WHITE_OVER) {
		// ライン超えてない時はスルー
		return false;
	}

	// LINEの状態を読み取る
	ls = get_line_state();
	// 格納
	line_history[num%LINE_HISTORY_SIZE] = (ls == IN_LINE);

	// 回数を数える
	for (int i=0; i<LINE_HISTORY_SIZE; i++) {
		line_num += line_history[i];
	}

	printint(line_num, 10);
	printstr("\t");
	printlnint(ls, 10);

	return (line_num > LINE_HISTORY_THRESHOLD);
}
void print_ir(void) {
	if (must_avoid) {
		printstr("avoid\t");
	} else {
		printstr("     \t");
	}

	printstr("ir:");

	switch (min_ir) {
		case LEFT_IR:
			printstr("left \t");
			break;
		case RIGHT_IR:
			printstr("right\t");
			break;
		case CENTER_RIGHT_IR:
			printstr("c_rig\t");
			break;
		case CENTER_LEFT_IR:
			printstr("c_lef\t");
			break;
		default:
			printstr("xxxxx\t");
			break;
	}

	for (int i=0; i<SIZE_OF_IRSensorID; i++) {
		printint(distance_array[i], 10);
		printstr("\t");
	}

	printlnstr("");
}
void print_line() {
	// ラインを過去何回読んだか
	int line_num = 0;
	// 回数を数える
	for (int i=0; i<LINE_HISTORY_SIZE; i++) {
		line_num += line_history[i];
	}
	printint(line_num, 10);
	printstr("\t");
	printlnint(ls, 10);
}