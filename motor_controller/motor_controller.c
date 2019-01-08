#include <avr/io.h>
#include <avr/interrupt.h>

#include "usi_twi_slave.h"

#ifndef address
#   define address 0x01
#endif

void initializeI2C()
{
    usiTwiSlaveInit(address);
    sei();
}

void initializePWM()
{
    // Timer 1
    TCCR1 = (1 << CS10); // no prescaler
    GTCCR = (1 << COM1B1) | (1 << PWM1B); // clear OC1B on compare
    OCR1B = 0; // duty cycle // pulse width
    OCR1C = 127; // frequency

    // Set PWM Pin as Output
    DDRB |= (1 << PIN4);
}

void initializeDirectionPins()
{
    DDRB |= (1 << PIN1) | (1 << PIN3);
    PORTB &= ~((1 << PIN1) | (1 << PIN3));
}

void initialize()
{
    initializeI2C();
    initializePWM();
    initializeDirectionPins();
}

void controlMotor(uint8_t command)
{
    OCR1B = command & 0x7F;
    if (command & 0x80)
    {
        PORTB &= ~(1 << PIN1);
        PORTB |= (1 << PIN3);
    }
    else
    {
        PORTB &= ~(1 << PIN3);
        PORTB |= (1 << PIN1);
    }
}

int main()
{
    initialize();

    while (1)
    {
        controlMotor(usiTwiReceiveByte());
    }
}
