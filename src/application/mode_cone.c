#include "mode_cone.h"
#include "arm.h"
#include "distance.h"
#include <util/delay.h>
#include "motion.h"

// 調整用
#include "../peripheral/adc.h"
#include "serial.h"
#define DIST_R_far   160
#define DIST_R_near  140
#define DIST_L_far   115
#define DIST_L_near  100

#define DEST_R_REL 200	//以下
#define DEST_L_REL 200	//以下

void adj(void);
void motionTest(void);

void mode_cone_loop(void){
	InitADC();
	init_distance();
	init_serial();
	init_motion();

	// アーム初期化
	init_arm();

	motionTest();

	for(;;){
		//adj();


	}
}

void mode_appeal(void){
	//コーンを取れる位置にセットしておくこと．

	// アームを上にあげておく
	set_arm_insert();
	_delay_ms(4000);

	// 5cmほど前進
	move_forward(50, 40);
	wait_completion();

	// アームを下げる
	set_arm_grab();
	_delay_ms(300);

	// アームを上げる
	set_arm_holdUp();
	_delay_ms(300);

	// 15cmほど前進
	move_forward(150, 40);
	wait_completion();

	// コーンを放す
	set_arm_release();
	_delay_ms(300);
}

void motionTest(){
	int dist_r = get_distance(CENTER_RIGHT_IR);
	int dist_l = get_distance(CENTER_LEFT_IR);


	// アーム上にあげとく
	set_arm_insert();
	_delay_ms(1000);

	// 直進（ライントレース）
	// 45cm程度進んだら左折

	// 67.5cm以上直進するとコーンがある

	// 60cmぐらい直進
	move(0,600, 40);
	wait_completion();

	// になるまでちょっとずつ進む
	dist_r = get_distance(CENTER_RIGHT_IR);
	dist_l = get_distance(CENTER_LEFT_IR);



	// 掴む
	set_arm_grab();

	// 持ち上げる
	set_arm_holdUp();

	// 後ろに下がる？
	//move(0, -100, 40);
	//wait_completion();

	// 旋回
	move_turn(90, LEFT_TURN, 40);

	// 直進
	move_forward(300,40);

	// ちょっと探してみる

	// 下ろす


/* フィードバックなし
	int dist_r = get_distance(CENTER_RIGHT_IR);
	int dist_l = get_distance(CENTER_LEFT_IR);
	while( ! ((DIST_R_near < dist_r) && ( dist_r < DIST_R_far)) &&
		   ((DIST_L_near < dist_l) && ( dist_l < DIST_L_far)))

		   if (dist_r > DIST_R_far ){
		   	move_turn
		   }
	)
*/
}

// 調整用
void adj(void){

	set_arm_insert();

	//int arm_l = get_lowerArm_position();// (ADC_Solo(0)<<3);
	//int arm_u = get_upperArm_position();// (ADC_Solo(1)<<3);

	//move_arms(arm_l, arm_u);

	int dist_r = get_distance(CENTER_RIGHT_IR);
	int dist_l = get_distance(CENTER_LEFT_IR);

	printf("r:%d,\tl%d\n\r",dist_r,dist_l);

	_delay_ms(100);
	set_arm_holdUp();
	_delay_ms(300);

	set_arm_release();
	_delay_ms(300);

	set_arm_insert();
	_delay_ms(300);
	//printf("%d,\t%d\n\r",get_lowerArm_position(),get_upperArm_position());

	set_arm_grab();
	_delay_ms(300);
	//printf("%d,\t%d\n\r",get_lowerArm_position(),get_upperArm_position());

	set_arm_grab();
	_delay_ms(300);


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