#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../spi.c"
#include "../nRF24L01.c"

uint8_t nRF24L01_data[32];
uint8_t *buffer =  &nRF24L01_data[0];
uint8_t buffersize = sizeof(nRF24L01_data);

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

#define __AVR_ATmega168__

ISR( PCINT2_vect ) 
{
    uint8_t status;   
    // If still in transmitting mode then finish transmission
    PORTD ^= 1<<0; //toggle bit
    if (PTX) {
    
        // Read nRF24L01 status 
        nRF24L01_CSN_lo;			// Pull down chip select
        status = spi_transmit_byte(NOP);	// Read status register
        nRF24L01_CSN_hi;			// Pull up chip select

    	nRF24L01_CSN_lo;			// Pull down chip select
    	spi_transmit_byte( FLUSH_TX );		// Write cmd to flush tx fifo
   	nRF24L01_CSN_hi;			// Pull up chip select

        nRF24L01_CE_lo;				// Deactivate transreceiver
        RX_POWERUP;                             // Power up in receiving mode
        nRF24L01_CE_hi;				// Listening for pakets
        PTX = 0;				// Set to receiving mode

        // Reset status register for further interaction
        nRF24L01_config_register(STATUS,(1<<TX_DS)|(1<<MAX_RT)); // Reset status register
    }
}


int main(void){
	// Initialize AVR for use with nRF24L01
	nRF24L01_init();
	// Wait for nRF24L01 to come up
	_delay_ms(50);
	// Activate interrupts
	sei();
	// Configure nRF24L01
	nRF24L01_config();

	//set up PORTD for indicating LED lights
	SET_BIT(DDRD, 0);
	SET_BIT(DDRD, 1);
	SET_BIT(PORTD, 1);

	//wait 300ms before starting 
	_delay_ms(3000);

	while(1)	//infinite loop
	{
		while (!nRF24L01_data_ready());		//wait for packet from nRF24L01+
		_delay_us(100);				//wait 100us before reading the data
		nRF24L01_get_data(buffer);			//get the data from the nRF24L01+ and store it in the buffer


		// turn on an appropriate led based uppon the message recieved
		if (nRF24L01_data[0] == 0x01)		//LED 1
		{
			SET_BIT(PORTD, 0);		//turn on led 1
			CLEAR_BIT(PORTD,1);		//turn off led 2
		}
		else if (nRF24L01_data[0] == 0x02)	//LED2
		{
			SET_BIT(PORTD, 1);		//turn on led 2
			CLEAR_BIT(PORTD,0);		//turn off led 1
		}
	}
}
//test




















