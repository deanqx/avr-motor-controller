#include "motor_controller.h"

#include <stdint.h>
#include <util/delay.h>

#include "hal.c"

static uint16_t steps_per_revolution = 0;
static uint16_t degree_per_step_10 = 0;
static uint16_t rpm = 0;
static uint16_t delay_between_steps_us = 0;

void delay_us(uint16_t wait_us)
{
    for (uint16_t waited_us = 0; waited_us < wait_us; waited_us += 100)
    {
        // added small tolerance of 5 us
        _delay_us(95);
    }
}

void mc_set_half_step(uint8_t half_step_10)
{
    degree_per_step_10 = half_step_10 * 2;
    steps_per_revolution = 3600 / degree_per_step_10;
}

void mc_set_rpm(uint16_t new_rpm)
{
    rpm = new_rpm;
    // 1 min = 60 000 000 us
    delay_between_steps_us = 60000000 / (uint32_t)(steps_per_revolution * rpm);
}

void mc_calibrate(int8_t direction)
{
    mc_step(direction);
    mc_step(direction);
    mc_step(direction);
    mc_step(direction);
}

void mc_step(int8_t direction)
{
    static int8_t step_phase = 0;

    step_phase = (step_phase + direction) % 4;

    if (step_phase < 0)
    {
        step_phase = 3;
    }

    switch (step_phase)
    {
        case 0:
            hal_io_set(PORT_IN2, PIN_IN2, 0);
            hal_io_set(PORT_IN4, PIN_IN4, 0);
            //_delay_us(20.0);
            hal_io_set(PORT_IN1, PIN_IN1, 1);
            hal_io_set(PORT_IN3, PIN_IN3, 1);
            break;
        case 1:
            hal_io_set(PORT_IN1, PIN_IN1, 0);
            hal_io_set(PORT_IN4, PIN_IN4, 0);
            //_delay_us(20.0);
            hal_io_set(PORT_IN2, PIN_IN2, 1);
            hal_io_set(PORT_IN3, PIN_IN3, 1);
            break;
        case 2:
            hal_io_set(PORT_IN1, PIN_IN1, 0);
            hal_io_set(PORT_IN3, PIN_IN3, 0);
            //_delay_us(20.0);
            hal_io_set(PORT_IN2, PIN_IN2, 1);
            hal_io_set(PORT_IN4, PIN_IN4, 1);
            break;
        case 3:
            hal_io_set(PORT_IN2, PIN_IN2, 0);
            hal_io_set(PORT_IN3, PIN_IN3, 0);
            //_delay_us(20.0);
            hal_io_set(PORT_IN1, PIN_IN1, 1);
            hal_io_set(PORT_IN4, PIN_IN4, 1);
            break;
    }
}

void mc_step_degree(int16_t degree)
{
    // forward: 1, backward: -1
    int8_t direction = (degree > 0) - (degree < 0);
    uint16_t degree_abs_10 = (direction * degree) * 10;

    for (uint16_t degree_stepped_10 = 0; degree_stepped_10 < degree_abs_10;
         degree_stepped_10 += degree_per_step_10)
    {
        mc_step(direction);
        delay_us(delay_between_steps_us);
    }
}

void mc_step_for_ms(int8_t direction, uint16_t time_ms)
{
    for (uint16_t time_passed_ms = 0; time_passed_ms <= time_ms;
         time_passed_ms += delay_between_steps_us)
    {
        mc_step(direction);
        delay_us(delay_between_steps_us);
    }
}

void mc_step_until(int8_t direction, bool (*callback)())
{
    while (callback())
    {
        mc_step(direction);
        delay_us(delay_between_steps_us);
    }
}

void mc_release(void)
{
    hal_io_set(PORT_IN1, PIN_IN1, 0);
    hal_io_set(PORT_IN2, PIN_IN2, 0);
    hal_io_set(PORT_IN3, PIN_IN3, 0);
    hal_io_set(PORT_IN4, PIN_IN4, 0);
}
