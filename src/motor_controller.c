#include "motor_controller.h"

#include <util/delay.h>

void delay_ms(uint16_t ms)
{
    while (ms--)
    {
        _delay_ms(1);
    }
}

void mc_init(MotorController* controller, float degree_per_micro_step)
{
    controller->degree_per_step = degree_per_micro_step * 2;
    controller->steps_per_revolution = (short)(360.0f / controller->degree_per_step);

    controller->step_phase = IN1_PIN;

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

void mc_step(MotorController* controller)
{
    PORTB |= (1 << controller->step_phase);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << controller->step_phase);

    switch (controller->step_phase)
    {
        case IN1_PIN:
            controller->step_phase = IN2_PIN;
            break;
        case IN2_PIN:
            controller->step_phase = IN3_PIN;
            break;
        case IN3_PIN:
            controller->step_phase = IN4_PIN;
            break;
        case IN4_PIN:
            controller->step_phase = IN1_PIN;
            break;
    }
}

void mc_step_for_degree(MotorController* controller, float degree)
{
    for (float degree_stepped = 0.0f; degree_stepped < degree;
         degree_stepped += controller->degree_per_step)
    {
        mc_step(controller);
    }
}
