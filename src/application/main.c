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
	init_arm();


//	for(;;){
	while(1){
		/* insert your main loop code here */

		int arm_l = (ADC_Solo(0)<<3);
		int arm_u = (ADC_Solo(1)<<3);
		int a2 = (ADC_Solo(2)<<4);
		int a3 = (ADC_Solo(3)<<4);
		move_arms(arm_l, arm_u);
		printf("%d,\t%d,\t%d,\t%d\n\r",arm_l,arm_u,a2,a3);
		_delay_ms(100);

		mode_cone_loop();




	}


	return 0;   /* never reached */
}
