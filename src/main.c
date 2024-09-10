#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define IN1_PIN PORTB0
#define IN2_PIN PORTB1
#define IN3_PIN PORTB2
#define IN4_PIN PORTB3

typedef struct
{
    uint16_t steps_per_revolution;
    float degree_per_full_step;

    // Has to be set using set_rpm()
    float rpm;
    uint16_t delay_between_steps_ms;
} MotorController;

void delay_ms(uint16_t ms)
{
    while (ms--)
    {
        _delay_ms(1);
    }
}

void mc_set_rpm(MotorController* controller, float rpm)
{
    controller->rpm = rpm;
    // 1min=60000ms
    controller->delay_between_steps_ms = 60000 / (controller->steps_per_revolution * rpm);
}

// @param degree_per_micro_step Full Step Angle / 4
void mc_init(MotorController* controller, float degree_per_micro_step)
{
    controller->steps_per_revolution = (short)(360.0f / degree_per_micro_step);
    controller->degree_per_full_step = degree_per_micro_step;

    mc_set_rpm(controller, 1.0f);

    DDRB |= (1 << IN1_PIN);
    DDRB |= (1 << IN2_PIN);
    DDRB |= (1 << IN3_PIN);
    DDRB |= (1 << IN4_PIN);
}

void mc_step_for_degree(const MotorController* controller, float degree)
{
    PORTB |= (1 << IN1_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN1_PIN);

    PORTB |= (1 << IN2_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN2_PIN);

    PORTB |= (1 << IN3_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN3_PIN);

    PORTB |= (1 << IN4_PIN);
    delay_ms(controller->delay_between_steps_ms);
    PORTB &= ~(1 << IN4_PIN);
}

void led_test()
{
    // internal LED
    DDRB |= (1 << DDB5);

    while (1)
    {
        PORTB |= (1 << PORTB5);
        delay_ms(3000);
        PORTB &= ~(1 << PORTB5);
        delay_ms(1000);
    }
}

int main(void)
{
    //led_test();

    MotorController controller;
    mc_init(&controller, 0.3515625f);
    mc_set_rpm(&controller, 20.0f);
    
    // internal LED
    DDRB |= 1 << DDB5;

    while (1)
    {
        mc_step_for_degree(&controller, 360.0f);
        PORTB ^= (1 << PORTB5);
    }
}
