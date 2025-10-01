#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "hal.c"
#include "motor_controller.h"
#include "usart.h"

bool reached_pull_limit()
{
    const bool reached_end = hal_io_get(PORT_DETECT_TOP, PIN_DETECT_TOP);
    return !reached_end;
}

bool reached_retracted_limit()
{
    const bool reached_end = hal_io_get(PORT_DETECT_BOTTOM, PIN_DETECT_BOTTOM);
    return !reached_end;
}

void set_position_with_uart()
{
    printf("Set motor position with these commands:\r\n");
    printf("Up: u\r\n");
    printf("Down: d\r\n");
    printf("Exit: e\r\n");

    while (1)
    {
        const char command = uart_getc();

        if (command == 'u')  // Up
        {
            mc_step_degree(10);
        }
        else if (command == 'd')  // Down
        {
            mc_step_degree(-10);
        }
        else if (command == 'e')  // exit
        {
            break;
        }
    }
}

void find_best_rpm(void)
{
    for (uint16_t rpm = 40;; rpm += 5)
    {
        uart_putint(rpm);
        uart_puts("\r\n");
        mc_set_rpm(rpm);
        mc_step_degree(-10);
        _delay_ms(500);
    }
}

int main(void)
{
    hal_init();

    uart_init(BAUD_CALC(19200));

    // attach uart stream to stdout & stdin
    stdout = &uart0_io;
    stdin = &uart0_io;

    mc_set_half_step(9);
    mc_set_rpm(95);

    set_position_with_uart();

    find_best_rpm();

    set_position_with_uart();

    while (1)
    {
        mc_step_degree(90);
        _delay_ms(500);
        mc_step_degree(-90);
        _delay_ms(500);
    }
}
