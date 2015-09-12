#include "mode_cone.h"
#include "arm.h"
#include "distance.h"
#include <util/delay.h>

// 調整用
#include "../peripheral/adc.h"
#include "serial.h"

void adj(void);

void mode_cone_loop(void){
	InitADC();
	init_distance();

	// アーム初期化
	init_arm();

	for(;;){
		adj();

	}
}

// 調整用
void adj(void){
	set_arm_insert();

	int arm_l = get_lowerArm_position();// (ADC_Solo(0)<<3);
	int arm_u = get_upperArm_position();// (ADC_Solo(1)<<3);

	//move_arms(arm_l, arm_u);

	int dist_r = get_distance(CENTER_RIGHT_IR);
	int dist_l = get_distance(CENTER_LEFT_IR);

	printf("r:%d,\tl%d\n\r",dist_r,distl);
/*
	set_arm_insert();
	_delay_ms(1000);
	printf("%d,\t%d\n\r",get_lowerArm_position(),get_upperArm_position());

	set_arm_grab();
	_delay_ms(1000);
	printf("%d,\t%d\n\r",get_lowerArm_position(),get_upperArm_position());

	set_arm_grab();
	_delay_ms(1000);

	set_arm_holdUp();
	_delay_ms(1000);

	set_arm_release();
	_delay_ms(1000);
*/
}


void set_arm_insert(void){
	move_arms(ARM_L_POSITION_INSERT, ARM_U_POSITION_INSERT);
}

void set_arm_grab(void){
	move_arms(ARM_L_POSITION_INSERT, ARM_U_POSITION_HOLD);
}

void set_arm_holdUp(void){
	// ゆっくり上げる
	move_arm_lower_slow(ARM_L_POSITION_HOLD);
}

void set_arm_release(void){
	// 上側アームはリリース位置にセットしておく
	move_arms(ARM_L_POSITION_HOLD, ARM_U_POSITION_RELEASE);
	// ゆっくり下ろす
	move_arm_lower_slow(ARM_L_POSITION_RELEASE);
}