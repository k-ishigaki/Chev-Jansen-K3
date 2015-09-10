#include "jansenmodel.h"
#include <math.h>

// 基準からのxy座標[cnt]．ただし，計算の都合上2倍になっている．
int16_t x_mul_2;
int16_t y_mul_2;
// 基準からの角度[Rad]．
float theta;

// 仮原点を記録するための配列
RectCood cood_array[COOD_LIST_SIZE];

/**
 * 引数で指定された値からRectCoodを生成して返します．
 * @param x 座標x成分[cnt]．
 * @param y 座標x成分[cnt]．
 * @param rad 角度[rad]．
 * @return 指定された座標に対応するRectCood構造体
 */
static RectCood cnt2rect(int16_t x, int16_t y, float rad);

void set_origin(void) {
	x_mul_2 = 0;
	y_mul_2 = 0;
	theta = 0.0;
}
void move_one_step(int16_t cnt_r, int16_t cnt_l) {
	// 移動距離(の2倍)
	int16_t delta_distance;
	// 角度の変化
	float delta_theta;

	// 移動距離を取得(2倍，[cnt])
	delta_distance = (cnt_r + cnt_l);
	// 角度変位を取得([rad])
	delta_theta = (cnt_r - cnt_l) / (2.0 * MACHINE_RADIUS_CNT);

	// 現在のxy座標計算(2倍，[cnt])
	x_mul_2 += delta_distance * cos(theta + (delta_theta/2.0));
	y_mul_2 += delta_distance * sin(theta + (delta_theta/2.0));
	// 現在の角度計算([rad])
	theta += delta_theta;
}
void set_rel_origin(uint8_t cood_index) {
	// index out of range
	if (cood_index < 0 || cood_index >= COOD_LIST_SIZE) {
		return;
	}

	// 記録する座標
	RectCood rc;
	// 計算の都合上2倍になっているので2で割ってから代入．
	rc = cnt2rect((x_mul_2>>1), (y_mul_2>>1), theta);

	// 配列に記録しとく
	cood_array[cood_index] = rc;
}
PoleCood get_abs_polecood() {
	// まずはRectCoodで取得
	RectCood rc;
	rc = get_abs_rectcood();

	// 変換して返す．
	return rect2pole(rc);
}
RectCood get_abs_rectcood() {
	// 返す座標構造体
	RectCood rc;

	// 計算の都合上2倍になっているので2で割ってから代入．
	rc = cnt2rect((x_mul_2>>1), (y_mul_2>>1), theta);

	return rc;
}
PoleCood get_rel_polecood(uint8_t cood_index) {
	// index out of range
	if (cood_index < 0 || cood_index >= COOD_LIST_SIZE) {
		PoleCood pc = {0, 0, 0};
		return pc;
	}

	// まずはRectCoodで取得
	RectCood rc;
	rc = get_rel_rectcood(cood_index);

	// 変換して返す．
	return rect2pole(rc);
}
RectCood get_rel_rectcood(uint8_t cood_index) {
	// index out of range
	if (cood_index < 0 || cood_index >= COOD_LIST_SIZE) {
		RectCood rc = {0, 0, 0};
		return rc;
	}

	// 返す座標構造体
	RectCood rc;

	// 現在の座標を取得．
	rc = get_abs_rectcood();
	// 指定された仮原点との差分を返す．
	rc.x -= cood_array[cood_index].x;
	rc.y -= cood_array[cood_index].y;
	rc.theta -= cood_array[cood_index].theta;

	return rc;
}

static RectCood cnt2rect(int16_t x, int16_t y, float rad) {
	// 記録する座標
	RectCood rc;

	// 1[cnt] -> 0.22[mm]
	rc.x = x * MM_PER_CNT;
	rc.y = y * MM_PER_CNT;

	// 1[rad] -> 360/2pi[degree]
	rc.theta = rad * DEG_PER_RAD;

	return rc;	
}
