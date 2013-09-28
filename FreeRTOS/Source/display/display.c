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

volatile uint8_t *Display_cmd = (uint8_t *) 0x4000;
volatile uint8_t *Display_data = (uint8_t *) 0x4200;

void vDisplayInit (void)
{
/*
	vDisplaySetDisplayOff ();
	vDisplaySendCommand (0xa1); //segment remap
	vDisplaySendCommand (0xda); //common pads hardware: alternative
	vDisplaySendCommand (0x12);
	
	vDisplaySendCommand (0xc8); //common output scan direction:com63~com0
	
	vDisplaySetMuxRatio (0x3F);	
	vDisplaySetDisplayClock (0x80);	
	vDisplaySetContrast (0x50);
	vDisplaySetPreChargePeriod (0x21);
	vDisplaySetMemoryAddressingMode (PAGE_ADDRESSING_MODE);
	vDisplaySetVcomDeselectLevel (VCOMH_DESELECT_0_83);
	vDisplaySetIref (IREF_INTERNAL);
	
	vDisplaySendCommand (Display_DISPLAY_RAM);
	vDisplaySendCommand (Display_DISPLAY_NORMAL);
	vDisplayClearDisplay ();
	vDisplaySetDisplayOn ();*/

	/* Initialize external memory interface */
	XMCRA |= (1 << SRE);
		
	/* Release PC7-PC2 */
	XMCRB |= (1 << XMM2) | (1 << XMM1);

	vDisplaySendCommand (0xae); // display off
	vDisplaySendCommand (0xa1); //segment remap
	vDisplaySendCommand (0xda); //common pads hardware: alternative
	vDisplaySendCommand (0x12);
	vDisplaySendCommand (0xc8); //common output scan direction:com63~com0
	vDisplaySendCommand (0xa8); //multiplex ration mode:63
	vDisplaySendCommand (0x3f);
	vDisplaySendCommand (0xd5); //display divide ratio/osc. freq. mode
	vDisplaySendCommand (0x80);
	vDisplaySendCommand (0x81); //contrast control
	vDisplaySendCommand (0x50);
	vDisplaySendCommand (0xd9); //set pre-charge period
	vDisplaySendCommand (0x21);
	vDisplaySendCommand (0x20); //Set Memory Addressing Mode
	vDisplaySendCommand (0x02);
	vDisplaySendCommand (0xdb); //VCOM deselect level mode
	vDisplaySendCommand (0x30);
	vDisplaySendCommand (0xad); //master configuration
	vDisplaySendCommand (0x00);
	vDisplaySendCommand (0xa4); //out follows RAM content
	vDisplaySendCommand (0xa6); //set normal display
	vDisplaySendCommand (0xaf); // display on
	
	vDisplayClearDisplay ();
}

void vDisplaySendCommand (uint8_t cmd)
{
	
	*Display_cmd = cmd;
}

void vDisplaySendCommands (uint8_t len, uint8_t *cmds)
{
	for (int i = 0; i < len; i++)
	{
		*Display_cmd = cmds[i];
	}
}

void vDisplaySendData (uint8_t data)
{
	*Display_data = data;
}

void vDisplaySetColumn (uint8_t col)
{
	vDisplaySendCommand (0x00 + (col & 0x0F));
	vDisplaySendCommand (0x10 + (col >> 4));
}

void vDisplaySetColumnAddress (uint8_t start, uint8_t end)
{
uint8_t cmds[3] = {DISPLAY_COLUMN_ADDRESS, start, end};
vDisplaySendCommands (3, cmds);
}

void vDisplaySetContrast (uint8_t contrast)
{
	uint8_t cmds[2] = { DISPLAY_CONTRAST, contrast };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetDisplayClock (uint8_t clock)
{
	uint8_t cmds[2] = { DISPLAY_DISPLAY_CLOCK, clock };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetDisplayOn (void)
{
	vDisplaySendCommand (DISPLAY_POWER_ON);
}

void vDisplaySetDisplayOff (void)
{
	vDisplaySendCommand (DISPLAY_POWER_OFF);
}

void vDisplaySetIref (uint8_t ref)
{
	uint8_t cmds[2] = { DISPLAY_SELECT_IREF, ref };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetLine (uint8_t line)
{
	vDisplaySendCommand (DISPLAY_PAGE_START_ADDRESS + line);
	vDisplaySetColumn (0);
}

void vDisplaySetMemoryAddressingMode (uint8_t mode)
{
	uint8_t cmds[2] = { DISPLAY_MEMORY_ADDRESSING_MODE, mode };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetMuxRatio (uint8_t ratio)
{	
	uint8_t cmds[2] = { DISPLAY_MUX_RATIO, ratio };
	vDisplaySendCommands (2, cmds);
}	

void vDisplaySetPageAddress (uint8_t start, uint8_t end)
{
	uint8_t cmds[3] = {DISPLAY_PAGE_ADDRESS, start, end};
	vDisplaySendCommands (3, cmds);
}

void vDisplaySetPosition (uint8_t line, uint8_t col)
{
	vDisplaySetLine (line);
	vDisplaySetColumn (col * FONT_WIDTH);
}

void vDisplaySetPreChargePeriod (uint8_t period)
{
	uint8_t cmds[2] = { DISPLAY_PRE_CHARGE_PERIOD, period };
	vDisplaySendCommands (2, cmds);
}

void vDisplaySetVcomDeselectLevel (uint8_t level)
{
	uint8_t cmds[2] = { DISPLAY_VCOMH_DESELECT_LEVEL, level };
	vDisplaySendCommands (2, cmds);
}

void vDisplayClearDisplay (void)
{
	vDisplaySetDisplayOff ();
	for (uint8_t line = 0; line < PAGES; line++)
	{
		vDisplayClearLine (line);
	}
	vDisplaySetDisplayOn ();
}

void vDisplayClearLine (uint8_t line)
{
	vDisplaySetLine (line);

	for (uint8_t col = 0; col < COLUMNS; col++)
	{
		vDisplaySendData (0);
	}
}

void vDisplayPutString ( const char line, const signed char * const pcString, unsigned short usStringLength )
{
	vDisplaySetLine ( line );
	for (unsigned short i = 0; i < usStringLength; i++)
	{
		vDisplayPutChar ( pcString[i] );
	}
}

void vDisplayPutChar (char c)
{
	for (uint8_t i = 0; i < FONT_WIDTH; i++)
	{
		vDisplaySendData (pgm_read_byte (&font[c - ' '][i]));
	}
}