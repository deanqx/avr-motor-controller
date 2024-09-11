#include <avr/io.h>
#include <util/delay.h>

#include "motor_controller.h"

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

    while (1)
    {
        // mc_step_for_degree(&controller, 1, 360.0f);
        mc_step_until(&controller, 1, switch_top);
        PORTB ^= (1 << PB5);
        mc_step_until(&controller, -1, switch_buttom);
        PORTB ^= (1 << PB5);
    }
}
