/*
 * can_ll.c
 *
 * Low-level abstraction layer between CAN module and device driver
 * 
 * Created: 01.10.2013 18:52:01
 *  Author: mortenmj
 */

#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "MCP2515.h"
#include "can_ll.h"
#include "spi.h"

/* Writing 1 to ucAbort terminates pending transfers, writing 0 cancels a previous request */
void vCanLLCtrlAbortPending ( unsigned char ucEnable )
{
	mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_ABORT_TX, ucEnable );
}

void vCanLLCtrlClkoutEnable ( unsigned char ucEnable )
{
	mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_CLKOUT_ENABLE, ucEnable );
}

void vCanLLCtrlClkoutPrescale ( eCanPrescale ePrescale )
{
	switch (ePrescale)
	{
		case canPRESCALE_1:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_CLKOUT_PS8, MCP2515_CLKOUT_PS1 );
			break;
		case canPRESCALE_2:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_CLKOUT_PS8, MCP2515_CLKOUT_PS2 );
			break;
		case canPRESCALE_4:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_CLKOUT_PS8, MCP2515_CLKOUT_PS4 );
			break;
		case canPRESCALE_8:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_CLKOUT_PS8, MCP2515_CLKOUT_PS8 );
			break;
	}			
}

/* Sets the three highest bits of CANCTRL to change operating mode */
void vCanLLCtrlMode ( eCanMode eMode )
{
	switch (eMode)
	{
		case canMODE_NORMAL:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_MASK, MCP2515_MODE_NORMAL );
			break;
		case canMODE_SLEEP:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_MASK, MCP2515_MODE_SLEEP );
			break;
		case canMODE_LOOPBACK:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_MASK, MCP2515_MODE_LOOPBACK );
			break;
		case canMODE_LISTEN:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_MASK, MCP2515_MODE_LISTENONLY );
			break;
		case canMODE_CONFIG:
			mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_MASK, MCP2515_MODE_CONFIG );
			break;
	}
}

void vCanLLCtrlOneShot ( unsigned char ucEnable)
{
	mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_ONESHOT, ucEnable );
}

void vCanLLInit ( void )
{
	mcp2515_spi_port_init ();
	
	mcp2515_set_baudrate(250000, 1);

	/* Enable all interrupts */
	mcp2515_write ( MCP2515_CANINTE, 0xFF );

	/* Set loopback mode */
	mcp2515_bit_modify ( MCP2515_CANCTRL, MCP2515_MODE_MASK, MCP2515_MODE_LOOPBACK );
}

void vCanLLRead ( unsigned char ucAddr, unsigned char *pucInChar )
{
	mcp2515_read ( (uint8_t) ucAddr, (uint8_t *) pucInChar );
}

void vCanLLReset ( void )
{
	mcp2515_reset ();
}

void vCanLLWrite ( unsigned char ucAddr, unsigned char ucOutChar )
{
	mcp2515_write ( (uint8_t) ucAddr, (uint8_t) ucOutChar );
}

void vCanLLTest ( mcp2515_can_frame_t *InFrame )
{
	mcp2515_can_frame_t OutFrame = { 1, {123}, 1 };
	mcp2515_load_tx_buf(MCP2515_TX_BUF_0, &OutFrame);
	mcp2515_rts (MCP2515_TX_BUF_0);
	
	mcp2515_read_rx_buf(MCP2515_RX_BUF_0, InFrame);
}