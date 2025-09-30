#include "motor_controller.h"

#include <stdint.h>
#include <util/delay.h>

#include "hal.c"
#include "usart.h"

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

    controller->step_phase = 0;

    mc_set_rpm(controller, 1.0f);
}

void mc_set_rpm(MotorController* controller, float rpm)
{
    // printf("RPM: %d\r\n", (int)rpm);

    controller->rpm = rpm;
    // 1min=60000ms
    controller->delay_between_steps_ms =
        60000 / (uint16_t)(controller->steps_per_revolution * rpm);
}

void mc_calibrate(MotorController* controller, int8_t direction)
{
    mc_step(controller, direction);
    mc_step(controller, direction);
    mc_step(controller, direction);
    mc_step(controller, direction);
}

void mc_step(MotorController* controller, int8_t direction)
{
    controller->step_phase += direction;

    if (controller->step_phase > 3)
    {
        controller->step_phase = 0;
    }
    else if (controller->step_phase < 0)
    {
        controller->step_phase = 3;
    }

    switch (controller->step_phase)
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

void mc_stop(MotorController* controller)
{
    hal_io_set(PORT_IN1, PIN_IN1, 0);
    hal_io_set(PORT_IN2, PIN_IN2, 0);
    hal_io_set(PORT_IN3, PIN_IN3, 0);
    hal_io_set(PORT_IN4, PIN_IN4, 0);
}

void mc_step_for_degree(MotorController* controller, int8_t direction,
                        float degree)
{
    for (float degree_stepped = 0.0f; degree_stepped < degree;
         degree_stepped += controller->degree_per_step)
    {
        mc_step(controller, direction);
        delay_ms(controller->delay_between_steps_ms);
    }
}

void mc_step_for_ms(MotorController* controller, int8_t direction,
                    uint16_t time_ms)
{
    for (uint16_t time_passed_ms = 0; time_passed_ms <= time_ms;
         time_passed_ms += controller->delay_between_steps_ms)
    {
        mc_step(controller, direction);
        delay_ms(controller->delay_between_steps_ms);
    }
}

void mc_step_until(MotorController* controller, int8_t direction,
                   bool (*callback)(MotorController* controller))
{
    while (callback(controller))
    {
        /*printf("\nPress s to Step and r to reverse: ");

        char command = getchar();

        if (command == 'r')
        {
            return;
        }*/

        mc_step(controller, direction);
        delay_ms(controller->delay_between_steps_ms);
    }
}

void mc_vibrate(MotorController* controller)
{
    mc_step(controller, 1);
    delay_ms(controller->delay_between_steps_ms);
    mc_step(controller, -1);
    delay_ms(controller->delay_between_steps_ms);
}
