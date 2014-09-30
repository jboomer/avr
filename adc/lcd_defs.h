#ifndef lcd_defs_h
#define lcd_defs_h

#include <inttypes.h>
#include <avr/io.h>
#include <string.h>
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// Pin definitions -Jasper
#define DATAPORT PORTC
#define INSTRPORT PORTD
#define DIRDATA DDRC
#define DIRINSTR DDRD

int lcd_init(uint8_t rs, uint8_t rw, uint8_t enable,
		uint8_t fourbitmode);
   
void lcd_begin(uint8_t cols, uint8_t rows, uint8_t dotsize);

void lcd_clear();
void lcd_home();

void lcd_noDisplay();
void lcd_display();
void lcd_noBlink();
void lcd_blink();
void lcd_noCursor();
void lcd_cursor();
void lcd_scrollDisplayLeft();
void lcd_scrollDisplayRight();
void lcd_leftToRight();
void lcd_rightToLeft();
void lcd_autoscroll();
void lcd_noAutoscroll();

//void createChar(uint8_t, uint8_t[]);

void lcd_writeString(char*);
void lcd_setCursor(uint8_t, uint8_t); 
size_t lcd_write(uint8_t);
void lcd_command(uint8_t);


//using Print::write;
void lcd_send(uint8_t, uint8_t);
void lcd_write4bits(uint8_t);
void lcd_write8bits(uint8_t);
void lcd_pulseEnable();

uint8_t lcd_rs_pin; // LOW: command.  HIGH: character.
uint8_t lcd_rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
uint8_t lcd_enable_pin; // activated by a HIGH pulse.

uint8_t lcd_displayfunction;
uint8_t lcd_displaycontrol;
uint8_t lcd_displaymode;

uint8_t lcd_initialized;

uint8_t lcd_numlines; 
uint8_t lcd_currline;


#endif
