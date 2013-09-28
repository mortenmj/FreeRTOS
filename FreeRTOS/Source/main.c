/*
    FreeRTOS V7.5.2 - Copyright (C) 2013 Real Time Engineers Ltd.

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main. c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.  
 * Each task that does not flash an LED maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles an LED.  Should any task contain an error at any time the LED toggle
 * will stop.
 *
 * The LED flash and communications test tasks do not maintain a count.
 */

/*
Changes from V1.2.0
	
	+ Changed the baud rate for the serial test from 19200 to 57600.

Changes from V1.2.3

	+ The integer and comtest tasks are now used when the cooperative scheduler 
	  is being used.  Previously they were only used with the preemptive
	  scheduler.

Changes from V1.2.5

	+ Set the baud rate to 38400.  This has a smaller error percentage with an
	  8MHz clock (according to the manual).

Changes from V2.0.0

	+ Delay periods are now specified using variables and constants of
	  portTickType rather than unsigned long.

Changes from V2.6.1

	+ The IAR and WinAVR AVR ports are now maintained separately.

Changes from V4.0.5

	+ Modified to demonstrate the use of co-routines.

*/

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>

#ifdef GCC_MEGA_AVR
	/* EEPROM routines used only with the WinAVR compiler. */
	#include <avr/eeprom.h> 
#endif

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

/* Includes for our tasks */
#include "serial.h"
#include "adc.h"
#include "display/display.h"

#define F_CPU 16000000UL
#include <util/delay.h>

#include <avr/interrupt.h>

/* Definitions for our LEDs and switches */
#define LED0 PD0
#define LED1 PD2
#define LED2 PD4
#define LED3 PD6

#define SW0 PD1
#define SW1 PD3
#define SW2 PD5
#define SW3 PD7

/* Priority definitions for our tasks. */
#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainJOYSTICK_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* Baud rate used by the serial port tasks. */
#define mainCOM_BAUD_RATE				( ( unsigned long ) 19200 )

/* LED used by the serial port tasks.  This is toggled on each character Tx,
and mainCOM_TEST_LED + 1 is toggles on each character Rx. */
#define mainCOM_TEST_LED				( 4 )

/* Number of ADC values to queue */
#define mainNUM_ADC_VALUES				( 4 )

/* LED that is toggled by the check task.  The check task periodically checks
that all the other tasks are operating without error.  If no errors are found
the LED is toggled.  If an error is found at any time the LED is never toggles
again. */
#define mainCHECK_TASK_LED				( 7 )

/* The period between executions of the check task. */
#define mainCHECK_PERIOD				( ( portTickType ) 3000 / portTICK_RATE_MS  )

/* An address in the EEPROM used to count resets.  This is used to check that
the demo application is not unexpectedly resetting. */
#define mainRESET_COUNT_ADDRESS			( ( void * ) 0x50 )

/* The number of coroutines to create. */
#define mainNUM_FLASH_COROUTINES		( 3 )

void vSendString ( char *s );

/*
 * The task function for the "Check" task.
 */
static void vErrorChecks ( void *pvParameters );

/*
 * Checks the unique counts of other tasks to ensure they are still operational.
 * Flashes an LED if everything is okay. 
 */
static void prvCheckOtherTasksAreStillRunning ( void );

/*
 * Called on boot to increment a count stored in the EEPROM.  This is used to 
 * ensure the CPU does not reset unexpectedly.
 */
static void vTest1 ( void *pvParameters );
static void vTest2 ( void *pvParameters );

/*
 * The idle hook is used to scheduler co-routines.
 */
void vApplicationIdleHook ( void );

/*
* The stack overflow hook is used to inspect stack overflow errors
*/
void vApplicationStackOverflowHook ( xTaskHandle xTask, signed portCHAR *pcTaskName );

/*
* The tick hook is called on every tick
*/
void vApplicationTickHook ( void );

/*-----------------------------------------------------------*/

int main( void )
{	
	DDRB = 0xFF;
	PORTB = 0xFF;
	
	xSerialPortInitMinimal ( 4800, 10 );
	vSendString ( "init\n" );
	vDisplayInitialize ();
	
	/* Test test test */
	xTaskCreate ( vTest1, (signed char * ) "Test1", configMINIMAL_STACK_SIZE, NULL, (mainLED_TASK_PRIORITY+1), NULL );
	xTaskCreate ( vTest2, (signed char * ) "Test2", configMINIMAL_STACK_SIZE, NULL, (mainLED_TASK_PRIORITY+1), NULL );

	/* Start scheduler */
	vTaskStartScheduler();
	
	return 0;
}
/*-----------------------------------------------------------*/

static void vTest1 ( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 500;
	char c[5];

	xLastWakeTime = xTaskGetTickCount ();

	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, xFrequency );

		vSendString ("task1:\n");
		itoa ( xLastWakeTime, &c, 10 );
		vSendString ( c );
		vSendString ( "\n\n" );
	}	
}

static void vTest2 ( void *pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 1000;
	char c[5];

	xLastWakeTime = xTaskGetTickCount ();

	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, xFrequency );

		vSendString ("task2:\n");
		itoa ( xLastWakeTime, &c, 10 );
		vSendString ( c );
		vSendString ( "\n\n" );
	}
}

/*-----------------------------------------------------------*/

void vSendString ( char *s )
{
	while ( *s != 0x00 )
	{
		xSerialPutChar ( NULL, *s, 0 );
		s++;
	}
}

void vApplicationIdleHook( void )
{

}

void vApplicationStackOverflowHook ( xTaskHandle xTask, signed portCHAR *pcTaskName )
{
	vSendString ( "overflow\n" );
}

void vApplicationTickHook ( void )
{

}