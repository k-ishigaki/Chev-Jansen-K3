#include "linetrace.h"

#include <stdbool.h>
#include <stdlib.h>

#include "motion.h"     // モータ制御
#include "line.h"       // ラインセンサ読み込み
#include "util/delay.h" // 制御周期調整
#include "serial.h"     // for debug

#define CURVETURE_OFFSET -1000

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
	.INITIAL       = 80,
	.MAX           = 100,
	.MIN           = 80,
	.EDGE          = 40,
	.INCREASE_RATE = 100,
	.DECREASE_RATE = 200,
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
	/**
	 * 減速率[10^(-3)mm/s]
	 * get_line_positionの絶対値に掛けて，その分減速する
	 */
	int DECELERATION_RATE;
};
static const struct CurveConstants LeftCurveConstants = {
	.PROPORTIONAL         =   300,
	.REVOLUTION_CURVATURE = 30000,
	.DECELERATION_RATE    =  500,

};
static const struct CurveConstants RightCurveConstants = {
	.PROPORTIONAL         =    300,
	.REVOLUTION_CURVATURE = -30000,
	.DECELERATION_RATE    =   500,
};


#define DUMMY_DISTANCE  1000

#define T_EDGE_DISTANCE        10
#define EDGE_DISTANCE          5

struct Velocity {
	void (*initialize)(void);
	void (*decrease)(void);
	void (*increase)(void);
	int (*getValue)(void);
};
static long velocity;
static void Velocity_initialize() {
	velocity = (long)VelocityConstants.INITIAL * 1000;
}
static void Velocity_decrease() {
	velocity -= VelocityConstants.DECREASE_RATE;
	if (velocity < (long)VelocityConstants.MIN * 1000) {
		velocity = (long)VelocityConstants.MIN * 1000;
	}
}
static void Velocity_increase() {
	velocity += VelocityConstants.INCREASE_RATE;
	if (velocity > (long)VelocityConstants.MAX * 1000) {
		velocity = (long)VelocityConstants.MAX * 1000;
	}
}
static int Velocity_getValue() {
	return velocity / 1000;
}
static const struct Velocity Velocity = {
	.initialize = Velocity_initialize,
	.decrease   = Velocity_decrease,
	.increase   = Velocity_increase,
	.getValue   = Velocity_getValue,
};

void init_linetrace() {
	init_motion();
	init_line();
	init_serial();
	Velocity.initialize();
}

void linetrace() {
	if (abs(line) < 20) {
		Velocity.increase();
	} else {
		Velocity.decrease();
	}
	if (line > 0) {
		move(
				-line * RightCurveConstants.PROPORTIONAL + CURVETURE_OFFSET,
				DUMMY_DISTANCE,
				Velocity.getValue() - (long)abs(line) * RightCurveConstants.DECELERATION_RATE / 1000);
	} else {
		move(
				-line * LeftCurveConstants.PROPORTIONAL + CURVETURE_OFFSET,
				DUMMY_DISTANCE,
				Velocity.getValue() - (long)abs(line) * LeftCurveConstants.DECELERATION_RATE / 1000);
	}
}

void mode_linetrace() {
	while (1) {
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
				break;
			case T_EDGE:
				move(
						0,
						T_EDGE_DISTANCE,
						Velocity.getValue());
				while (is_moving());
				if (get_line_state() != T_EDGE) break;
				move(
						2000,
						100,
						-VelocityConstants.INITIAL);
				while (is_moving());
				move(
						1000,
						120,
						VelocityConstants.INITIAL);
				while (is_moving());
				if (get_line_state() != T_EDGE) break;
				return;
				break;
			case LEFT_EDGE:
				move(
						0,
						T_EDGE_DISTANCE,
						VelocityConstants.MIN);
				while (is_moving());
				if (get_line_state() != LEFT_EDGE) break;
				while (1) {
					move(
							LeftCurveConstants.REVOLUTION_CURVATURE,
							EDGE_DISTANCE,
							VelocityConstants.EDGE);
					while (is_moving());
					static int count = 0;
					if (get_line_state() == IN_LINE) {
						count++;
					} else {
						count = 0;
					}
					if (count >= 4) break;
				}
				Velocity.initialize();
				break;
			case RIGHT_EDGE:
				move(
						0,
						T_EDGE_DISTANCE,
						VelocityConstants.MIN);
				while (is_moving());
				if (get_line_state() != RIGHT_EDGE) break;
				while (1) {
					move(
							RightCurveConstants.REVOLUTION_CURVATURE,
							EDGE_DISTANCE,
							VelocityConstants.EDGE);
					while (is_moving());
					static int count = 0;
					if (get_line_state() == IN_LINE) {
						count++;
					} else {
						count = 0;
					}
					if (count >= 4) break;
				}
				Velocity.initialize();
				break;
			case IN_LINE:
				linetrace();
				break;
		}
	}
}

