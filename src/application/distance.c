#include "../peripheral/adc.h"
#include "distance.h"

/*
 * センサ特性
 * 白い箱（スリーピースユニット）をマシンの正面に置いて計測
 * 距離はマシン側面のアクリルの前面側の縁を基準(0cm)にしています
 * 距離[mm] LEFT CENTER_LEFT CENTER_RIGHT RIGHTの順に並べると，
 * （値はAD変換の上位8ビット，実験回数：1回）
 * 300	59	56	69	53
 * 250	69	65	73	63
 * 200	82	79	76	76
 * 150	109	105	97	103
 * 100	156	150	143	151
 * 75	166	166	167	168
 * 50	143	149	148	144
 * となったので，これを元に式を決定しています
 * また，75mm以下は正確な値が計測できません
 * 100mm以下，200mm以上は値の振れが小さいので誤差が大きくなります
 */

// 定数定義（defineでもいいけど…）
enum Constant {
	// AD変換の繰り返し回数定義
	NUM_OF_AD_AVERAGE = 10,
};

// AD変換時のチャンネル定義
enum Channels {
	LEFT         = 0b0001, // LEFTとCENTER_LEFTが順番通りではない
	CENTER_LEFT  = 0b0000,
	CENTER_RIGHT = 0b0010,
	RIGHT        = 0b0011,
};

// IRSensorIDとChannelsの対応付け
static const int channels[SIZE_OF_IRSensorID] = {
	LEFT,         // LEFT_IRに対応
	CENTER_LEFT,  // CENTER_LEFTに対応
	CENTER_RIGHT, // CENTER_RIGHTに対応
	RIGHT,        // RIGHTに対応
};


/*
 * 式について
 * AD変換(8bit)の値をxとすると，距離d[mm]は
 * 	d = 1/x * slope + intercept
 * で計算されます
 */
static const int slope[SIZE_OF_IRSensorID] = {
	18894,
	17674,
	16190,
	16289,
};

static const int intercept[SIZE_OF_IRSensorID] = {
	-24,
	-19,
	-14,
	- 9,
};


void init_distance() {
	InitADC();
}

// 100mm以下，200mm以上は正確に計測できません
int get_distance(enum IRSensorID id) {
	int ch = channels[id];
	long result = 0;
	for (int i = 0; i < NUM_OF_AD_AVERAGE; i++ ) {
		result += ADC_Solo(ch);
	}
	result /= NUM_OF_AD_AVERAGE;
	return slope[id] / result + intercept[id];
}

