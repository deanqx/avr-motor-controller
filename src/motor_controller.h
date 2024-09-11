#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <avr/io.h>
#include <stdint.h>

// IN1 == IN_PINB[0]
static const uint8_t IN_PINB[4] = {PORTB0, PORTB1, PORTB2, PORTB3};

typedef struct
{
    float steps_per_revolution;
    float degree_per_step;

    // Don't modify the following variables:
    const uint8_t* step_phase;
    float rpm;
    uint16_t delay_between_steps_ms;
} MotorController;

// @param degree_per_micro_step Full Step Angle / 2
void mc_init(MotorController* controller, float degree_per_micro_step);
void mc_set_rpm(MotorController* controller, float rpm);
// @param direction forward: 1, backward: -1
void mc_step(MotorController* controller, short direction);
void mc_step_for_degree(MotorController* controller, short direction, float degree);

#endif
