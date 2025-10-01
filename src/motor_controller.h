#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

// @param degree_per_micro_step Full Step Angle / 2
void mc_set_half_step(float degree_per_micro_step);
void mc_set_rpm(float rpm);
void mc_calibrate(int8_t direction);
// @param direction forward: 1, backward: -1
void mc_step(int8_t direction);
void mc_stop();
void mc_step_for_degree(int8_t direction, float degree);
void mc_step_for_ms(int8_t direction, uint16_t time_ms);
// @param callback This function returns when callback returns false
void mc_step_until(int8_t direction, bool (*callback)());
// mc_calibrate() has to be called once before

#endif
