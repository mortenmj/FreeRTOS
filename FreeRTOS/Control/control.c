/*
 * control.c
 *
 * Created: 02.10.2013 23:26:51
 *  Author: mortenmj
 */ 

#include "FreeRTOS.h"
#include "task.h"
#include "control.h"

#include "Control/can/can.h"
#include "Control/adc/adc.h"

#include "Control/can/MCP2515define.h"

/* Task frequency */
#define ctrlTASK_FREQUENCY				( ( const portTickType ) 200 )

/* Number of ADC values to queue */
#define ctrlNUM_ADC_VALUES				( 2 )

/* 
 * This is ugly and temporary
 */
signed char valx, valy;
xCanFrame xOutFrame = {1, {'w','t','f'}, 3};
xCanFrame xInFrame;

void vControl ( void *pvParameters )
{
	portTickType xLastWakeTime;
	
	xLastWakeTime = xTaskGetTickCount ();
	
	/* CAN init */
	vCanInit ();
	
	/* ADC init */
	vAdcInit ( ctrlNUM_ADC_VALUES );
	
	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, ctrlTASK_FREQUENCY );
		
		if ( xAdcTakeSemaphore () == pdTRUE )
		{
			xAdcGetValue ( &valx, 0 );
			xAdcGetValue ( &valy, 0 );
			
			vAdcStartConversion ();
		}
		
		vCanSendPacket ( &xOutFrame );
		vCanReceivePacket ( &xInFrame );

/*
		vCanRead (MCP2515_CANSTAT, &canstat);
		vCanRead (MCP2515_CANINTF, &canintf);

		vCanRead (MCP2515_CNF1, &cnf1);
		vCanRead (MCP2515_CNF2, &cnf2);
		vCanRead (MCP2515_CNF3, &cnf3);
*/
	}
}