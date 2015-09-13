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
#include "mode_cone.h"



int main(void)
{
	/* insert your hardware initialization here */
	DDRB = 0xff;
	DDRD = 0xff;

	init_motion();
	init_serial();

	InitADC();

	//mode_appeal_loop();

//	for(;;){
	while(1){
		/* insert your main loop code here */

		mode_cone_loop();
	}


	return 0;   /* never reached */
}
