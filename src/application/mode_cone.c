#include "arm.h"
#include <util/delay.h>

// 調整用
#include "../peripheral/adc.h"
#include "serial.h"

void adj(void);

void mode_cone_loop(void){
	InitADC();

	// アーム初期化
	init_arm();

	for(;;){
		adj();

	}
}

// 調整用
void adj(void){
	int arm_l = (ADC_Solo(0)<<3);
	int arm_u = (ADC_Solo(1)<<3);

	move_arms(arm_l, arm_u);

	printf("%d,\t%d\n\r",arm_l,arm_u);

	_delay_ms(100);
}

