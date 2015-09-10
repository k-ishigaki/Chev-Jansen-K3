#ifndef COORDINATE_H
#define COORDINATE_H

// 360/2pi[degree/rad]
#define DEG_PER_RAD	57.29577951
// 2pi/360[rad/degree]
#define RAD_PER_DEG	0.01745329252

// [rad] -> [rad] 変換マクロ
#define deg2rad(deg)	(deg*RAD_PER_DEG)
// [rad] -> [deg] 変換マクロ
#define rad2deg(rad)	(rad*DEG_PER_RAD)

/**
 * 極座標を表す構造体．
 */
typedef struct {
	int distance;
	int phi1;
	int phi2;
} PoleCood;
/**
 * 直交座標を表す構造体．
 */
typedef struct {
	int x;
	int y;
	int theta;
} RectCood;

/**
 * 極座標系から直交座標系に変換します．
 * @param pc 変換する元の極座標構造体．
 * @return 変換した直交座標構造体．
 */
RectCood pole2rect(PoleCood pc);
/**
 * 直交座標系から極座標系に変換します．
 * @param rc 変換する元の直交座標構造体．
 * @return 変換した極座標構造体．
 */
PoleCood rect2pole(RectCood rc);

#endif /** COORDINATE_H */
