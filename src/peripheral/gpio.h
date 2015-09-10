#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

struct DigitalPin_Direction {
	int INPUT;
	int INPUT_WITH_PULLUP;
	int OUTPUT;
};
extern const struct DigitalPin_Direction DigitalPin_Direction;

struct DigitalPin {
	/**
	 * Set direction of IO pin.
	 */
	void (*setDirection)(int);

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
	/**
	 * RXD pin.
	 */
	const struct DigitalPin* (*PD0Pin)(void);

	/**
	 * Line sensor digital input pin.
	 */
	const struct DigitalPin* (*PD4Pin)(void);

	/**
	 * SCK pin.
	 */
	const struct DigitalPin* (*PB5Pin)(void);

	/**
	 * MISO pin.
	 */
	const struct DigitalPin* (*PB4Pin)(void);

	/**
	 * MOSI pin.
	 */
	const struct DigitalPin* (*PB3Pin)(void);

	/**
	 * OC1B pin.
	 */
	const struct DigitalPin* (*PB2Pin)(void);

	/**
	 * OC1A pin.
	 */
	const struct DigitalPin* (*PB1Pin)(void);
};
extern const struct DigitalPins DigitalPins;

#endif /* GPIO_H */
