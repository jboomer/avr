#include <avr/io.h>
#include <util/delay.h>
//#define F_CPU 16000000L
//#define (__AVR_ATmega328__)

int main(void)
{
  DDRB|=(0x20);
   
   while(1)
   {
      PORTB ^=0x20;
      _delay_ms(5000);
   }

   return 0;
}
