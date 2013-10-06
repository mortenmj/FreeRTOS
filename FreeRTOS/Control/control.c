/*
 * control.c
 *
 * Created: 02.10.2013 23:26:51
 *  Author: mortenmj
 */ 

#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "control.h"

#include "adc.h"
#include "serial.h"

#include "can.h"
#include "MCP2515define.h"

#include "radio.h"

/* Task frequency */
#define ctrlTASK_FREQUENCY				( ( const portTickType ) 200 )

/* Number of ADC values to queue */
#define ctrlNUM_ADC_VALUES				( 2 )

void vControl ( void *pvParameters )
{
	portTickType xLastWakeTime;
/*
	signed char valx, valy;
	xCanFrame xOutFrame = {1, {'w', 't', 'f'}, 3};
	xCanFrame xInFrame;
*/	
	radiopacket_t packet;
	uint8_t local_addr[5] = { 0x15, 0x15, 0x15, 0x15, 0x15 };
	/*uint8_t success_rate;
	char rate[3];*/
	
	xLastWakeTime = xTaskGetTickCount ();
	
	/* CAN init */
	//vCanInit ();
	
	/* ADC init */
	//vAdcInit ( ctrlNUM_ADC_VALUES );
	
	/* Radio init */
	Radio_Init ();
	Radio_Configure (RADIO_2MBPS, RADIO_HIGHEST_POWER);
	Radio_Configure_Rx(RADIO_PIPE_0, local_addr, ENABLE);

/*
	packet.type = MESSAGE;
	packet.payload.message.address[0] = remote_addr[0];
	packet.payload.message.address[1] = remote_addr[1];
	packet.payload.message.address[2] = remote_addr[2];
	packet.payload.message.address[3] = remote_addr[3];
	packet.payload.message.address[4] = remote_addr[4];
*/

	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, ctrlTASK_FREQUENCY );
/*
		
		if ( xAdcTakeSemaphore () == pdTRUE )
		{
			xAdcGetValue ( &valx, 0 );
			xAdcGetValue ( &valy, 0 );
			
			vAdcStartConversion ();
		}
*/
		//Radio_Set_Tx_Addr(station_addr);
		//Radio_Transmit (&packet, RADIO_WAIT_FOR_TX);
		//Radio_Receive(&packet);
	}
}

/* Handle interrupts from SPI connections */
ISR ( PCINT0_vect )
{
	Radio_Interrupt_Handler ();
	//mcp2515_read ( MCP2515_CANINTF, &xCan.interrupt );
}