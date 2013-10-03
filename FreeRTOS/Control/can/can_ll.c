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
#include "can.h"
#include "can_ll.h"

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

void vCanLLReset ( void )
{
	mcp2515_reset ();
}

void vCanLLWrite ( unsigned char ucAddr, unsigned char ucOutChar )
{
	mcp2515_write ( (uint8_t) ucAddr, (uint8_t) ucOutChar );
}