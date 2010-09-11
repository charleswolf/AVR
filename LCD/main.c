#define F_CPU 8000000UL
//#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <string.h>
#include "lcd.c"

//Macros for setting, clearing and toggleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))    //or
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM))) //and
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM))) //exclusive or

//#define READ_BIT(

int8_t response_started;
int8_t response_ended;
int8_t bit_count;
int8_t byte_count;
int8_t parity;
char data_byte;
int8_t pin_C0;
int8_t clock_edge_rising; 		//1 => rising, 0 => falling
char ascii_data[32];



int main(void)
{
	DDRB = 0xFF;		//set port B as output

	int8_t start_field = 0;
	int8_t end_field = 0;
	uint16_t ms_count;

	/* initialize display, cursor off */
	lcd_init(LCD_DISP_ON);
	
	for(;;)
	{	
		lcd_clrscr();
		lcd_puts("line1\nline2");
		_delay_ms(1000);
	}
}
