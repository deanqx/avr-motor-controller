#include <avr/io.h>
#include <util/delay.h>

#define IN1_PIN PORTB0
#define IN2_PIN PORTB1
#define IN3_PIN PORTB2
#define IN4_PIN PORTB3

#define DELAY 500

typedef struct
{
    short steps_per_revolution;
    float steps_per_degree;
} MotorController;

void init_motor_controller(MotorController* controller, float steps_per_degree)
{
    controller->steps_per_revolution = (short)(360.0f / steps_per_degree);
    controller->steps_per_degree = steps_per_degree;
}

void step()
{
    DDRB |= 1 << IN1_PIN;
    DDRB &= ~(1 << IN3_PIN);
    PORTB |= 1 << IN1_PIN;
    _delay_ms(DELAY);
    PORTB &= ~(1 << IN1_PIN);

    //DDRB |= 1 << IN2_PIN;
    //DDRB &= ~(1 << IN4_PIN);
    //PORTB |= 1 << IN2_PIN;
    //_delay_ms(DELAY);
    //PORTB &= ~(1 << IN2_PIN);

    //DDRB |= 1 << IN3_PIN;
    //DDRB &= ~(1 << IN1_PIN);
    //PORTB |= 1 << IN3_PIN;
    //_delay_ms(DELAY);
    //PORTB &= ~(1 << IN3_PIN);

    //DDRB |= 1 << IN4_PIN;
    //DDRB &= ~(1 << IN2_PIN);
    //PORTB |= 1 << IN4_PIN;
    //_delay_ms(DELAY);
    //PORTB &= ~(1 << IN4_PIN);
}

void step2()
{
DDRB |= (1 << IN1_PIN);
DDRB |= (1 << IN2_PIN);
DDRB |= (1 << IN3_PIN);
DDRB |= (1 << IN4_PIN);

PORTB |= (1 << IN1_PIN);
_delay_ms(DELAY);
PORTB &= ~(1 << IN1_PIN);

PORTB |= (1 << IN2_PIN);
_delay_ms(DELAY);
PORTB &= ~(1 << IN2_PIN);

PORTB |= (1 << IN3_PIN);
_delay_ms(DELAY);
PORTB &= ~(1 << IN3_PIN);

PORTB |= (1 << IN4_PIN);
_delay_ms(DELAY);
PORTB &= ~(1 << IN4_PIN);
}

void led_test()
{
    // internal LED
    DDRB |= 1 << DDB5;

    while (1)
    {
        PORTB |= 1 << PORTB5;
        _delay_ms(3000);
        PORTB &= ~(1 << PORTB5);
        _delay_ms(1000);
    }
}

int main(void)
{
    // led_test();

    MotorController controller;
    init_motor_controller(&controller, 11.32f);

    while(1)
    {
        step2();
    }
}
