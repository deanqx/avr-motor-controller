#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "hal.c"
#include "motor_controller.h"
#include "usart.h"

#define BOTTOM_DELAY_MS 1000

bool go_down(MotorController* controller)
{
    const bool reached_top = hal_io_get(PORT_DETECT_TOP, PIN_DETECT_TOP);
    return !reached_top;
}

bool go_up(MotorController* controller)
{
    const bool reached_bottom =
        hal_io_get(PORT_DETECT_BOTTOM, PIN_DETECT_BOTTOM);
    return !reached_bottom;
}

int main(void)
{
    hal_init();

    uart_init(BAUD_CALC(115200));

    // attach uart stream to stdout & stdin
    stdout = &uart0_io;
    stdin = &uart0_io;

    MotorController controller;
    mc_init(&controller, 0.9f);

    while (1)
    {
        // Upward
        printf("Upward\r\n");
        mc_set_rpm(&controller, 148.0f);
        mc_step_for_ms(&controller, -1, 200);
        mc_set_rpm(&controller, 160.0f);
        mc_step_until(&controller, -1, go_down);

        // Downward
        printf("Downward\r\n");
        mc_set_rpm(&controller, 147.0f);
        mc_step_for_ms(&controller, 1, 300);
        mc_set_rpm(&controller, 151.0f);  // 147 - 151
        mc_step_until(&controller, 1, go_up);

        mc_stop(&controller);
        _delay_ms(BOTTOM_DELAY_MS);
    }
}
