#include <avr/io.h>
#include <avr/interrupt.h>

#include "usiTwiSlave.h"

void initializeI2C()
{
    usiTwiSlaveInit(0x01);
    sei();
}

void initializePWM()
{
    // Timer1
    TCCR1 = (1 << CS10); // no prescaler
    GTCCR = (1 << COM1B1) | (1 << PWM1B); // clear OC1B on compare
    OCR1B = 0; // initial duty cycle
    OCR1C = 127; // frequency
    
    DDRB |= (1 << PIN4); // OC1B as Output for PWM1
}

void initializeDirectionSelect()
{
    DDRB |= (1 << PIN1) | (1 << PIN3);
    PORTB &= ~((1<< PIN1) | (1 << PIN3));
}

void initialize()
{
    initializeI2C();
    initializePWM();
    initializeDirectionSelect();
}

void setMotorSpeed(uint8_t command)
{
    OCR1B = command & 0x7F;
}

void setMotorDirection(uint8_t command)
{
    if (command & 0x80)
    {
        PORTB &= ~(1 << PIN3);
        PORTB |= (1 << PIN1);
    }
    else
    {
        PORTB &= ~(1 << PIN1);
        PORTB |= (1 << PIN3);
    }
}

int main()
{
    initialize();

    uint8_t command= 0;
    while (1)
    {
        command = usiTwiReceiveByte();
        setMotorSpeed(command);
        setMotorDirection(command);
    }
}