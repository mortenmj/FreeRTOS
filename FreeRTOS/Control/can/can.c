/*
 * can.c
 *
 * Created: 01.10.2013 20:11:09
 *  Author: mortenmj
 */

#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "MCP2515.h"
#include "MCP2515define.h"
#include "can.h"

xCanStatus xCan;

void vCanInit ( void )
{
	mcp2515_spi_port_init ();
		
	mcp2515_set_baudrate(250000, 1);

	/* Enable all interrupts */
	mcp2515_write ( MCP2515_CANINTE, 0xFF );

	/* Set loopback mode */
	mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_MASK, MCP2515_MODE_LOOPBACK );
}


void vCanRead ( unsigned char ucAddr, unsigned char *pucInChar )
{
	mcp2515_read ( (uint8_t) ucAddr, (uint8_t *) pucInChar );
}

void vCanReceivePacket ( xCanFrame *pxInFrame )
{
	mcp2515_read_rx_buf(MCP2515_RX_BUF_0, (mcp2515_can_frame_t *) pxInFrame);
}

void vCanSendPacket ( xCanFrame *pxOutFrame )
{
	mcp2515_load_tx_buf(MCP2515_TX_BUF_0, (mcp2515_can_frame_t *) pxOutFrame);
	mcp2515_rts (MCP2515_TX_BUF_0);
}

void vCanWrite ( unsigned char ucAddr, unsigned char ucOutChar )
{
	mcp2515_write ( (uint8_t) ucAddr, (uint8_t) ucOutChar );
}

/* Handle interrupts from MCP2515 */
ISR ( PCINT0_vect )
{
	mcp2515_read ( MCP2515_CANINTF, &xCan.interrupt );
}