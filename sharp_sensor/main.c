# define F_CPU 8000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdlib.h>


uint16_t ReadChannel(uint8_t mux)
{
  uint8_t i;
  uint16_t result = 0;          // Initialization importantly, since local variables
                                // not are initialized automatically and
                          // coincidental values to have. Otherwise knows rubbish get a
  ADCSRA = ((1<<ADEN) | (1<<ADPS1) | (1<<ADPS0));     // frequency prescaler
                                // set to 8 (1) and ADC activate (1)
  ADMUX = mux;                       // channel select
  ADMUX |= (1<<REFS1) | (1<<REFS0); // internal reference tension use
  /* after activating the ADC a “dummy Readout” are recommended, one read thus
  a value and reject this, around the ADC “warm up to let”*/
  ADCSRA |= (1<<ADSC);               // a ADC transformation
  while ( ADCSRA & (1<<ADSC) ) {
      ;     // on conclusion of the conversion wait
  }
  /* actual measurement - average value from 4 following each other transformations */
  for(i=0;i<4;i++)
  {
     ADCSRA |= (1<<ADSC);            // a transformation “single conversion”
     while ( ADCSRA & (1<<ADSC) ) {
       ;   // on conclusion of the conversion wait
     }
     result += ADCW;                // transformation results add
  }
  ADCSRA &= ~(1<<ADEN);              // ADC deactivate (2)
  result /= 4;                      // sum by four divide = arithm. Average value
  return result;
}




int main(void){

   //setting PWM-Ports as output
    DDRB = _BV (PB1);

   //
   TCCR1A  = _BV(WGM10) | _BV(WGM11) | _BV(COM1A1);

   // Timers running at MCU/1024, roughly 15 kHz (using ATMEGA128)
   TCCR1B = _BV(CS10);

   // set PWM Out
   // PWM duty = motor / 255
   OCR1A = 10; // out at OC1A

  uint16_t adcval; 

   while(1){
	adcval = ReadChannel(0); /* MUX bits on 0b0000 - > Channel 0 */
	OCR1A = adcval;
   }
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
	adcval = ReadChannel(0); /* MUX bits on 0b0000 - > Channel 0 */
   }

   return 0;
}

