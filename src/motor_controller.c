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
    controller->steps_per_revolution = 360.0f / controller->degree_per_step;

    controller->step_phase = IN_PINB;

    mc_set_rpm(controller, 1.0f);

    DDRB |= (1 << IN_PINB[0]);
    DDRB |= (1 << IN_PINB[1]);
    DDRB |= (1 << IN_PINB[2]);
    DDRB |= (1 << IN_PINB[3]);
}

void mc_set_rpm(MotorController* controller, float rpm)
{
    controller->rpm = rpm;
    // 1min=60000ms
    controller->delay_between_steps_ms = 60000 / (uint16_t)(controller->steps_per_revolution * rpm);
}

void mc_step(MotorController* controller, short direction)
{
    PORTB |= (1 << *controller->step_phase);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << *controller->step_phase);

    controller->step_phase += direction;

    if (controller->step_phase > IN_PINB + 3)
    {
        controller->step_phase = IN_PINB;
    }

    if (controller->step_phase < IN_PINB)
    {
        controller->step_phase = IN_PINB + 3;
    }
}

void mc_step_for_degree(MotorController* controller, short direction, float degree)
{
    for (float degree_stepped = 0.0f; degree_stepped < degree;
         degree_stepped += controller->degree_per_step)
    {
        mc_step(controller, direction);
    }
}
