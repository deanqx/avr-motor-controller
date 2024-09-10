#include <util/delay.h>
#include "motor_controller.h"

void delay_ms(uint16_t ms)
{
    while (ms--)
    {
        _delay_ms(1);
    }
}

void mc_init(MotorController* controller, float degree_per_micro_step)
{
    controller->steps_per_revolution = (short)(360.0f / degree_per_micro_step);
    controller->degree_per_full_step = degree_per_micro_step;

    mc_set_rpm(controller, 1.0f);

    DDRB |= (1 << IN1_PIN);
    DDRB |= (1 << IN2_PIN);
    DDRB |= (1 << IN3_PIN);
    DDRB |= (1 << IN4_PIN);
}

void mc_set_rpm(MotorController* controller, float rpm)
{
    controller->rpm = rpm;
    // 1min=60000ms
    controller->delay_between_steps_ms = 60000 / (controller->steps_per_revolution * rpm);
}

void mc_step_for_degree(const MotorController* controller, float degree)
{
    PORTB |= (1 << IN1_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN1_PIN);

    PORTB |= (1 << IN2_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN2_PIN);

    PORTB |= (1 << IN3_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN3_PIN);

    PORTB |= (1 << IN4_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN4_PIN);
}
