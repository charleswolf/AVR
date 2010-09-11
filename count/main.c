//#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

//count displaying on multiple 7 segments with only 1 driver chip


int main(void) {
	DDRD = 0xff;		//set port D as output
	uint16_t number = 0;	//number used to store the count
//	uint8_t display;	//temporary holder
	char number_string[2];	//string for holding BCD values of number
	char temp[2];		//temporary string for adjusting number_string
	uint8_t i;		
	uint16_t j;
	while(1) {
		//number = 456;
		utoa(number, number_string, 10 );//MSB => number_String[0]
		if (number < 10)
		{
			*temp = *number_string;
			number_string[0] = 0;
			number_string[1] = 0;
			number_string[2] = temp[0];	
		}
		else if ((number >9) && (number < 100))
		{
			temp[0] = number_string[0];
			temp[1] = number_string[1];
			number_string[0] = 0;
			number_string[1] = temp[0];
			number_string[2] = temp[1];
		}

		for(j=0;j<20000;j++)
		{
			for(i=0;i<3;i++)
			{		
				PORTD &= 1 << (5+i);
				PORTD = (number_string[i]) & 0x0F;
				PORTD |= 1 << (5+i);
			}
		}
		PORTD &= 0x00;
		number ++;
		if (number>999)
		{
			number = 0;
		}
    	}
	return 0;
}
