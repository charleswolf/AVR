# define F_CPU 8000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>



int main(void){

   //setting PWM-Ports as output
    DDRB = _BV (PB1);

   //
   TCCR1A  = _BV(WGM10) | _BV(WGM11) | _BV(COM1A1);

   // Timers running at MCU/1024
   TCCR1B = _BV(CS10);

   // set PWM Out
   // PWM duty = motor / 255
   OCR1A = 10; // out at OC1A

   while(1){
	unsigned int diff = 0;
	OCR1A = ((rand() / (RAND_MAX / 125 + 1) )+75 );
	diff = 255-OCR1A;
	//_delay_ms(rand() / (RAND_MAX / 100 + 1)+100);

	unsigned int c = 0;
	unsigned int udelay = 1000/diff;
	for(c=0;c<diff;c++)
	{
		OCR1A = OCR1A + 1;
		_delay_us(udelay);
	}
	_delay_ms(rand() / (RAND_MAX / 100 + 1)+35);
   }

   return 0;
}

