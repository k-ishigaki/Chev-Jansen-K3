#include "mode.h"
#include "../peripheral/gpio.h"
#include <stdbool.h>

static const struct DigitalPin* sckPin;
static const struct DigitalPin* misoPin;
static const struct DigitalPin* mosiPin;

void init_mode() {
	sckPin  = DigitalPins.PB5Pin();
	misoPin = DigitalPins.PB4Pin();
	mosiPin = DigitalPins.PB3Pin();
	// SCKピンはLOWを出力
	sckPin->setDirection(DigitalPin_Direction.OUTPUT);
	sckPin->write(false);
	// MISOとMOSIはプルアップに設定
	misoPin->setDirection(DigitalPin_Direction.INPUT_WITH_PULLUP);
	mosiPin->setDirection(DigitalPin_Direction.INPUT_WITH_PULLUP);
}

enum ModeStatus get_mode() {
	// jumperxはジャンパが付いている時にtrue
	bool jumper1 = !(misoPin->read());
	bool jumper2 = !(mosiPin->read());
	if (jumper2 == false) {
		if (jumper1 == false) {
			return LINE_TRACE_1;
		} else {
			return OBSTACLE_AVOIDANCE;
		}
	} else {
		if (jumper1 == false) {
			return LINE_TRACE_2;
		} else {
			return CORN_LOADING;
		}
	}
}

