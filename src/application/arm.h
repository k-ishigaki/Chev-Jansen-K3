#ifndef ARM_H
#define ARM_H

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

#endif /* ARM_H */
