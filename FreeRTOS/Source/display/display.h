/*
 * oled.h
 *
 * Created: 15.09.2013 13:45:42
 *  Author: mortenmj
 *  Se https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/SSD1308
 */ 

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

#define ROWS		64
#define COLUMNS		128
#define PAGES		8
#define PAGE_WIDTH	(ROWS/8)
#define FONT_WIDTH	8
#define CHARS		(COLUMNS / FONT_WIDTH)
#define MAX_PAGE	(PAGES - 1)
#define MAX_COL		(COLUMNS - 1)

#define DISPLAY_RESET					0x08

#define HORIZONTAL_ADDRESSING_MODE		0x00
#define VERTICAL_ADDRESSING_MODE		0x01
#define PAGE_ADDRESSING_MODE			0x02
#define DISPLAY_MEMORY_ADDRESSING_MODE	0x20

#define DISPLAY_COLUMN_ADDRESS			0x21
#define DISPLAY_PAGE_ADDRESS			0x22
#define DISPLAY_CONTRAST				0x81
#define DISPLAY_SEGMENT_REMAP_START		0xA0
#define DISPLAY_SEGMENT_REMAP_END		0xA1
#define DISPLAY_DISPLAY_RAM				0xA4
#define DISPLAY_DISPLAY_ON				0xA5
#define DISPLAY_DISPLAY_NORMAL			0xA6
#define DISPLAY_DISPLAY_INVERSE			0xA7
#define DISPLAY_MUX_RATIO				0xA8

#define IREF_INTERNAL					0x00
#define IREF_EXTERNAL					0x10
#define DISPLAY_SELECT_IREF				0xAD

#define DISPLAY_POWER_OFF				0xAE
#define DISPLAY_POWER_ON				0xAF
#define DISPLAY_CMD_MODE				0x80
#define DISPLAY_DATA_MODE				0x40

#define VCOMH_DESELECT_0_65				0x00
#define VCOMH_DESELECT_0_77				0x20
#define VCOMH_DESELECT_0_83				0x30
#define DISPLAY_VCOMH_DESELECT_LEVEL	0xDB

#define DISPLAY_PAGE_START_ADDRESS		0xB0

#define DISPLAY_DISPLAY_OFFSET			0xD3
#define DISPLAY_DISPLAY_CLOCK			0xD5
#define DISPLAY_PRE_CHARGE_PERIOD		0xD9
#define DISPLAY_COM_PINS				0xDA

void vDisplayInit (void);
void vDisplaySendCommand (uint8_t cmd);
void vDisplaySendCommands (uint8_t len, uint8_t *cmds);
void vDisplaySendData (uint8_t data);

void vDisplaySetColumn (uint8_t col);
void vDisplaySetColumnAddress (uint8_t start, uint8_t end);
void vDisplaySetContrast (uint8_t contrast);
void vDisplaySetDisplayClock (uint8_t clock);
void vDisplaySetDisplayOn (void);
void vDisplaySetDisplayOff (void);
void vDisplaySetIref (uint8_t ref);
void vDisplaySetLine (uint8_t line);
void vDisplaySetMemoryAddressingMode (uint8_t mode);
void vDisplaySetMuxRatio (uint8_t ratio);
void vDisplaySetPageAddress (uint8_t start, uint8_t end);
void vDisplaySetPosition (uint8_t line, uint8_t col);
void vDisplaySetPreChargePeriod (uint8_t period);
void vDisplaySetVcomDeselectLevel (uint8_t level);

void vDisplayClearDisplay (void);
void vDisplayClearLine (uint8_t line);
void vDisplayPrint (char *c);
void vDisplayPutChar (char c);


#endif /* OLED_H_ */