#include "motor_controller.h"

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include "hal.c"
#include "usart.h"

static uint16_t half_steps_per_revolution = 0;
static uint16_t degree_per_half_step_10 = 0;
static uint16_t delay_between_half_steps_us = 0;
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
    degree_per_half_step_10 = half_step_10;
    half_steps_per_revolution = 3600 / degree_per_half_step_10;
}

void mc_set_rpm(uint16_t rpm)
{
    // 1 min = 60 000 000 us
    delay_between_half_steps_us =
        60000000 / (uint32_t)(half_steps_per_revolution * rpm);

    delay_between_steps_us = 2 * delay_between_half_steps_us;
}

void mc_step(int8_t move)
{
    static int8_t step_phase = 0;  // step_phase * 45 = electrical angle

    step_phase = (step_phase + move) % 8;

    // round up to remove neutral polarization for full step
    if (abs(move) == 2 && step_phase % 2 == 0)
    {
        step_phase = step_phase + 1;
    }

    // Windings polarization are set multiple times to keep equal timing
    // and to support switch between half and full step
    switch (step_phase)
    {
        case 0:
            // winding A positive
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 1);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 0);
            // winding B neutral
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 0);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 0);
            break;
        case 1:
            // winding A positive
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 1);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 0);
            // winding B positive
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 1);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 0);
            break;
        case 2:
            // winding A neutral
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 0);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 0);
            // winding B positive
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 1);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 0);
            break;
        case 3:
            // winding A negative
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 0);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 1);
            // winding B positive
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 1);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 0);
            break;
        case 4:
            // winding A negative
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 0);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 1);
            // winding B neutral
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 0);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 0);
            break;
        case 5:
            // winding A negative
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 0);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 1);
            // winding B negative
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 0);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 1);
            break;
        case 6:
            // winding A neutral
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 0);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 0);
            // winding B negative
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 0);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 1);
            break;
        case 7:
            // winding A positive
            hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 1);
            hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 0);
            // winding B negative
            hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 0);
            hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 1);
            break;
    }
}

void mc_step_degree(const int16_t degree)
{
    // forward: 2, backward: -2
    const int8_t move = ((degree > 0) - (degree < 0)) * 2;

    const uint16_t steps_needed =
        abs(degree * 10) / (degree_per_half_step_10 * 2);

    for (uint16_t step = 0; step < steps_needed; step++)
    {
        mc_step(move);
        delay_us(delay_between_steps_us);
    }
}

void mc_half_step_degree(const int16_t degree)
{
    // forward: 1, backward: -1
    const int8_t move = (degree > 0) - (degree < 0);

    const uint16_t half_steps_needed =
        abs(degree * 10) / degree_per_half_step_10;

    uart_putint(half_steps_needed);
    uart_puts("\r\n");

    for (uint16_t half_step = 0; half_step < half_steps_needed; half_step++)
    {
        mc_step(move);
        delay_us(delay_between_half_steps_us);
    }
}

void mc_step_for_ms(const int8_t move, const uint16_t time_ms)
{
    for (uint16_t time_passed_ms = 0; time_passed_ms <= time_ms;
         time_passed_ms += delay_between_steps_us)
    {
        mc_step(move);
        // TODO: handle full step
        delay_us(delay_between_steps_us);
    }
}

void mc_step_until(const int8_t move, bool (*callback)())
{
    while (callback())
    {
        mc_step(move);
        delay_us(delay_between_steps_us);
    }
}

void mc_release(void)
{
    hal_io_set(PORT_MOTOR_A1, PIN_MOTOR_A1, 0);
    hal_io_set(PORT_MOTOR_A2, PIN_MOTOR_A2, 0);
    hal_io_set(PORT_MOTOR_B1, PIN_MOTOR_B1, 0);
    hal_io_set(PORT_MOTOR_B2, PIN_MOTOR_B2, 0);
}
