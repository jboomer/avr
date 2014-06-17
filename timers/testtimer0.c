#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
    OCR1A = 31249;

    DDRB |= 0x20;
    // Set PB5 as output

    TCCR1B |= (1 << WGM12);
    // Mode 4, CTC on OCR1A

    TIMSK1 |= (1 << OCIE1A);
    //Set interrupt on compare match

    TCCR1B |= (1 << CS12) | (1 << CS10);
    // set prescaler to 1024 and start the timer


    sei();
    // enable interrupts


    while (1);
    {
        // we have a working Timer
    }
}

ISR (TIMER1_COMPA_vect)
{
    PORTB ^= 0x20;
    // Toggle PB5
}
