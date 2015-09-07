#include "line.h"
#include "../peripheral/gpio.h"
#include "../peripheral/adc.h"

/*
 * ラインセンサの反応について
 *
 * ラインの位置がまったくリニアには出力されていません
 * 中心こそ2とか-4とかの値を出しますが、
 * 10, -10あたりをすっ飛ばして22とか-20あたりの数を
 * いきなり返してきます
 * ライントレースもこれを踏まえてあまりきれいなライントレースを
 * 考えないほうがいいと思います
 */

static const struct DigitalPin* digitalInput;

enum Constant {
	AD_CHANNEL = 5,
};

void init_line() {
	digitalInput = DigitalPins.PD4Pin();
	digitalInput->setDirection(DigitalPin_Direction.INPUT);
	InitADC();
}

enum LineState get_line_state() {
	if (digitalInput->read() == false) {
		// LOWの時はライン上
		return IN_LINE;
	}
	// HIGHの時はAD変換をして、
	// 上位2ビットで分岐する
	unsigned int value = ADC_Solo(AD_CHANNEL);
	switch ((value >> 6) & 0b11) {
		case 0b00:
			return NO_LINE;
		case 0b01:
			return RIGHT_EDGE;
		case 0b10:
			return LEFT_EDGE;
		case 0b11:
			return T_EDGE;
	}
	return NO_LINE;
}

int get_line_position() {
	if (digitalInput->read() == false) {
		// ライン上のときは中心からの位置を返す
		// 適当な式です
		int position =
			((int)ADC_Solo(AD_CHANNEL) - 128) * 2 / 5;
		return position;
	} else {
		// ライン上でないときは0（中心）を返す
		return 0;
	}
}

