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
#include "adc/adc.h"
#include "serial/serial.h"

#include "u8g.h"
//#include "m2.h"
//#include "m2ghu8g.h"
#include "can/MCP2515.h"
#include "can/can.h"
#include "can/can_ll.h"

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

/* Strict timing */
#define mainCONTROL_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )

/* Deadline timing */
#define mainJOYSTICK_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainDISPLAY_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* Flexible timing */
#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY )

/* Baud rate used by the serial port tasks. */
#define mainCOM_BAUD_RATE				( ( unsigned long ) 28800 )

/* Delay needed before initializing display */
#define mainDISPLAY_INIT_DELAY			500
#define mainCAN_INIT_DELAY				500

/* Task frequencies */
#define mainCONTROL_TASK_FREQUENCY		( ( const portTickType ) 200 )
#define mainJOYSTICK_TASK_FREQUENCY		( ( const portTickType ) 1000 )
#define mainDISPLAY_TASK_FREQUENCY		( ( const portTickType ) 200 )

/* LED used by the serial port tasks.  This is toggled on each character Tx,
and mainCOM_TEST_LED + 1 is toggles on each character Rx. */
#define mainCOM_TEST_LED				( 4 )

/* Number of ADC values to queue */
#define mainNUM_ADC_VALUES				( 2 )

/* An address in the EEPROM used to count resets.  This is used to check that
the demo application is not unexpectedly resetting. */
#define mainRESET_COUNT_ADDRESS			( ( void * ) 0x50 )

/* The number of coroutines to create. */
#define mainNUM_FLASH_COROUTINES		( 3 )

//M2_LABEL(hello_world_label, NULL, "Hello World");

/*
 * Called on boot to increment a count stored in the EEPROM.  This is used to 
 * ensure the CPU does not reset unexpectedly.
 */
static void vJoystick ( void *pvParameters );
static void vDisplay ( void *pvParameters );
static void vControl ( void *pvParameters );

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

/* 
 * This is ugly and temporary
 */
signed char valx, valy;
uint8_t canstat;
uint8_t canintf;
uint8_t cnf1;
uint8_t cnf2;
uint8_t cnf3;
xCanFrame xOutFrame = {1, {'w','t','f'}, 3};
xCanFrame xInFrame;

/*-----------------------------------------------------------*/

int main( void )
{	
	/* Connect PD4 & PD6 to LED0 & LED1 */
	DDRD |= (1 << PD4) | (1 << PD6);
	
	PORTD |= (1 << PD4);
	PORTD |= (1 << PD6);
	
	PCMSK0 |= (1 << PCINT4);
	PCICR |= (1 << PCIE0);
		
	xSerialPortInitMinimal ( mainCOM_BAUD_RATE, 100 );
	vSerialPutString ( NULL, (signed char *) "init\n", 5 );
	
	//vAdcInit ( mainNUM_ADC_VALUES );
	vAdcInit ( 10 );
		
	/* Task to process joystick position from ADC */
	//xTaskCreate ( vJoystick, (signed char * ) "Joystick", configMINIMAL_STACK_SIZE, NULL, mainJOYSTICK_TASK_PRIORITY, NULL );
	xTaskCreate ( vDisplay, (signed char * ) "Display", ( configDISPLAY_STACK_SIZE ), NULL, mainDISPLAY_TASK_PRIORITY, NULL );
	xTaskCreate ( vControl, (signed char * ) "Control", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

	/* Start scheduler */
	vTaskStartScheduler();
	
	return 0;
}
/*-----------------------------------------------------------*/

static void vControl ( void *pvParameters )
{
	portTickType xLastWakeTime;
	
	xLastWakeTime = xTaskGetTickCount ();
	
	/* CAN init */	
	vCanInit ();
		
	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, mainCONTROL_TASK_FREQUENCY );
		
		vCanSendPacket ( &xOutFrame );
		vCanReceivePacket ( &xInFrame );

		vCanRead (MCP2515_CANSTAT, &canstat);
		vCanRead (MCP2515_CANINTF, &canintf);

		vCanRead (MCP2515_CNF1, &cnf1);
		vCanRead (MCP2515_CNF2, &cnf2);
		vCanRead (MCP2515_CNF3, &cnf3);
	}
}

static void vJoystick ( void *pvParameters )
{
	portTickType xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount ();

	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, mainJOYSTICK_TASK_FREQUENCY );
	
		if ( xAdcTakeSemaphore () == pdTRUE )
		{
			xAdcGetValue ( &valx, 0 );
			xAdcGetValue ( &valy, 0 );
			
			vAdcStartConversion ();
		}
	}	
}

static void vDisplay ( void *pvParameters )
{
	portTickType xLastWakeTime;
	u8g_t u8g;
	char cCanstat[4];
	char cCanintf[4];
	char cCnf1[4];
	char cCnf2[4];
	char cCnf3[4];
	
	xLastWakeTime = xTaskGetTickCount ();
	
	/* Display requires a short wait before initialization */
	vTaskDelay ( mainDISPLAY_INIT_DELAY / portTICK_RATE_MS );	
	u8g_Init8BitFixedPort(&u8g, &u8g_dev_ssd1308_128x64_parallel, 0, 0, 0, 0, 0);
	u8g_SetFont (&u8g, u8g_font_5x8);
	
/*
	m2_Init (&hello_world_label, m2_es_avr_u8g, m2_eh_6bs, m2_gh_u8g_bfs);
	m2_SetU8g(&u8g, m2_u8g_box_icon);
	m2_SetFont(0, (const void *)u8g_font_5x8);
*/
	
	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, mainDISPLAY_TASK_FREQUENCY );
		
		utoa (canstat, cCanstat, 10);
		utoa (canintf, cCanintf, 10);
		utoa (cnf1, cCnf1, 10);
		utoa (cnf2, cCnf2, 10);
		utoa (cnf3, cCnf3, 10);
		
		u8g_FirstPage(&u8g);
		do
		{
			u8g_DrawStr ( &u8g, 0, 10, "CANSTAT: " );
			u8g_DrawStr ( &u8g, 45, 10, cCanstat );
			
			u8g_DrawStr ( &u8g, 0, 20, "CANINTF: " );
			u8g_DrawStr ( &u8g, 45, 20, cCanintf );

			u8g_DrawStr ( &u8g, 0, 30, "CNF1: " );
			u8g_DrawStr ( &u8g, 45, 30, cCnf1 );
			
			u8g_DrawStr ( &u8g, 0, 40, "CNF2: " );
			u8g_DrawStr ( &u8g, 45, 40, cCnf2 );
			
			u8g_DrawStr ( &u8g, 0, 50, "CNF3: " );
			u8g_DrawStr ( &u8g, 45, 50, cCnf3 );
			
			u8g_DrawLine ( &u8g, 64, 0, 64, 64 );
			
			u8g_DrawStr ( &u8g, 70, 10, (const char *) xInFrame.data );
		} while ( u8g_NextPage( &u8g ) );
	}
}

/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{

}

void vApplicationStackOverflowHook ( xTaskHandle xTask, signed portCHAR *pcTaskName )
{
	vSerialPutString ( NULL, (signed char *) "overflow\n", 9);
}

void vApplicationTickHook ( void )
{

}