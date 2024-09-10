#include <util/delay.h>
#include "motor_controller.h"

void led_test()
{
    // internal LED
    DDRB |= (1 << DDB5);

    while (1)
    {
        PORTB |= (1 << PORTB5);
        _delay_ms(3000);
        PORTB &= ~(1 << PORTB5);
        _delay_ms(1000);
    }
}

int main(void)
{
    //led_test();

    MotorController controller;
    mc_init(&controller, 0.08789f);
    mc_set_rpm(&controller, 10.0f);
    
    // internal LED
    DDRB |= 1 << DDB5;

    while (1)
    {
        mc_step_for_degree(&controller, 360.0f);
        PORTB ^= (1 << PORTB5);
    }
}
