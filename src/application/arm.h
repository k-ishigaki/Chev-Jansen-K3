#ifndef ARM_H
#define ARM_H

#include <util/delay.h>

/**
 * Init servo Pins for arms
 *
 * To controll the arms,
 * this function must be called first.
 */
void init_arm(void);

/**
 * Move arms while keeping machine safety
 *
 * PWM値をサーボ用レジスタに代入．
 * マシンが壊れない可動範囲内で動くことが保証される．
 */
void move_arms(int, int);

/**
 * Move an arm slowly
 *
 */
void move_arm_upper_slow(int target);
void move_arm_lower_slow(int target);

/**
 * 上側アームのポジション（書き込み値基準）を返す
 *
 * 現在のPWMの値 - BOTTOM値
 */
int get_upperArm_position(void);

/**
 * 下側アームのポジション（書き込み値基準）を返す
 *
 * 現在のPWMの値 - BOTTOM値
 */
int get_lowerArm_position(void);

#endif /* ARM_H */
