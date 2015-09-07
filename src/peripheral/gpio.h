#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

struct DigitalPin_Direction {
	int INPUT;
	int OUTPUT;
};
extern const struct DigitalPin_Direction DigitalPin_Direction;

struct DigitalPin {
	/**
	 * Set direction of IO pin.
	 */
	void (*setDirection)(bool);

	/**
	 * Read pin state.
	 *
	 * @param true if HIGH<br>
	 * 	false if LOW<br>
	 */
	bool (*read)(void);

	/**
	 * Write digital value to output latch.
	 *
	 * @param write HIGH if true<br>
	 * 	write LOW if false<br>
	 */
	void (*write)(bool);
};

struct DigitalPins {
	const struct DigitalPin* (*PD0Pin)(void);
	const struct DigitalPin* (*PD4Pin)(void);
	const struct DigitalPin* (*PB5Pin)(void);
	const struct DigitalPin* (*PB4Pin)(void);
	const struct DigitalPin* (*PB3Pin)(void);
};
extern const struct DigitalPins DigitalPins;

#endif /* GPIO_H */
