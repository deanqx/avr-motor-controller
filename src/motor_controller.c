#include "motor_controller.h"

#include <stdint.h>
#include <util/delay.h>

#include "hal.c"
#include "usart.h"

static float steps_per_revolution = 0;
static float degree_per_step = 0;
static float rpm = 0;
static uint16_t delay_between_steps_ms = 0;

void mc_init(float degree_per_micro_step)
{
    degree_per_step = degree_per_micro_step * 2;
    steps_per_revolution = 360.0f / degree_per_step;

    mc_set_rpm(1.0f);
}

void mc_set_rpm(float new_rpm)
{
    // printf("RPM: %d\r\n", (int)rpm);

    rpm = new_rpm;
    // 1min=60000ms
    delay_between_steps_ms = 60000 / (uint16_t)(steps_per_revolution * rpm);
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

    step_phase += direction;

    if (step_phase > 3)
    {
        step_phase = 0;
    }
    else if (step_phase < 0)
    {
        step_phase = 3;
    }

    switch (step_phase)
    {
        // Turning off first is required
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

void mc_stop()
{
    hal_io_set(PORT_IN1, PIN_IN1, 0);
    hal_io_set(PORT_IN2, PIN_IN2, 0);
    hal_io_set(PORT_IN3, PIN_IN3, 0);
    hal_io_set(PORT_IN4, PIN_IN4, 0);
}

void mc_step_for_degree(int8_t direction, float degree)
{
    for (float degree_stepped = 0.0f; degree_stepped < degree;
         degree_stepped += degree_per_step)
    {
        mc_step(direction);
        _delay_ms(delay_between_steps_ms);
    }
}

void mc_step_for_ms(int8_t direction, uint16_t time_ms)
{
    for (uint16_t time_passed_ms = 0; time_passed_ms <= time_ms;
         time_passed_ms += delay_between_steps_ms)
    {
        mc_step(direction);
        _delay_ms(delay_between_steps_ms);
    }
}

void mc_step_until(int8_t direction, bool (*callback)())
{
    while (callback())
    {
        /*printf("\nPress s to Step and r to reverse: ");

        char command = getchar();

        if (command == 'r')
        {
            return;
        }*/

        mc_step(direction);
        _delay_ms(delay_between_steps_ms);
    }
}

void mc_vibrate()
{
    mc_step(1);
    _delay_ms(delay_between_steps_ms);
    mc_step(-1);
    _delay_ms(delay_between_steps_ms);
}
