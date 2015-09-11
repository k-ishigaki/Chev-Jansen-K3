#ifndef MODE_AVOIDANCE_H
#define MODE_AVOIDANCE_H

/**
 * 障害物回避ループです．
 * 一度目のライントレースが終わった後に呼んで下さい．
 * ラインに復帰したらreturnします．
 */
void move_avoidance_loop(void);

#endif /** MODE_AVOIDANCE_H */
