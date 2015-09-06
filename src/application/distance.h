#ifndef DISTANCE_H
#define DISTANCE_H

/**
 * Distance sensor id.
 *
 * Distance sensors are consist of four IR distance sensor (GP2Y0A21YK)
 * You can obtain distance value in mm unit by calling {@code get_distance}
 * function.
 * The function reqires IR sensor id constant so that identify these
 * positions. For example: {@code
 * init_distance(); // initialize distance module.
 * int left = get_distance(LEFT_IR); // get distance sensor fixed at left
 * }
 */
enum IRSensorID {
	LEFT_IR            = 0,
	CENTER_LEFT_IR     = 1,
	CENTER_RIGHT_IR    = 2,
	RIGHT_IR           = 3,
	SIZE_OF_IRSensorID = 4,
};

/** Initialize distance module. */
void init_distance(void);

/**
 * Get distance in mm.
 *
 * This function includes ad conversion process.
 * So it is recommended not to call this function in interrupt.
 * To get distance value of four sensors, below method is can be used.
 * {@code
 * int results[SIZE_OF_IRSensorID];
 * init_distance(); // initialize distance module
 * for (int i = 0; i < SIZE_OF_IRSensorID; i++) {
 * 	results = get_distance(i);
 * }
 * if (results[LEFT_IR]) ... // check left distance value
 * }
 *
 * @param ir sensor id constant
 * @return distance obtained from the sensor<br>
 */
int get_distance(enum IRSensorID num);

#endif /** DISTANCE_H */
