#ifndef JANSENMODEL_H
#define JANSENMODEL_H
#include <stdint.h>
#include "coordinate.h"

// マシンの中心から車輪までの距離([cnt]，1700/2pi)
#define MACHINE_RADIUS_CNT	270.5634033
// マシンの中心から車輪までの距離([mm]，0.22*1700/2pi)
#define MACHINE_RADIUS_MM	59.52394872

// 超信地旋回した時の，円周[cnt]
#define CIRC_CNT		1700
// 超信地旋回した時の，円周[mm]
#define CIRC_MM			374

// 1cntで進む距離[mm/cnt]．
#define MM_PER_CNT		0.22
// 1mmあたりカウント数[cnt/mm]．
#define CNT_PER_MM		4.545454545

// PID制御周波数([cycle/s]，20MHz/(1024分周*256カウント*(T2OVF_NUM_MAX+1)))
#define CYCLE_PER_SEC	15.25878906
// PID制御周期([s/cycle]，(1024分周*256カウント*(T2OVF_NUM_MAX+1))/20MHz)
#define SEC_PER_CYCLE	0.065536

// 記録できる仮想原点の数
#define COOD_LIST_SIZE	16

/**
 * 現在いる位置を原点とする．使い道あるかな？？
 */
void set_origin(void);
/**
 * 引数で指定された右左のカウント数だけ，座標を進めます．
 * @param cnt_r 単位時間あたりの右車輪のカウント数
 * @param cnt_l 単位時間あたりの左車輪のカウント数
 */
void move_one_step(int16_t cnt_r, int16_t cnt_l);
/**
 * 現在の位置を，相対座標取得関数で使用する，仮原点として記録します．
 * 引数で指定されたインデックスにより，この仮原点を参照します．
 * @param cood_index 仮原点のインデックス．
 */
void set_rel_origin(uint8_t cood_index);
/**
 * {@code set_origin()}で指定した点を原点とした時の現在の座標を極座標で返します．
 * @return 現在の絶対座標．
 */
PoleCood get_abs_polecood();
/**
 * {@code set_origin()}で指定した点を原点とした時の現在の座標を直交座標で返します．
 * @return 現在の絶対座標．
 */
RectCood get_abs_rectcood();
/**
 * {@code set_rel_origin(uint8_t cood_index)}で指定した座標からの相対座標を極座標で返します．
 * @param cood_index {@code set_rel_origin(uint8_t cood_index)}で指定した，仮原点のインデックス．
 * @return 指定された座標からの相対座標．
 */
PoleCood get_rel_polecood(uint8_t cood_index);
/**
 * {@code set_rel_origin(uint8_t cood_index)}で指定した座標からの相対座標を極座標で返します．
 * @param cood_index {@code set_rel_origin(uint8_t cood_index)}で指定した，仮原点のインデックス．
 * @return 指定された座標からの相対座標．
 */
RectCood get_rel_rectcood(uint8_t cood_index);

#endif /** JANSENMODEL_H */
