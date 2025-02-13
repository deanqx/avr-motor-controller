#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "motor_controller.h"
#include "usart.h"

#define SWITCH_TOP_PIN PD5
#define SWITCH_BOTTOM_PIN PD4

#define BOTTOM_DELAY_MIN_100MS 10
#define BOTTOM_DELAY_MAX_100MS 30

uint8_t bottom_delay = BOTTOM_DELAY_MIN_100MS;

// Get delay to wait at the bottom from poti
uint8_t get_delay_100ms()
{
    // Start conversion
    ADCSRA |= (1 << ADSC);
    // Wait until conversion is complete
    while (ADCSRA & (1 << ADSC));

    // Map 1023 to 255
    const uint8_t brightness = ADC / (1023 / 255);

    // Set Timer0 duty cycle for PD6
    OCR0A = brightness;

    return ADC * (BOTTOM_DELAY_MAX_100MS - BOTTOM_DELAY_MIN_100MS) / 1023 + BOTTOM_DELAY_MIN_100MS;
}

bool switch_top(MotorController* controller)
{
    const bool button_pressed = PIND & (1 << SWITCH_TOP_PIN);

    bottom_delay = get_delay_100ms();

    return !button_pressed;
}

bool switch_buttom(MotorController* controller)
{
    const bool button_pressed = PIND & (1 << SWITCH_BOTTOM_PIN);

    bottom_delay = get_delay_100ms();

    return !button_pressed;
}

void configure_optional_poti()
{
    // Poti reading
    // Set the ADC prescaler to 128 (16MHz / 128 = 125KHz)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Using AVcc as reference and select ADC0
    ADMUX |= (1 << REFS0) | ADC0D;
    // Enable the Analog Digital Converter
    ADCSRA |= (1 << ADEN);

    // Speed indicating LED
    DDRD |= (1 << PD6);
    // Configure Timer0 for Fast PWM mode
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    // Clear OC0A on Compare Match
    TCCR0A |= (1 << COM0A1);
    // No prescaling
    TCCR0B |= (1 << CS00);
}

int main(void)
{
    cli();

    uart_init(BAUD_CALC(115200));

    // attach uart stream to stdout & stdin
    stdout = &uart0_io;
    stdin = &uart0_io;

    MotorController controller;
    mc_init(&controller, 0.9f);

    // TODO check if used
    // internal LED as direction indicator
    DDRB |= (1 << PB5);
    DDRD &= ~(1 << SWITCH_TOP_PIN);
    DDRD &= ~(1 << SWITCH_BOTTOM_PIN);

    configure_optional_poti();

    sei();

    while (1)
    {
        // Upward
        printf("Upward\r\n");
        mc_set_rpm(&controller, 148.0f);
        mc_step_for_ms(&controller, -1, 200);
        mc_set_rpm(&controller, 160.0f);
        mc_step_until(&controller, -1, switch_top);
        printf("TOP: %d; BOTTOM: %d\r\n", switch_top(&controller), switch_buttom(&controller));

        // Downward
        printf("Downward\r\n");
        mc_set_rpm(&controller, 147.0f);
        mc_step_for_ms(&controller, 1, 300);
        mc_set_rpm(&controller, 151.0f);  // 147 - 151
        mc_step_until(&controller, 1, switch_buttom);
        printf("TOP: %d; BOTTOM: %d\r\n", switch_top(&controller), switch_buttom(&controller));

        printf("Stop for %d00ms\r\n", bottom_delay);
        mc_stop(&controller);

        for (uint8_t waited_100ms = bottom_delay; waited_100ms > 0; waited_100ms--)
        {
            _delay_ms(100.0);
        }
    }
}
