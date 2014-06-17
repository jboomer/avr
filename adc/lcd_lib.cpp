#ifndef lcd_lib_h
#define lcd_lib_h
#include "lcd_defs.h"

#define F_CPU 16000000L
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/delay.h>


// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).






LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable, //set rw=255 to disable rw pin
			     uint8_t fourbitmode)
{
  _rs_pin = rs;
  _rw_pin = rw;
  _enable_pin = enable;
  
  DIRINSTR |=1<<_rs_pin;
  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (_rw_pin != 255) { 
    DIRINSTR |= 1<<_rw_pin;
  }
  DIRINSTR |= 1<< _enable_pin;
  
  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else 
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  begin(16, 2);  
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  _delay_us(50000); 
  // Now we pull both RS and R/W low to begin commands
  INSTRPORT &= ~((1<<_rs_pin)|(1<<_enable_pin));
  if (_rw_pin != 255) { 
    INSTRPORT &=~(1<<_rw_pin);
  }
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms
    
    // third go!
    write4bits(0x03); 
    _delay_us(150);

    // finally, set to 4-bit interface
    write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    _delay_us(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    _delay_us(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }

  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void LiquidCrystal::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void LiquidCrystal::setCursor(uint8_t row, uint8_t col)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row >= _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
/*void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}*/

void LiquidCrystal::writeString(char *aString){
	while (*aString>0){
		write(*aString++);
	}
}

/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal::command(uint8_t value) {
  send(value, 0);
}

inline size_t LiquidCrystal::write(uint8_t value) {
  send(value, 1);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LiquidCrystal::send(uint8_t value, uint8_t mode) {
  if (mode){
    INSTRPORT |= (1<<_rs_pin);
  } else {
    INSTRPORT &= ~(1<<_rs_pin);
  }

    // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) { 
    INSTRPORT &= ~(_rw_pin<<1);
  }
  
  if (_displayfunction & LCD_8BITMODE) {
    write8bits(value); 
  } else {
    write4bits(value>>4);
    write4bits(value);
  }
}

void LiquidCrystal::pulseEnable(void) {
  INSTRPORT &= ~(1<<_enable_pin);
  _delay_us(1);    
  INSTRPORT |= (1<<_enable_pin);
  _delay_us(1);    // enable pulse must be >450ns
  INSTRPORT &= ~(1<<_enable_pin);
  _delay_us(100);   // commands need > 37us to settle
  
  /*
  INSTRPORT|=(1<<_rw_pin); // set RW to high to read
  INSTRPORT &=~(1<<_rs_pin); // set to command mode
  DIRDATA = 0x00; // set dataport to read mode
  uint8_t BF=0;
  while (BF>=0x08)
  {
  INSTRPORT |= (1<<_enable_pin);
  _delay_us(1);    // read busy flag
  INSTRPORT &= ~(1<<_enable_pin);
  BF=DATAPORT&0x08;
  INSTRPORT |= (1<<_enable_pin);
  _delay_us(1);    // read busy flag
  INSTRPORT &= ~(1<<_enable_pin);
  }
  DIRDATA = 0x0f; // set dataport back to output mode*/
}

void LiquidCrystal::write4bits(uint8_t value) {
  DIRDATA = 0x0f;
  DATAPORT = 0x00 | value;

  pulseEnable();
}

void LiquidCrystal::write8bits(uint8_t value) {
  DIRDATA = 0xff;
  DATAPORT = value;
  
  pulseEnable();
}


#endif
