/*
 * spi.c
 *
 * Created: 30.09.2013 20:08:29
 *  Author: mortenmj
 */ 

#include <stdlib.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "spi.h"

#include <util/delay.h>

/* Constants for writing to SPCR. */
#define spiENABLE						( ( unsigned char ) ( 1 << SPE ) )
#define spiINT_ENABLE					( ( unsigned char ) ( 1 << SPIE ) )
#define spiMASTER_MODE					( ( unsigned char ) ( 1 << MSTR ) )

#define spiSS							( ( unsigned char ) ( 1 << PB0 ) )
#define spiSLCK							( ( unsigned char ) ( 1 << PB1 ) )
#define spiMOSI							( ( unsigned char ) ( 1 << PB2 ) )
#define spiMISO							( ( unsigned char ) ( 1 << PB3 ) )
#define spiINT							( ( unsigned char ) ( 1 << PB4 ) )
											
/*-----------------------------------------------------------*/

void mcp2515_spi_port_init (void)
{
	portENTER_CRITICAL();
	{
		/* SS, SLCK & MOSI as output */
		DDRB |= ( spiSS | spiSLCK | spiMOSI );
	
		/* MISO and PCINT4 input */
		DDRB &= ~( spiMISO );
	
		SPCR |= ( spiENABLE | spiMASTER_MODE );
	}	
	portEXIT_CRITICAL();
}

void mcp2515_spi_select (void)
{
	PORTB &= ~spiSS;
}

void mcp2515_spi_unselect (void)
{
	PORTB |= spiSS;
}

void mcp2515_spi_transfer (uint8_t dataOut, uint8_t *dataIn)
{
	SPDR = dataOut;
	while(!(SPSR & (1<<SPIF)));
	*dataIn = SPDR;
}