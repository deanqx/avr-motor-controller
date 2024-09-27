#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "motor_controller.h"
#include "usart.h"

#define TOP_BUTTON_PIN PB5
#define BUTTOM_BUTTON_PIN PB4

bool switch_top()
{
    bool button_pressed = PIND & (1 << TOP_BUTTON_PIN);
    return !button_pressed;
}

bool switch_buttom()
{
    bool button_pressed = PIND & (1 << BUTTOM_BUTTON_PIN);
    return !button_pressed;
}

int main(void)
{
    cli();

    uart_init(BAUD_CALC(115200));  // 8n1 transmission is set as default

    // attach uart stream to stdout & stdin
    stdout = &uart0_io;
    stdin = &uart0_io;

    MotorController controller;
    mc_init(&controller, 0.9f);
    mc_set_rpm(&controller, 0.1f);
    mc_calibrate(&controller, 1);

    // internal LED as direction indicator
    DDRB |= (1 << PB5);
    DDRD &= ~(1 << TOP_BUTTON_PIN);
    DDRD &= ~(1 << BUTTOM_BUTTON_PIN);

    // Set the ADC prescaler to 128 (16MHz / 128 = 125KHz)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Using AVcc as reference and select ADC0
    ADMUX |= (1 << REFS0) | ADC0D;
    // Enable the Analog Digital Converter
    ADCSRA |= (1 << ADEN);

    DDRD |= (1 << PD6);
    // Configure Timer0 for Fast PWM mode
    TCCR0A |= (1 << WGM00) | (1 << WGM01);
    // Clear OC0A on Compare Match
    TCCR0A |= (1 << COM0A1);
    // No prescaling
    TCCR0B |= (1 << CS00);

    sei();

    while (1)
    {
        // Start conversion
        ADCSRA |= (1 << ADSC);
        // Wait until conversion is complete
        while (ADCSRA & (1 << ADSC));

        // Map 1023 to 255
        uint8_t brightness = ADC / (1023 / 255);
        printf("%d; %d\r\n", ADC, brightness);

        // Set Timer0 duty cycle for PD6
        OCR0A = brightness;
    }

    while (1)
    {
        // mc_step_for_degree(&controller, 1, 360.0f);
        // printf("Forward\r\n");
        mc_step_until(&controller, 1, switch_top);
        PORTB ^= (1 << PB5);

        // printf("Backwards\r\n");
        mc_step_until(&controller, -1, switch_buttom);
        PORTB ^= (1 << PB5);
    }
}
