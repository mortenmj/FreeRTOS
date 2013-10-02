/*
 * can.c
 *
 * Created: 01.10.2013 20:11:09
 *  Author: mortenmj
 */

#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "can_ll.h"
#include "can.h"

void vCanInit ( void )
{
	vCanLLInit ();
}