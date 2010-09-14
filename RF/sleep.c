#include "sleep.h"

extern void sleep_timer_init()
{
	// set up timer 2 in normal mode with interrupt on overflow
	TCCR2A = 0x00; 
	TCCR2B  |= 	_BV(CS22)
		|	_BV(CS21)
		|	_BV(CS20);	// prescaler of 1024
	TIMSK2 	|=	_BV(TOIE2); 	//enable timer 2 overflow interrupt
			
	TCNT2 = 247;			//set timer value to 247
}

extern void sleep_1_ms( uint8_t loops)		//function for sleeping multiples of 1ms
{
		uint8_t sleep_count;
		for (sleep_count = 0; sleep_count < loops; sleep_count++)
		{
			TCNT2 = 247;			//set timer value to 247
			sleep_enable();			//enable sleep mode bit
			sleep_cpu();			//go to selected sleep mode
			TIMSK2 = 0;			//disable timer 2 interrupts
		}
}
