/*adc_main.cpp*/

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

/*Global variables*/
volatile uint8_t adc_value;
volatile bool newValue = true;

int main(void){
    /*Setup LCD*/
	//RS, RW (255 for disabled), E, 4 bit Y/N
    LiquidCrystal lcd = LiquidCrystal(PD7,255,PD5,1); 
	
	/*Setup timer1 and ADC*/
	setup_tmr();	
	setup_adc();

	sei(); //Enable interrupts

	char adc_str[6]; //Holds printed string
	
	/*Inf loop*/
	while(1){
		if (newValue){
		sprintf(adc_str, "%d", adc_value);
		lcd.clear();
		lcd.writeString("VAL :");
		lcd.setCursor(0,6);
		lcd.writeString(adc_str);
		newValue = false;
		}
    }

	return 0; 
}


ISR(ADC_vect){
	newValue = true; //There is a new reading
	adc_value = ADCH; //Read ADC value
	TIFR1 |= (1 <<OCF1B); //Clear interrupt flag manually
}

void setup_tmr(){
	/*Set timer1 */
	OCR1A = 0x10D3; // TOP for CTC mode
	TCCR1B |=  (1 << WGM12); //Mode 4: CTC on OCR1A
	TCCR1B |=  (1 << CS12) ; //Prescaler 1024 and start
}

void setup_adc(){
    /*Set up ADC on ADC5*/
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
