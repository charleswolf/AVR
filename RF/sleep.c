#include "sleep.h"

volatile uint16_t SLEEP_COUNTER;	//loop counter for sleeping
volatile uint8_t SLEEP_FLAG;		//flag to indicate when sleeping

void sleep_timer_init()
{
	// set up timer 2 in normal mode with interrupt on overflow
	TCCR2A = 0x00; 
	TCCR2B  |= _BV(CS22) | _BV(CS21) | _BV(CS20);	// prescaler of 1024
	TIMSK2 	|= _BV(TOIE2); 	//enable timer 2 overflow interrupt

	//set timer value to 247
	TCNT2 = 247;
	SLEEP_FLAG = 1; 	//sleep interrupt is now active
}

void sleep_timer_destroy()
{
	//disable timer 2 interrupts
	TIMSK2 = 0;
	sleep_disable();	//return from sleeping
	SLEEP_FLAG = 0;
}

extern void sleep_int_handler()
{
	TCNT2 = 247;

	//decrement this counter until zero is reached
	if (SLEEP_COUNTER != 0)	
	{
		SLEEP_COUNTER --;
	}

	//test if zero is reached		
	if (SLEEP_COUNTER < 1)
	{
		sleep_timer_destroy();	
	}
}

extern void sleep_ms ( uint16_t loops)		//function for sleeping multiples of 1ms
{
	SLEEP_COUNTER = loops; //set the number of ms for sleeping
	sleep_timer_init();	//initialize timer 2 for sleeping
	sei();			//enable interrupts
	sleep_enable();		//enable sleep	
	sleep_bod_disable();	//disable brown out detect
	sleep_cpu();		//go to sleep now
}
