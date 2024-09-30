#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "motor_controller.h"
#include "usart.h"

#define SWITCH_TOP_PIN PD5
#define SWITCH_BOTTOM_PIN PD4
#define MIN_RPM 160.0f
#define MAX_RPM 300.0f
#define UPWARD_RATE 0.95f
#define SLOW_START_RATE 0.97f
#define SLOW_START_TIME_MS 300

float get_rpm_from_poti()
{
    // Start conversion
    ADCSRA |= (1 << ADSC);
    // Wait until conversion is complete
    while (ADCSRA & (1 << ADSC));

    // Map 1023 to 255
    const uint8_t brightness = ADC / (1023 / 255);

    // Set Timer0 duty cycle for PD6
    OCR0A = brightness;

    return (float)ADC * (MAX_RPM - MIN_RPM) / 1023.0f + MIN_RPM;
}

bool switch_top(MotorController* controller)
{
    mc_set_rpm(controller, get_rpm_from_poti());

    bool button_pressed = PIND & (1 << SWITCH_TOP_PIN);
    return !button_pressed;
}

bool switch_buttom(MotorController* controller)
{
    mc_set_rpm(controller, get_rpm_from_poti() * UPWARD_RATE);

    bool button_pressed = PIND & (1 << SWITCH_BOTTOM_PIN);
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
    mc_set_rpm(&controller, get_rpm_from_poti());
    mc_calibrate(&controller, 1);

    // internal LED as direction indicator
    DDRB |= (1 << PB5);
    DDRD &= ~(1 << SWITCH_TOP_PIN);
    DDRD &= ~(1 << SWITCH_BOTTOM_PIN);

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

    // Downward
    while (0)
    {
        mc_set_rpm(&controller, 147.0f);
        mc_step_for_ms(&controller, 1, 300);
        mc_set_rpm(&controller, 151.0f); // 147 - 151
        mc_step_for_ms(&controller, 1, UINT16_MAX);
    }

    // Upward
    while (1)
    {
        mc_set_rpm(&controller, 148.0f);
        mc_step_for_ms(&controller, -1, 200);
        mc_set_rpm(&controller, 160.0f);
        mc_step_for_ms(&controller, -1, UINT16_MAX);
    }

    while (1)
    {
        mc_set_rpm(&controller, get_rpm_from_poti() * SLOW_START_RATE);
        mc_step_for_ms(&controller, 1, SLOW_START_TIME_MS);
        mc_step_until(&controller, 1, switch_top);
        PORTB ^= (1 << PB5);

        mc_set_rpm(&controller, get_rpm_from_poti() * SLOW_START_RATE);
        mc_step_for_ms(&controller, -1, SLOW_START_TIME_MS);
        mc_step_until(&controller, -1, switch_buttom);
        PORTB ^= (1 << PB5);
    }
}
