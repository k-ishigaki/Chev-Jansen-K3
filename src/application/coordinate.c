#include "coordinate.h"
#include <math.h>

RectCood pole2rect(PoleCood pc) {
	// 返す直交座標の構造体．
	RectCood rc;

	// [degree] -> [rad]
	float phi1_rad = deg2rad(pc.phi1);

	// x座標[mm]
	rc.x = - pc.distance * sin(phi1_rad);
	// y座標[mm]
	rc.y = pc.distance * cos(phi1_rad);

	// 回転量[degree]
	rc.theta = pc.phi1 + pc.phi2;

	return rc;
}
PoleCood rect2pole(RectCood rc) {
	// 返す極座標の構造体．
	PoleCood pc;

	// 距離[mm]
	pc.distance = sqrt(rc.x*rc.x + rc.y*rc.y);

	// φ1[rad]
	float phi1_rad = atan2(-rc.x, rc.y);
	// [rad] -> [degree]
	pc.phi1 = rad2deg(phi1_rad);
	// φ2[degree]
	pc.phi2 = rc.theta - pc.phi1;

	return pc;
}
