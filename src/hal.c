#include <avr/io.h>

#define PIN_DETECT_TOP PD5
#define PIN_DETECT_BOTTOM PD5
#define PIN_MOTOR_A1 PB1  // arduino: D9
#define PIN_MOTOR_A2 PB2  // arduino: D10
#define PIN_MOTOR_B1 PB3  // arduino: D11
#define PIN_MOTOR_B2 PB4  // arduino: D12

#define DDR_DETECT_TOP DDRD
#define DDR_DETECT_BOTTOM DDRD
#define DDR_MOTOR_A1 DDRB
#define DDR_MOTOR_A2 DDRB
#define DDR_MOTOR_B1 DDRB
#define DDR_MOTOR_B2 DDRB

#define PORT_DETECT_TOP PORTD
#define PORT_DETECT_BOTTOM PORTD
#define PORT_MOTOR_A1 PORTB
#define PORT_MOTOR_A2 PORTB
#define PORT_MOTOR_B1 PORTB
#define PORT_MOTOR_B2 PORTB

#define hal_io_set(port, pin, state) port = port & ~(1 << pin) | (state << pin)
#define hal_io_get(port, pin) port = port >> pin & 1

static const unsigned char OUTPUT = 1;
static const unsigned char INPUT = 0;

static inline void hal_init(void)
{
    hal_io_set(DDR_DETECT_TOP, PIN_DETECT_TOP, INPUT);
    hal_io_set(DDR_DETECT_BOTTOM, PIN_DETECT_BOTTOM, INPUT);
    hal_io_set(DDR_MOTOR_A1, PIN_MOTOR_A1, OUTPUT);
    hal_io_set(DDR_MOTOR_A2, PIN_MOTOR_A2, OUTPUT);
    hal_io_set(DDR_MOTOR_B1, PIN_MOTOR_B1, OUTPUT);
    hal_io_set(DDR_MOTOR_B2, PIN_MOTOR_B2, OUTPUT);
}
