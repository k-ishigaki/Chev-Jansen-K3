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

int main(void)
{
	/* insert your hardware initialization here */
	DDRB = 0xff;
	DDRD = 0xff;

	init_motion();
	init_serial();

	PoleCood pc;
	pc.phi1 = 90;
	pc.distance = 200;
	pc.phi2 = 90;

	move(-10000, 400, 75);

	for(;;){
		/* insert your main loop code here */
	}


	return 0;   /* never reached */
}
