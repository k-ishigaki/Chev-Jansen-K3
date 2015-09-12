#include "linetrace.h"

#include <stdbool.h>
#include <stdlib.h>

#include "motion.h"     // モータ制御
#include "line.h"       // ラインセンサ読み込み
#include "util/delay.h" // 制御周期調整
#include "serial.h"     // for debug

struct VelocityConstants {
	/**
	 * 初期速度
	 */
	int INITIAL;
	/**
	 * 最大速度
	 * move関数に入れる速度がこれを超えることはない
	 */
	int MAX;
	/**
	 * 最小速度
	 * 最大とは意味が違うことに注意
	 * ライン上でのライントレースでの速度がこれ以下になることはない
	 * エッジを曲がる時などはこれ以下の速度になることがある
	 */
	int MIN;
	/**
	 * エッジ速度
	 * エッジを曲がる時の速度
	 * MINより小さい値を指定してもよい
	 */
	int EDGE;
	/**
	 * 速度上昇率[10^(-3)mm/s]
	 * ラインが中央に近い場合は速度が徐々に上昇する
	 * 1サイクルでどの程度速度を足すか
	 */
	int INCREASE_RATE;
	/**
	 * 速度減少率[10^(-3)mm/s]
	 * ラインが中央から離れている場合は速度が徐々に減少する
	 * 1サイクルでどの程度速度を減らすか
	 */
	int DECREASE_RATE;
};
static const struct VelocityConstants VelocityConstants = {
	.INITIAL       = 60,
	.MAX           = 100,
	.MIN           = 60,
	.EDGE          = 30,
	.INCREASE_RATE = 100,
	.DECREASE_RATE = 500,
};

struct CurveConstants {
	/**
	 * 比例項
	 * move関数のcurvatureがget_line_position関数の戻り値に
	 * 比例するとした時の，その比例定数
	 */
	int PROPORTIONAL;
	/**
	 * 旋回曲率（左が正，右が負）
	 * 信地旋回をするときのmove関数に渡すcurvatureの値
	 * エッジ検出時に使用される
	 */
	int REVOLUTION_CURVATURE;
};
static const struct CurveConstants LeftCurveConstants = {
	.PROPORTIONAL         =  -300,
	.REVOLUTION_CURVATURE = 30000,
};
static const struct CurveConstants RightCurveConstants = {
	.PROPORTIONAL         =    500,
	.REVOLUTION_CURVATURE = -30000,
};


#define DUMMY_DISTANCE  1000
#define VELOCITY  80

#define DECELERATION_RATE 1

#define RIGHT_CURVATURE_RATE 500
#define LEFT_CURVATURE_RATE  300

#define RIGHT_EDGE_CURVATURE -30000
#define LEFT_EDGE_CURVATURE   30000
#define T_EDGE_VELOCITY        80
#define EDGE_VELOCITY          30

#define T_EDGE_DISTANCE        30
#define EDGE_DISTANCE          10

void init_linetrace() {
	init_motion();
	init_line();
	init_serial();
}

void mode_linetrace() {
	_delay_ms(10);
	// 初回なら初期化する
	static bool first = true;
	if (first) {
		init_linetrace();
		first = false;
	}
	enum LineState state = get_line_state();
	// int line = state == IN_LINE ? get_line_position : 0;
	int line;
	if (state == IN_LINE) {
		line = get_line_position();
	}
	switch (state) {
		case NO_LINE:
		case T_EDGE:
			printf("NO_LINE or T_EDGE\n");
			move(
					0,
					T_EDGE_DISTANCE,
					T_EDGE_VELOCITY);
			while (is_moving());
			enum LineState s = get_line_state();
			if (s != NO_LINE && s != T_EDGE) break;
			move(
					0,
					-100,
					-T_EDGE_VELOCITY);
			while (is_moving());
			move(
					0,
					50,
					T_EDGE_VELOCITY);
			while (is_moving());
			break;
		case LEFT_EDGE:
			printf("LEFT_EDGE\n");
			move(
					0,
					T_EDGE_DISTANCE,
					T_EDGE_VELOCITY);
			while (is_moving());
			while (1) {
				move(
						LEFT_EDGE_CURVATURE,
						EDGE_DISTANCE,
						EDGE_VELOCITY);
				while (is_moving());
				static int count = 0;
				if (get_line_state() == IN_LINE) {
					count++;
				} else {
					count = 0;
				}
				if (count >= 2) break;
			}
			break;
		case RIGHT_EDGE:
			printf("RIGHT_EDGE\n");
			move(
					0,
					T_EDGE_DISTANCE,
					T_EDGE_VELOCITY);
			while (is_moving());
			while (1) {
				move(
						RIGHT_EDGE_CURVATURE,
						EDGE_DISTANCE,
						EDGE_VELOCITY);
				while (is_moving());
				static int count = 0;
				if (get_line_state() == IN_LINE) {
					count++;
				} else {
					count = 0;
				}
				if (count >= 2) break;
			}
			break;
		case IN_LINE:
			printf("IN_LINE\n");
			if (line > 0) {
				move(
						-line * RIGHT_CURVATURE_RATE,
						DUMMY_DISTANCE,
						VELOCITY - abs(line) * DECELERATION_RATE);
			} else {
				move(
						-line * LEFT_CURVATURE_RATE,
						DUMMY_DISTANCE,
						VELOCITY - abs(line) * DECELERATION_RATE);
			}
			break;
	}

}

