#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h> 
//#include <stdlib.h>
//#include <avr/interrupt.h> 

//define some postions for the servo
#define servo_left 900
#define servo_right 1900
#define servo_center 1450

//define danger zones
#define forward_buffer 18	//how close the robot will get to something in front of it (cm)
#define sides_buffer 13		//how close the robot will allow things on the sides (cm)
#define now_clear 6		//additional buffer distance to determine if now clear after getting too close

//define motor speed offset
#define l_ofst 0
#define r_ofst 4

//define some speeds to run the motor pwm at 
#define speed_00p 0	// 0   percent duty cycle
#define speed_50p 127	// 50  percent duty cycle
#define speed_70p 180	// 70  percent duty cycle
#define speed_80p 205	// 80  percent duty cycle
#define speed_90p 230   // 90  percent duty cycle
#define speed_100p 255	// 100 percent duty cycle

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOOGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

//Macro for setting servo position.  
#define servo_position(srv_position, delayms) { OCR1A = srv_position; _delay_ms(delayms); } 

//Macros for controlling the motors. 
#define drive_forward(speed) { OCR0A = (speed + r_ofst); OCR0B = (speed + l_ofst); CLEAR_BIT(PORTD, 7); CLEAR_BIT(PORTB, 0);}
#define drive_reverse(speed) { OCR0A = (255-speed); OCR0B = (255-speed); SET_BIT(PORTD, 7); SET_BIT(PORTB, 0);}
#define drive_stop(speed) { OCR0A = 0; OCR0B = 0; CLEAR_BIT(PORTD, 7); CLEAR_BIT(PORTB, 0);}
#define drive_right(speed) { OCR0A = speed; OCR0B = 255-speed; SET_BIT(PORTD, 7); CLEAR_BIT(PORTB, 0);}
#define drive_left(speed) { OCR0A = 255-speed; OCR0B = speed; CLEAR_BIT(PORTD, 7); SET_BIT(PORTB, 0);}

static void beep_meow(void)
{
	uint8_t i;
	for(i = 0; i<150; i++)
	{
		TOOGLE_BIT(PORTD,2);
		_delay_us(700);
	}
	CLEAR_BIT(PORTD,2);
}

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


int16_t sensor_distance(void)
{
		int16_t dist;
		int16_t cm_dist;
		//send the SRF04 a 10us pulse to begin measurement
		PORTD |= 0x01;		//begin the a 10us pulse
		_delay_us(10);		//wait 10us
		PORTD &= 0xFE;		//end the 10us pulse
		
		//wait for the SRF04 to send back data pulse
		while ( !( PIND & (1 << PD1)) ) ; //loop until PORTD Bit D1 is set
		dist = 0;
		
		//wait for end of pulse
		while ( PIND & ( 1<< PD1 ) ) //loop until PORTD Bit D1 is clear
		{
			dist ++; 
			_delay_us(1);
		}
		cm_dist = dist / 58;  // uS / 58 = cm  or  uS / 128 = inches
		return cm_dist;		
}
 

int main(void) {
	//setting PWM-Ports as output
	DDRB = 0xFF;		//set port B as output
	DDRD = 0b11111101;	//set port D as output with pin D1 input


	//setup the pwm for the servo

	TCCR1A =  _BV(COM1A1) // set OC1A/B at TOP
		| _BV(COM1B1) // clear OC1A/B when match
		| _BV(WGM11); //(fast PWM, clear TCNT1 on match ICR1)

	TCCR1B =  _BV(WGM13)
		| _BV(WGM12)
		| _BV(CS11); // timer uses main system clock with 1/8 prescale
	
	ICR1 = 20000; // used for TOP, makes for 50 hz PWM
	OCR1A = 1500; // servo at center

	//set up pwm for the motors
	TCCR0A =  _BV(COM0A1) // set OC1A/B at TOP
		| _BV(COM0B1) // clear OC1A/B when match
		| _BV(WGM00)  // fast PWM mode
		| _BV(WGM01); 
	
	TCCR0B =  _BV(CS01)
		| _BV(CS00);

	drive_stop(0);				//motors stopped
	servo_position(servo_center, 50);	//servo center

	//variables to hold the distances;
	uint16_t forward_dist;	
	uint16_t left_dist;
	uint16_t right_dist;
	uint8_t fwd_count;

	servo_position(servo_center, 50);



	while(1)
	{
		forward_dist = sensor_distance();
		_delay_ms(50);
		if (forward_dist <= forward_buffer)
		{
			drive_stop(0);
			beep_meow();
			servo_position(servo_left, 400);
			left_dist = sensor_distance();
			servo_position(servo_right,400);
			right_dist = sensor_distance();
			servo_position(servo_center,200);
			while ((left_dist > right_dist) && (forward_dist <= (forward_buffer + now_clear)))
			{
				drive_left(speed_80p);
				forward_dist = sensor_distance();
				_delay_ms(50);
			}
			while ((left_dist <= right_dist) && (forward_dist <= (forward_buffer + now_clear)))
			{
				drive_right(speed_80p);
				forward_dist = sensor_distance();
				_delay_ms(50);
			}
		}
		drive_forward(speed_80p);
		fwd_count++;
		if (fwd_count == 20)
		{
			fwd_count = 0;
			servo_position(servo_left, 150);
			left_dist = sensor_distance();
			if (left_dist <= sides_buffer)
			{	
				drive_stop(0);
				beep_meow();
				while (left_dist <= (sides_buffer + now_clear))
				{
					drive_right(speed_80p);
					left_dist = sensor_distance();
					_delay_ms(100);
				}
			}
			servo_position(servo_center,100);
			servo_position(servo_right, 150);
			right_dist = sensor_distance();
			if (right_dist <= sides_buffer)
			{
				drive_stop(0);
				beep_meow();
				while (right_dist <= (sides_buffer + now_clear))
				{
					drive_left(speed_80p);
					right_dist = sensor_distance();
					_delay_ms(100);
				}
			}
			servo_position(servo_center,100);
		}
				
	}

	return 0;
}
