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
#include "semphr.h"
#include "task.h"

#include "serial.h"
#include "adc.h"

/* Constants for writing to ADMUX */
#define adc_AVCC						( ( unsigned char ) (1 << REFS0) )
#define adc_ADLAR						( ( unsigned char ) (1 << ADLAR) )

/* Constants for writing to ADCSRA */
#define adc_125KHZ						( ( unsigned char ) (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) )
#define adc_ENABLE						( ( unsigned char ) (1 << ADEN) )
#define adc_INT_ENABLE					( ( unsigned char ) (1 << ADIE) )
#define adc_START_CONVERSION			( ( unsigned char ) (1 << ADSC) )

static xQueueHandle xAdcValues;
xSemaphoreHandle xAdcSemaphore;

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

void vAdcInit ( unsigned portBASE_TYPE uxQueueLength )
{
	
	portENTER_CRITICAL();
	{
		xAdcValues = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
		vSemaphoreCreateBinary ( xAdcSemaphore );
		
		/* Take the semaphore so that the queue isn't accessed before it is populated */
		xSemaphoreTake ( xAdcSemaphore, 0 );
		
		/* Set prescaler at 128 for 125kHz operation */
		ADCSRA |= adc_125KHZ;
		
		/* Set reference voltage to AVCC and left shift result */
		ADMUX = ( adc_AVCC | adc_ADLAR );
	
		/* Enable ADC */
		ADCSRA |= adc_ENABLE;
			
		vInterruptOn();
		vAdcStartConversion ();
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

portBASE_TYPE xAdcGetPort ( void )
{
	return ADMUX & 0x1F;
}

void vAdcSetPort ( portBASE_TYPE port )
{
	unsigned char ucByte;
	
	/* Clear bottom 5 bits of ADMUX, then set them from port */
	ucByte = ADMUX;
	ucByte &= 0xE0;
	ucByte |= port;
	ADMUX = ucByte;
}

void vAdcStartConversion ( void )
{
	ADCSRA |= adc_START_CONVERSION;
}

portBASE_TYPE xAdcTakeSemaphore ( void )
{
	return xSemaphoreTake ( xAdcSemaphore, 0 );
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
signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
const portBASE_TYPE port = xAdcGetPort ();
	
	/* Get the value and post it on the queue of ADC values.
	If the post causes a task to wake force a context switch as the woken task may have a higher priority than the task we have interrupted. */
	cVal = ADCH;
	xQueueSendFromISR( xAdcValues, &cVal, &xHigherPriorityTaskWoken );
	
	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		taskYIELD ();
	}
	
	/* If the current result is from port 0, the next should be from port 1 and vice versa */
	if (port == adcPORT0) 
	{
		vAdcSetPort ( adcPORT1 );
		
		/* After one conversion, we immediately start the second. */
		vAdcStartConversion ();
	} else
	{
		vAdcSetPort ( adcPORT0 );
		
		/* After the second conversion, we give the semaphore */
		xSemaphoreGiveFromISR ( xAdcSemaphore, &xHigherPriorityTaskWoken );
	}
	
	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		taskYIELD ();
	}
}