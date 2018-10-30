#include <avr/io.h>
#include <avr/interrupt.h>

#include "usiTwiSlave.h"

void initializeI2C()
{
    DDRB &= ~(1 << PIN3);
    if (PINB & (1 << PIN3))
    {
        usiTwiSlaveInit(0x01);
    }
    else
    {
        usiTwiSlaveInit(0x02);
    }
    sei();
}

void initializePWM()
{
     // Timer 0
    TCCR0A = (1<<WGM00) | (1<<WGM01) | (1<<COM0A1) | (1<<COM0B1) | (1<<COM0A0) | (1<<COM0B0); // fast PWM, set OC0A and OC0B on compare
    TCCR0B = (1<<CS00); // fast PWM, top at 0xFF, no prescaler
    OCR0A = OCR0B = 0; // duty cycle // pulse width

    // Timer 1
    TCCR1 = (1<<CS10); // no prescaler
    GTCCR = (1<<COM1B1) | (1<<PWM1B); // clear OC1B on compare
    OCR1B = 0; // duty cycle // pulse width
    OCR1C = 255; // frequency

    // Set PWM Pins as Output
    DDRB |= (1 << PIN4) | (1 << PIN1);
}

void initialize()
{
    initializeI2C();
    initializePWM();
}

void controllMotor(uint8_t command)
{
    if (command & 0x80)
    {
        OCR1B = 0;
        OCR0B = 0xFF - (command & 0x7F) * 2;
    }
    else
    {
        OCR0B = 0xFF;
        OCR1B = (command & 0x7F) * 2;
    }
}

int main()
{
    initialize();

    while (1)
    {
        controllMotor(usiTwiReceiveByte());
    }
}