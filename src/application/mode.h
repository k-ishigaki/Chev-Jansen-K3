#ifndef MODE_H
#define MODE_H

/**
 * Mode status constant.
 *
 * The mode is settable from four modes.
 */
enum ModeStatus {
	LINE_TRACE_1,
	OBSTACLE_AVOIDANCE,
	LINE_TRACE_2,
	CORN_LOADING,
};

/**
 * Initialize mode module.
 *
 * This module uses ISP pins (SCK, MISO, MOSI), so a flash writer
 * must be disconnected if mode switch is used.
 */
void init_mode(void);

/**
 * Get mode status form input pins.
 *
 * @return mode status constant
 */
enum ModeStatus get_mode(void);

#endif /* MODE_H */
