#ifndef LINE_H
#define LINE_H

/**
 * Constants represents line state.
 */
enum LineState {
	NO_LINE,
	LEFT_EDGE,
	RIGHT_EDGE,
	T_EDGE,
	IN_LINE,
};

/**
 * Init line sensor module.
 *
 * To use get_line_state and get_line_position,
 * this function must be called first.
 */
void init_line(void);

/**
 * Get line state from line sensor.
 *
 * The state is represented by LineState enumration constants.
 * @return the line state
 */
enum LineState get_line_state(void);

/**
 * Get line position from line sensor.
 *
 * The position unit is mm.
 * When get_line_state returns NO_LINE, this function returns 0
 * (center position).
 * @return line position<br>
 * 	if line position is center, 0
 * 	if line state is no line, 0
 * 	if line position is left minus value (-50 ~ 0)
 * 	if line position is right minus value (0 ~ 50)
 */
int get_line_position(void);

#endif /* LINE_H */
