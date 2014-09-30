/*adc_main.cpp

Reads analog input and prints it on the lcd screen
*/


#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
#include "lcd_defs.h"

/*Functions*/
void setup_adc();
void setup_tmr();
void setup_pwm();

/*Global variables*/
volatile uint8_t adc_value;
volatile int newValue = 1;

int main(void){
	//RS, RW (255 for disabled), E, 4 bit Y/N
    	lcd_init(PD7,255,PD5,1); 
	
	setup_tmr();	
	setup_adc();
	setup_pwm();

	sei(); //Enable interrupts

	char adc_str[6];
	

	while(1){
		if (newValue){
		sprintf(adc_str, "%d", adc_value);
		lcd_clear();
		lcd_writeString("VAL :");
		lcd_setCursor(0,6);
		lcd_writeString(adc_str);
		newValue = 0;
		}
    }
	return 0; 
}


ISR(ADC_vect){
	newValue = 1; //There is a new reading
	adc_value = ADCH; //Read ADC value
	OCR0A = adc_value; //Change duty cycle
	TIFR1 |= (1 <<OCF1B); //Clear interrupt flag manually (Waarom?)
}

void setup_tmr(){
	OCR1A = 0x10D3; // TOP for CTC mode
	TCCR1B |=  (1 << WGM12); //Mode 4: CTC on OCR1A
	TCCR1B |=  (1 << CS12) ; //Prescaler 1024 and start
}

void setup_adc(){
	ADMUX |= (1 << REFS0); //Use AVcc as reference voltage
	ADMUX |= (1 << ADLAR); //Use only 8bit resolution
	ADMUX |= (1 << MUX0) | (1 << MUX2); //Select ADC5
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Clock div
	ADCSRA |= (1 << ADATE); //Enable auto-triggering
	ADCSRB |= (1 << ADTS2) | (1 << ADTS0); //Set to timer1 Compare match B
	OCR1B = 0x00ff; //Compare match
	ADCSRA |= (1 << ADIE); //Enable interrupt vector 
	ADCSRA |= (1 << ADEN); //Enable the ADC!
}

void setup_pwm(){
	DDRD |= (1 << PD6); //PD6 = OC0A
	OCR0A = 0;
	TCCR0A |= (1 << COM0A1); //Non-inverting mode
	TCCR0A |= (1 << WGM01) | (1 << WGM00); //Fast PWM
	TCCR0B |= (1 << CS01); //Prescaler = 8
}

