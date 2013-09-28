/*
 * oled.c
 *
 * Created: 15.09.2013 13:45:31
 *  Author: mortenmj
 */ 

#include <stdint.h>
#include <avr/pgmspace.h>

#include "oled.h"
#include "font_8x8.h"

volatile uint8_t *oled_cmd = (uint8_t *) 0x4000;
volatile uint8_t *oled_data = (uint8_t *) 0x4200;

void vOledInitialize (void)
{
	vOledSetDisplayOff ();
	vOledSendCommand (0xa1); //segment remap
	vOledSendCommand (0xda); //common pads hardware: alternative
	vOledSendCommand (0x12);
	
	vOledSendCommand (0xc8); //common output scan direction:com63~com0
	
	vOledSetMuxRatio (0x3F);	
	vOledSetDisplayClock (0x80);	
	vOledSetContrast (0x50);
	vOledSetPreChargePeriod (0x21);
	vOledSetMemoryAddressingMode (PAGE_ADDRESSING_MODE);
	vOledSetVcomDeselectLevel (VCOMH_DESELECT_0_83);
	vOledSetIref (IREF_INTERNAL);
	
	vOledSendCommand (OLED_DISPLAY_RAM);
	vOledSendCommand (OLED_DISPLAY_NORMAL);
	vOledClearDisplay ();
	vOledSetDisplayOn ();
}

void vOledSendCommand (uint8_t cmd)
{
	
	*oled_cmd = cmd;
}

void vOledSendCommands (uint8_t len, uint8_t *cmds)
{
	for (int i = 0; i < len; i++)
	{
		*oled_cmd = cmds[i];
	}
}

void vOledSendData (uint8_t data)
{
	*oled_data = data;
}

void vOledSetColumn (uint8_t col)
{
	vOledSendCommand (0x00 + (col & 0x0F));
	vOledSendCommand (0x10 + (col >> 4));
}

void vOledSetColumnAddress (uint8_t start, uint8_t end)
{
uint8_t cmds[3] = {OLED_COLUMN_ADDRESS, start, end};
vOledSendCommands (3, cmds);
}

void vOledSetContrast (uint8_t contrast)
{
	uint8_t cmds[2] = { OLED_CONTRAST, contrast };
	vOledSendCommands (2, cmds);
}

void vOledSetDisplayClock (uint8_t clock)
{
	uint8_t cmds[2] = { OLED_DISPLAY_CLOCK, clock };
	vOledSendCommands (2, cmds);
}

void vOledSetDisplayOn (void)
{
	vOledSendCommand (OLED_POWER_ON);
}

void vOledSetDisplayOff (void)
{
	vOledSendCommand (OLED_POWER_OFF);
}

void vOledSetIref (uint8_t ref)
{
	uint8_t cmds[2] = { OLED_SELECT_IREF, ref };
	vOledSendCommands (2, cmds);
}

void vOledSetLine (uint8_t line)
{
	vOledSendCommand (OLED_PAGE_START_ADDRESS + line);
	vOledSetColumn (0);
}

void vOledSetMemoryAddressingMode (uint8_t mode)
{
	uint8_t cmds[2] = { OLED_MEMORY_ADDRESSING_MODE, mode };
	vOledSendCommands (2, cmds);
}

void vOledSetMuxRatio (uint8_t ratio)
{	
	uint8_t cmds[2] = { OLED_MUX_RATIO, ratio };
	vOledSendCommands (2, cmds);
}	

void vOledSetPageAddress (uint8_t start, uint8_t end)
{
	uint8_t cmds[3] = {OLED_PAGE_ADDRESS, start, end};
	vOledSendCommands (3, cmds);
}

void vOledSetPosition (uint8_t line, uint8_t col)
{
	vOledSetLine (line);
	vOledSetColumn (col * FONT_WIDTH);
}

void vOledSetPreChargePeriod (uint8_t period)
{
	uint8_t cmds[2] = { OLED_PRE_CHARGE_PERIOD, period };
	vOledSendCommands (2, cmds);
}

void vOledSetVcomDeselectLevel (uint8_t level)
{
	uint8_t cmds[2] = { OLED_VCOMH_DESELECT_LEVEL, level };
	vOledSendCommands (2, cmds);
}

void vOledClearDisplay (void)
{
	vOledSetDisplayOff ();
	for (uint8_t line = 0; line < PAGES; line++)
	{
		vOledClearLine (line);
	}
	vOledSetDisplayOn ();
}

void vOledClearLine (uint8_t line)
{
	vOledSetLine (line);

	for (uint8_t col = 0; col < COLUMNS; col++)
	{
		vOledSendData (0);
	}
}

void vOledPrint (char *c)
{
	while (*c != '\0')
	{
		vOledPutchar (*c++);
	}
}

void vOledPutchar (char c)
{
	for (uint8_t i = 0; i < FONT_WIDTH; i++)
	{
		vOledSendData (pgm_read_byte (&font[c - ' '][i]));
	}
}