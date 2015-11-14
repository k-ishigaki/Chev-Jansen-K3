#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "arm.h"
#include "distance.h"
#include "line.h"
#include "mode.h"
#include "motion.h"
#include "serial.h"

#include "linetrace.h"
#include "mode_avoidance.h"
#include "mode_cone.h"

int main(void)
{
	DDRB = 0xff;
	DDRD = 0xff;

	init_arm();
	init_distance();
	init_line();
	init_mode();
	init_motion();
	init_serial();


	// jumper1(miso) jumper2(mosi)
	// x x line1
	// o x avoid
	// x o line2
	// o o corn
	enum ModeStatus ms = get_mode();

	while (true) {
		move(0, 10000, 120);
		_delay_ms(100);
	}
	if (ms == LINE_TRACE_1) {
		mode_linetrace();
		move_avoidance_loop();
		mode_linetrace();
		mode_cone_loop();
	} else if (ms == OBSTACLE_AVOIDANCE) {
		move_avoidance_loop();
		mode_linetrace();
		mode_cone_loop();
	} else if (ms == LINE_TRACE_1) {
		mode_linetrace();		
		mode_cone_loop();
	} else if (ms == CORN_LOADING) {
		mode_cone_loop();
	} else {
		mode_linetrace();
		move_avoidance_loop();
		mode_linetrace();
		mode_cone_loop();
	}

	for (;;) {

	}

	return 0;   /* never reached */
}
