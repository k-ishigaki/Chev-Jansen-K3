#ifndef MODE_CONE_H
#define MODE_CONE_H

// アーム位置
#define ARM_L_POSITION_INSERT  0	// コーンをセットできる位置
#define ARM_U_POSITION_INSERT  320	// コーンをセットできる位置
#define ARM_L_POSITION_HOLD    1000	// 持ち歩き位置
#define ARM_U_POSITION_HOLD	   220	// 掴んだ状態にする位置
#define ARM_L_POSITION_RELEASE 450	// 離す位置（ゆっくりこの値に）
#define ARM_U_POSITION_RELEASE 1100 // 離す位置（Lowerより先にこの値に）

void mode_cone_loop(void);


/**
 * コーンをすくい上げるポジションにセット
 */
void set_arm_insert(void);

/**
 * コーンを掴むポジションにセット
 *
 * upperアームをおろして，コーンを固定するポジションにセット
 */
void set_arm_grab(void);

/**
 * コーンを持ち上げる
 *
 * 持ち歩く歩くポジションで固定
 */
void set_arm_holdUp(void);

/**
 * コーンを下ろす
 */
void set_arm_release(void);


#endif /* MODE_CONE_H */
