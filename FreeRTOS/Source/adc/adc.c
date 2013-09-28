/*
 * adc.c
 *
 * Created: 08.09.2013 13:30:53
 * Author: mortenmj
 */ 

#include <stdlib.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "adc.h"

/* Constants for writing to ADMUX */
#define adc_AVCC						( ( unsigned char ) 0x40 )
#define adc_ADLAR						( ( unsigned char ) 0x20 )

/* Constants for writing to ADCSRA */
#define adc_125KHZ						( ( unsigned char ) 0x07 )
#define adc_AUTO_TRIGGER				( ( unsigned char ) 0x20 )
#define adc_ENABLE						( ( unsigned char ) 0x80 )
#define adc_INT_ENABLE					( ( unsigned char ) 0x08 )
#define adc_START_CONVERSION			( ( unsigned char ) 0x40 )

static xQueueHandle xAdcValues; 

#define vInterruptOn()												\
{																	\
	unsigned char ucByte;											\
	\
	ucByte = ADCSRA;												\
	ucByte |= adc_INT_ENABLE;										\
	ADCSRA = ucByte;												\
}
/*-----------------------------------------------------------*/

#define vInterruptOff()												\
{																	\
	unsigned char ucInByte;											\
	\
	ucInByte = ADCSRA;												\
	ucInByte &= ~adc_INT_ENABLE;									\
	ADCSRA = ucInByte;												\
}
/*-----------------------------------------------------------*/

void xAdcInit ( unsigned portBASE_TYPE uxQueueLength )
{
	
	portENTER_CRITICAL();
	{
		xAdcValues = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
		
		/* Set reference voltage to AVCC and left shift result */
		ADMUX = ( adc_AVCC | adc_ADLAR );
	
		/* Set prescaler to 125khz, auto triggered */
		ADCSRA = ( adc_125KHZ | adc_AUTO_TRIGGER | adc_ENABLE | adc_START_CONVERSION );
		
		vInterruptOn();
	}
	portEXIT_CRITICAL();
}

signed portBASE_TYPE xAdcGetValue ( signed char *pcAdcVal, portTickType xBlockTime )
{
	if ( xQueueReceive ( xAdcValues, pcAdcVal, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}
	
	return pdPASS;
}

void vAdcClose ( void )
{
unsigned char ucByte;
	
	portENTER_CRITICAL();
	{
		vInterruptOff();
		ucByte = ADCSRA;
		ucByte &= ~adc_ENABLE;
		ADCSRA = ucByte;
	}
	portEXIT_CRITICAL();		
}

ISR (ADC_vect)
{
portCHAR cVal;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Grab the ADC queue semaphore */

	/* Get the value and post it on the queue of ADC values.
	If the post causes a task to wake force a context switch as the woken task may have a higher priority than the task we have interrupted. */
	cVal = ADCH;
	
	if ( xQueueSendFromISR( xAdcValues, &cVal, &xHigherPriorityTaskWoken ) != pdPASS )
	{
		// queue is full. release semaphore
	}
	
	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		taskYIELD ();
	}
}