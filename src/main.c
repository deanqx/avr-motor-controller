#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "motor_controller.h"
#include "usart.h"

void led_test()
{
    // internal LED
    DDRB |= (1 << PB5);

    while (1)
    {
        PORTB |= (1 << PB5);
        _delay_ms(3000);
        PORTB &= ~(1 << PB5);
        _delay_ms(1000);
    }
}

bool switch_top()
{
    bool button_pressed = PINC & (1 << PC0);
    return !button_pressed;
}

bool switch_buttom()
{
    bool button_pressed = PINC & (1 << PC1);
    return !button_pressed;
}

int main(void)
{
    // led_test();
    cli();

    uart_init(BAUD_CALC(115200));  // 8n1 transmission is set as default

    MotorController controller;
    mc_init(&controller, 0.08789f);
    mc_set_rpm(&controller, 5.0f);
    mc_calibrate(&controller, 1);

    // internal LED
    DDRB |= 1 << PB5;

    // Top button input
    DDRC &= ~(1 << PC0);
    // Buttom button input
    DDRC &= ~(1 << PC1);

    sei();

    while (1)
    {
        // mc_step_for_degree(&controller, 1, 360.0f);
        uart_puts("Forward\r\n");
        mc_step_until(&controller, 1, switch_top);
        PORTB ^= (1 << PB5);

        uart_puts("Backwards\r\n");
        mc_step_until(&controller, -1, switch_buttom);
        PORTB ^= (1 << PB5);
    }
}
