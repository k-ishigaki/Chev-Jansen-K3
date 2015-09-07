/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "motion.h"


int main(void)
{
	/* insert your hardware initialization here */
	DDRB = 0xff;
	DDRD = 0xff;

	init_motion();

	for(;;){
		/* insert your main loop code here */
		test(30);
		_delay_ms(2000);
		test(15);
		_delay_ms(2000);
	}


	return 0;   /* never reached */
}
