/*
 * Display.c
 *
 * Created: 15.09.2013 13:45:31
 *  Author: mortenmj
 */ 

#include "FreeRTOS.h"

#include <stdint.h>
#include <avr/pgmspace.h>

#include "display.h"
#include "font_8x8.h"

volatile unsigned char *ucCommandAddress = (unsigned char *) 0x4000;
volatile unsigned char *ucDataAddress = (unsigned char *) 0x4200;

unsigned char buffer_[2][dispBUFSIZE];
unsigned char *current_;
unsigned char *next_;


void vDisplayInit (void)
{
	/* Initialize external memory interface */
	XMCRA |= (1 << SRE);
		
	/* Release PC7-PC2 */
	XMCRB |= (1 << XMM2) | (1 << XMM1);

	vDisplaySetDisplayOff ();
	vDisplaySendCommand (0xa1); //segment remap
	vDisplaySendCommand (0xda); //common pads hardware: alternative
	vDisplaySendCommand (0x12);
	
	vDisplaySendCommand (0xc8); //common output scan direction:com63~com0
	
	vDisplaySetMuxRatio (0x3F);
	vDisplaySetDisplayClock (0x80);
	vDisplaySetContrast (0x50);
	vDisplaySetPreChargePeriod (0x21);
	vDisplaySetMemoryAddressingMode (dispHORIZONTAL_ADDRESSING_MODE);
	vDisplaySetVcomDeselectLevel (dispVCOMH_DESELECT_0_83);
	vDisplaySetIref (dispIREF_INTERNAL);
	
	vDisplaySendCommand (dispDISPLAY_RAM);
	vDisplaySendCommand (dispDISPLAY_NORMAL);
	vDisplaySetDisplayOn ();
}

void vDisplaySendCommand (unsigned char cmd)
{
	
	*ucCommandAddress = cmd;
}

void vDisplaySendCommands (unsigned char len, unsigned char *cmds)
{
	for (int i = 0; i < len; i++)
	{
		*ucCommandAddress = cmds[i];
	}
}

void vDisplaySendData (unsigned char data)
{
	*ucDataAddress = data;
}

void vDisplaySetColumn (unsigned char col)
{
	vDisplaySendCommand (0x00 + (col & 0x0F));
	vDisplaySendCommand (0x10 + (col >> 4));
}

void vDisplaySetColumnAddress (unsigned char start, unsigned char end)
{
unsigned char cmds[3] = {dispCOLUMN_ADDRESS, start, end};
vDisplaySendCommands (3, cmds);
}

void vDisplaySetContrast (unsigned char contrast)
{
	unsigned char cmds[2] = { dispCONTRAST, contrast };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetDisplayClock (unsigned char clock)
{
	unsigned char cmds[2] = { dispCLOCK, clock };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetDisplayOn (void)
{
	vDisplaySendCommand (dispPOWER_ON);
}

void vDisplaySetDisplayOff (void)
{
	vDisplaySendCommand (dispPOWER_OFF);
}

void vDisplaySetIref (unsigned char ref)
{
	unsigned char cmds[2] = { dispSELECT_IREF, ref };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetLine (unsigned char line)
{
	vDisplaySendCommand (dispPAGE_START_ADDRESS + line);
	vDisplaySetColumn (0);
}

void vDisplaySetMemoryAddressingMode (unsigned char mode)
{
	unsigned char cmds[2] = { dispMEMORY_ADDRESSING_MODE, mode };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetMuxRatio (unsigned char ratio)
{	
	unsigned char cmds[2] = { dispMUX_RATIO, ratio };
	vDisplaySendCommands (2, cmds);
}	

void vDisplaySetPageAddress (unsigned char start, unsigned char end)
{
	unsigned char cmds[3] = { dispPAGE_ADDRESS, start, end};
	vDisplaySendCommands (3, cmds);
}

void vDisplaySetPosition (unsigned char line, unsigned char col)
{
	vDisplaySetLine (line);
	vDisplaySetColumn (col * dispFONT_WIDTH);
}

void vDisplaySetPreChargePeriod (unsigned char period)
{
	unsigned char cmds[2] = { dispPRE_CHARGE_PERIOD, period };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetVcomDeselectLevel (unsigned char level)
{
	unsigned char cmds[2] = { dispVCOMH_DESELECT_LEVEL, level };
	vDisplaySendCommands (2, cmds);
}

void vDisplayClear (void)
{
	for (unsigned char i = 0; i < dispPAGES; i++)
	{
		vDisplayClearLine (i);
	}
}

void vDisplayClearLine ( unsigned char cLine )
{
	/* Line n starts at n*dispCOLUMNS pixels */
	for (unsigned char i = 0; i < dispCOLUMNS; i++)
	{
		next_[ ( cLine * dispCOLUMNS ) + i ] = 0x00;
	}
}

void vDisplayPutString ( const unsigned char cLine, const signed char * const pcString, unsigned short usStringLength )
{
	unsigned short i = 0;
	
	while ( i < usStringLength && pcString[i] != 0x00)
	{
		vDisplayPutChar ( cLine, i, pcString[i++] );
	}
}

void vDisplayPutChar ( const unsigned char cLine, const unsigned char cPos, const unsigned char cOutChar )
{
	for (unsigned char i = 0; i < dispFONT_WIDTH; i++)
	{
		next_[ cLine * dispCOLUMNS + cPos * dispFONT_WIDTH + i ] = pgm_read_byte ( &font[ cOutChar - ' ' ][ i ] );
	}
}

void vDisplayBufferInit ( void )
{
	current_ = buffer_[0];
	next_ = buffer_[1];
}

void vDisplayBufferWrite ( void )
{
	vDisplaySetLine ( 0 );
	for (unsigned short i = 0; i < dispBUFSIZE; i++)
	{
		vDisplaySendData ( current_[i] );
	}
	
	vDisplayBufferSwap ();
}

void vDisplayBufferClear ( void )
{
	for (unsigned short i = 0; i < dispBUFSIZE; i++)
	{
		next_[i] = 0x00;
	}
}

void vDisplayBufferSwap ( void )
{
	unsigned char *tmp = current_;
	current_ = next_;
	next_ = tmp;
}