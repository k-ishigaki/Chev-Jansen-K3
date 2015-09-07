#include <avr/io.h>
#include "gpio.h"

#ifndef COMMON_DECLARATION
#define COMMON_DECLARATION

enum DigitalPin_Direction_Constant {
	DIGITAL_INPUT,
	DIGITAL_OUTPUT,
};
const struct DigitalPin_Direction DigitalPin_Direction = {
	DIGITAL_INPUT,
	DIGITAL_OUTPUT,
};

extern const struct DigitalPin* PD0Pin_constructor();
extern const struct DigitalPin* PD4Pin_constructor();
extern const struct DigitalPin* PB5Pin_constructor();
extern const struct DigitalPin* PB4Pin_constructor();
extern const struct DigitalPin* PB3Pin_constructor();

const struct DigitalPins DigitalPins = {
	PD0Pin_constructor,
	PD4Pin_constructor,
	PB5Pin_constructor,
	PB4Pin_constructor,
	PB3Pin_constructor,
};

#endif /* COMMON_DECLARATION */

#if !defined(PD0_DECLARATION)
#define PD0_DECLARATION

#define PxxPin_(name) PD0Pin_##name
#define Pxx           PD0
#define DDRx          DDRD
#define PORTx         PORTD
#define PINx          PIND
#define PINxx         PIND0

#elif !defined(PD4_DECLARATION)
#define PD4_DECLARATION

#define PxxPin_(name) PD4Pin_##name
#define Pxx           PD4
#define DDRx          DDRD
#define PORTx         PORTD
#define PINx          PIND
#define PINxx         PIND4

#elif !defined(PB5_DECLARATION)
#define PB5_DECLARATION

#define PxxPin_(name) PB5Pin_##name
#define Pxx           PB5
#define DDRx          DDRB
#define PORTx         PORTB
#define PINx          PINB
#define PINxx         PINB5

#elif !defined(PB4_DECLARATION)
#define PB4_DECLARATION

#define PxxPin_(name) PB4Pin_##name
#define Pxx           PB4
#define DDRx          DDRB
#define PORTx         PORTB
#define PINx          PINB
#define PINxx         PINB4

#elif !defined(PB3_DECLARATION)
#define PB3_DECLARATION

#define PxxPin_(name) PB3Pin_##name
#define Pxx           PB3
#define DDRx          DDRB
#define PORTx         PORTB
#define PINx          PINB
#define PINxx         PINB3

#define DECLARATION_COMPLITE
#endif

static void PxxPin_(setDirection)(bool direction) {
	switch ((enum DigitalPin_Direction_Constant)direction) {
		case DIGITAL_INPUT:
			// DDRxは0で入力
			DDRx &= ~(_BV(Pxx));
			break;
		case DIGITAL_OUTPUT:
			// DDRxは1で出力
			DDRx |= _BV(Pxx);
			break;
	}
}

static bool PxxPin_(read)() {
	// bit_is_set
	// 指定レジスタの指定のビットがセット(1)されているかどうかチェック
	return bit_is_set(PINx, PINxx);
}

static void PxxPin_(write)(bool value) {
	if (value == 0) {
		PORTx &= ~(_BV(Pxx));
	} else {
		PORTx |= _BV(Pxx);
	}
}

static const struct DigitalPin PxxPin_(instance) = {
	PxxPin_(setDirection),
	PxxPin_(read),
	PxxPin_(write),
};

const struct DigitalPin* PxxPin_(constructor)() {
	return &PxxPin_(instance);
}

#undef PxxPin_
#undef Pxx
#undef DDRx
#undef PORTx
#undef PINx
#undef PINxx

#ifndef DECLARATION_COMPLITE
#include "gpio.c"
#endif /* DECLARATION_COMPLITE */
