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






int lcd_init(uint8_t rs, uint8_t rw, uint8_t enable, //set rw=255 to disable rw pin
			     uint8_t fourbitmode)
{
  lcd_rs_pin = rs;
  lcd_rw_pin = rw;
  lcd_enable_pin = enable;
  
  DIRINSTR |= (1 << lcd_rs_pin);
  // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
  if (lcd_rw_pin != 255) { 
    DIRINSTR |= (1 << lcd_rw_pin);
  }
  DIRINSTR |= (1 << lcd_enable_pin);
  
  if (fourbitmode)
    lcd_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  else 
    lcd_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  lcd_begin(16, 2, LCD_5x8DOTS);

  return 1;
}

void lcd_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    lcd_displayfunction |= LCD_2LINE;
  }
  lcd_numlines = lines;
  lcd_currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    lcd_displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  _delay_us(50000); 
  // Now we pull both RS and R/W low to begin commands
  INSTRPORT &= ~((1 << lcd_rs_pin) | (1 << lcd_enable_pin));
  if (lcd_rw_pin != 255) { 
    INSTRPORT &=~ (1 << lcd_rw_pin);
  }
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (lcd_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    lcd_write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms

    // second try
    lcd_write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms
    
    // third go!
    lcd_write4bits(0x03); 
    _delay_us(150);

    // finally, set to 4-bit interface
    lcd_write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
    _delay_us(4500);  // wait more than 4.1ms

    // second try
    lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
    _delay_us(150);

    // third go
    lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);
  }

  // finally, set # lines, font size, etc.
  lcd_command(LCD_FUNCTIONSET | lcd_displayfunction);  

  // turn the display on with no cursor or blinking default
  lcd_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  lcd_display();

  // clear it off
  lcd_clear();

  // Initialize to default text direction (for romance languages)
  lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  lcd_command(LCD_ENTRYMODESET | lcd_displaymode);

}

/********** high level commands, for the user! */
void lcd_clear()
{
  lcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void lcd_home()
{
  lcd_command(LCD_RETURNHOME);  // set cursor position to zero
  _delay_us(2000);  // this command takes a long time!
}

void lcd_setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row >= lcd_numlines ) {
    row = lcd_numlines-1;    // we count rows starting w/0
  }
  
  lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcd_noDisplay() {
  lcd_displaycontrol &= ~LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}
void lcd_display() {
  lcd_displaycontrol |= LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

// Turns the underline cursor on/off
void lcd_noCursor() {
  lcd_displaycontrol &= ~LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}
void lcd_cursor() {
  lcd_displaycontrol |= LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_noBlink() {
  lcd_displaycontrol &= ~LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}
void lcd_blink() {
  lcd_displaycontrol |= LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_scrollDisplayRight(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(void) {
  lcd_displaymode |= LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(void) {
  lcd_displaymode &= ~LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(void) {
  lcd_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(void) {
  lcd_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
/*void lcd_createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}*/

void lcd_writeString(char *aString){
	while (*aString > 0){
		lcd_write(*aString++);
	}
}

/*********** mid level commands, for sending data/cmds */

inline void lcd_command(uint8_t value) {
  lcd_send(value, 0);
}

inline size_t lcd_write(uint8_t value) {
  lcd_send(value, 1);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void lcd_send(uint8_t value, uint8_t mode) {
  if (mode){
    INSTRPORT |= (1<<lcd_rs_pin);
  } else {
    INSTRPORT &= ~(1<<lcd_rs_pin);
  }

    // if there is a RW pin indicated, set it low to Write
  if (lcd_rw_pin != 255) { 
    INSTRPORT &= ~(lcd_rw_pin << 1);
  }
  
  if (lcd_displayfunction & LCD_8BITMODE) {
    lcd_write8bits(value); 
  } else {
    lcd_write4bits(value>>4);
    lcd_write4bits(value);
  }
}

void lcd_pulseEnable(void) {
  INSTRPORT &= ~(1 << lcd_enable_pin);
  _delay_us(1);    
  INSTRPORT |= (1 << lcd_enable_pin);
  _delay_us(1);    // enable pulse must be >450ns
  INSTRPORT &= ~(1 << lcd_enable_pin);
  _delay_us(100);   // commands need > 37us to settle
  
  /*
  INSTRPORT|= (1 << lcd_rw_pin); // set RW to high to read
  INSTRPORT &= ~(1 << lcd_rs_pin); // set to command mode
  DIRDATA = 0x00; // set dataport to read mode
  uint8_t BF = 0;
  while (BF >= 0x08)
  {
  INSTRPORT |= ( 1<< lcd_enable_pin);
  _delay_us(1);    // read busy flag
  INSTRPORT &= ~(1 << lcd_enable_pin);
  BF = DATAPORT&0x08;
  INSTRPORT |= (1 << lcd_enable_pin);
  _delay_us(1);    // read busy flag
  INSTRPORT &= ~(1 << lcd_enable_pin);
  }
  DIRDATA = 0x0f; // set dataport back to output mode*/
}

void lcd_write4bits(uint8_t value) {
  DIRDATA = 0x0f;
  DATAPORT = 0x00 | value;

  lcd_pulseEnable();
}

void lcd_write8bits(uint8_t value) {
  DIRDATA = 0xff;
  DATAPORT = value;
  
  lcd_pulseEnable();
}


#endif
