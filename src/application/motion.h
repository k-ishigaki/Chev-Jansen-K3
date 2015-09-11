#ifndef MOTION_H
#define MOTION_H
#include <stdbool.h>
#include <stdint.h>
#include "coordinate.h"

enum TURN_DIRECTION {
	RIGHT_TURN,
	LEFT_TURN,
};

/**
 * モータ移動制御に関する全ての初期化を一括して行います．
 */
void init_motion(void);
/**
 * 引数で指定された曲率/距離/速度を移動します．
 * 各単位はUnitTable.mdに従います．
 * @param curvature 移動時の曲率．UnitTable.md準拠
 * @param distance 移動距離．UnitTable.md準拠
 * @param velocity 速度．
 */
void move(int curvature, int distance, int velocity);
/**
 * 引数で指定された座標まで，指定された速度で移動します．
 * 各単位はUnitTable.mdに従います．
 * @param 
 * @param 
 */
void move_to_pole(PoleCood pc, int velocity);
/**
 * 引数で指定された座標まで，指定された速度で移動します．
 * 各単位はUnitTable.mdに従います．
 * @param 
 * @param 
 */
void move_to_rect(RectCood rc, int velocity);
/**
 * 片側の車輪を軸にして、引数で指定された速度と角度だけ旋回します．
 * @param degree 移動する角度[degree]
 * @param turn_direction 右の車輪を停止するときはRIGHT_TURN、それ以外はLEFT_TURN
 * @param velocity 移動する速度
 */
void move_turn(int degree, enum TURN_DIRECTION turn_direction, int velocity);
/**
 * 指定された速度と距離だけ移動します．
 * @param distance 移動する距離[mm]
 * @param velocity 移動する速度
 */
void move_forward(int distance, int velocity);
/**
 * 現在移動中であるかどうかをbool型変数として返します．
 * 関数{@code move(int,int,int)}と{@code init_serial(PoleCode*,int)}，{@code init_serial(RectCode*,int)}に関して，引数で指定された動作を完了した場合は false，それ以外は true
 * @return 現在移動中であるかどうか．
 */
bool is_moving(void);
/**
 * 指定した動作が終了するのを待ちます．
 */
void wait_completion(void);
/**
 * 整数の引数で指定されたインデックスを持つチェックポイントを指定します．
 * @param 
 */
void set_checkpoint(uint8_t cood_index);
/**
 * 
 * @param 
 * @return 
 */
PoleCood get_pole_cood(uint8_t cood_index);
/**
 * 
 * @param 
 * @return 
 */
RectCood get_rect_cood(uint8_t cood_index);

#endif /** MOTION_H */
