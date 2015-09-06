#ifndef SERIAL_H
#define SERIAL_H

/**
 * Initialize serial module.
 *
 * To use printf function, {@code init_serial()} must be called first.
 */
void init_serial();

int printf(const char *fmt, ...);

#endif /* SERIAL_H */
