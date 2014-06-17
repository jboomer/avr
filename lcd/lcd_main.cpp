/*
 * lcd_main.cpp
 *
 * Created: 6/2/2013 11:32:12 AM
 *  Author: Jasper
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "lcd_defs.h"


int main(void)
{
    //RS, RW (255 for disabled), E, 4 bit Y/N
    LiquidCrystal lcd = LiquidCrystal(PD7,255,PD5,1); 

    lcd.home();
    char sayhi[]="Hi Dickhead!";
    lcd.writeString(sayhi);
    
    while(1)
    {
	//LOOP		
    }

    return 0;
}

