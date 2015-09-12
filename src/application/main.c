/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "motion.h"
#include "coordinate.h"
#include "serial.h"
#include "mode_avoidance.h"
#include "line.h"
#include "distance.h"
#include "arm.h"


int main(void)
{
	/* insert your hardware initialization here */
	DDRB = 0xff;
	DDRD = 0xff;

	init_motion();
	init_serial();
	init_line();
	init_distance();
	init_arm();

	_delay_ms(2000);
	move_arms(5000, 5000);
	_delay_ms(2000);
	move_avoidance_loop();
	
	for(;;){
		/* insert your main loop code here */
	}


	return 0;   /* never reached */
}
