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
#include "serial.h"

#include <util/delay.h>

/* Constants for writing to SPCR. */
#define spiENABLE						( ( unsigned char ) ( 1 << SPE ) )
#define spiINT_ENABLE					( ( unsigned char ) ( 1 << SPIE ) )
#define spiMASTER_MODE					( ( unsigned char ) ( 1 << MSTR ) )

/* SPI pins */
#define spiSLCK							( ( unsigned char ) ( 1 << PB1 ) )
#define spiMOSI							( ( unsigned char ) ( 1 << PB2 ) )
#define spiMISO							( ( unsigned char ) ( 1 << PB3 ) )
#define spiINT							( ( unsigned char ) ( 1 << PB4 ) )
											
/*-----------------------------------------------------------*/

void spi_init (void)
{
	portENTER_CRITICAL();
	{
		/* SS, SLCK & MOSI as output */
		DDRB |= ( spiSLCK | spiMOSI );
	
		/* MISO and PCINT4 input */
		DDRB &= ~( spiMISO );
	
		SPCR |= ( spiENABLE | spiMASTER_MODE );
	}	
	portEXIT_CRITICAL();
}

uint8_t spi_transfer (uint8_t data_out)
{
	SPDR = data_out;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void spi_write_block (uint8_t *data_out, uint8_t len)
{
	for (int i = 0; i < len; i++)
	{
		spi_transfer ( data_out[i] );
	}
}

void spi_readwrite_block (uint8_t *data_out, uint8_t *data_in, uint8_t len)
{
	for (int i = 0; i < len; i++)
	{
		data_in[i] = spi_transfer ( data_out[i] );
	}
}