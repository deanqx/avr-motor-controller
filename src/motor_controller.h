#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * @param half_step_10 can be calculated by (full step angle / 2) * 10
 *
 * # Example
 * full step angle = 1.8 deg
 *
 * ```
 * mc_set_half_step(9);
 * ```
 */
void mc_set_half_step(uint8_t half_step_10);
void mc_set_rpm(uint16_t rpm);
void mc_calibrate(int8_t direction);
// @param direction forward: 1, backward: -1
void mc_step(int8_t direction);
void mc_step_degree(int16_t degree);
// @param direction forward: 1, backward: -1
void mc_step_for_ms(int8_t direction, uint16_t time_ms);
// @param direction forward: 1, backward: -1
// @param callback This function returns when callback returns false
void mc_step_until(int8_t direction, bool (*callback)());
void mc_release(void);

#endif
