#include <avr/interrupt.h>
#include <avr/io.h>
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

int main(void)
{
    hal_init();

    uart_init(BAUD_CALC(115200));

    // attach uart stream to stdout & stdin
    stdout = &uart0_io;
    stdin = &uart0_io;

    mc_set_half_step(0.9f);
    mc_set_rpm(80.0f);

    mc_step_for_degree(-1, 180.0f);

    while (1)
    {
    }
}
