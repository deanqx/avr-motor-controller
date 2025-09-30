#include <avr/io.h>

#define PIN_DETECT_TOP PD5
#define PIN_DETECT_BOTTOM PD5
#define PIN_IN1 PB0
#define PIN_IN2 PB1
#define PIN_IN3 PB2
#define PIN_IN4 PB3

#define DDR_DETECT_TOP DDRD
#define DDR_DETECT_BOTTOM DDRD
#define DDR_IN1 DDRB
#define DDR_IN2 DDRB
#define DDR_IN3 DDRB
#define DDR_IN4 DDRB

#define PORT_DETECT_TOP PORTD
#define PORT_DETECT_BOTTOM PORTD
#define PORT_IN1 PORTB
#define PORT_IN2 PORTB
#define PORT_IN3 PORTB
#define PORT_IN4 PORTB

#define hal_io_set(port, pin, state) port = port & ~(1 << pin) | (state << pin)
#define hal_io_get(port, pin) port = port >> pin & 1

static const unsigned char OUTPUT = 1;
static const unsigned char INPUT = 0;

inline void hal_init(void)
{
    hal_io_set(DDR_DETECT_TOP, PIN_DETECT_TOP, INPUT);
    hal_io_set(DDR_DETECT_BOTTOM, PIN_DETECT_BOTTOM, INPUT);
    hal_io_set(DDR_IN1, PIN_IN1, OUTPUT);
    hal_io_set(DDR_IN2, PIN_IN2, OUTPUT);
    hal_io_set(DDR_IN3, PIN_IN3, OUTPUT);
    hal_io_set(DDR_IN4, PIN_IN4, OUTPUT);
}
