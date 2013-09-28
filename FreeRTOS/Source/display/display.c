/*
 * display.c
 *
 * Created: 28.09.2013 00:50:58
 *  Author: mortenmj
 */ 

#include "FreeRTOS.h"

#include <avr/io.h>

#include "drivers/oled.h"

void vDisplayInitialize (void)
{
	/* Initialize external memory interface */
	XMCRA |= (1 << SRE);
	
	/* Release PC7-PC2 */
	XMCRB |= (1 << XMM2) | (1 << XMM1);
	
	vOledInitialize ();
}

void vDisplayTest (char *s)
{
	vOledSetLine ( 0 );
	vOledPrint ( &s );
}