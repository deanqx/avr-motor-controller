#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <stdint.h>
#include <avr/io.h>

#define IN1_PIN PORTB0
#define IN2_PIN PORTB1
#define IN3_PIN PORTB2
#define IN4_PIN PORTB3

typedef struct
{
    uint16_t steps_per_revolution;
    float degree_per_full_step;

    // Has to be set using set_rpm()
    float rpm;
    uint16_t delay_between_steps_ms;
} MotorController;

// @param degree_per_micro_step Full Step Angle / 4
void mc_init(MotorController* controller, float degree_per_micro_step);
void mc_set_rpm(MotorController* controller, float rpm);
void mc_step_for_degree(const MotorController* controller, float degree);

#endif
