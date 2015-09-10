#ifndef MOTOR_H
#define MOTOR_H

/**
 * モータPWMを発生させるためのTimer0初期設定を行います．
 */
void init_timer0(void);
/**
 * モータPID制御を行うためのTimer2初期設定を行います．
 * これにより，割り込み関数{@code ISR(TIMER2_OVF_vect)}は、約13msec周期(76.3Hz)で呼びだされます．
 * また，PID制御については，
 */
void init_timer2(void);
/**
 * ロータリエンコーダ割り込みを制御するための初期化を行います．
 */
void init_intr(void);
/**
 * Timer2割り込み時にコールバックする関数(引数は右左のロータリエンコーダのカウント値)を指定します．
 */
void set_t2intr_callback(void (*f)(int8_t, int8_t));
/**
 * モータに出力するPWMの値を設定します．
 * @param r_pwm 右モータに入力するPWMの値()
 */
void set_motor_pwm(int16_t r_pwm, int16_t l_pwm);

#endif /** MOTOR_H */
