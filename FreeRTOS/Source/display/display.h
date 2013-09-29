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

#define dispROWS		64
#define dispCOLUMNS		128
#define dispPAGES		8
#define dispPAGE_WIDTH	(dispROWS/8)
#define dispFONT_WIDTH	8
#define dispCHARS		(dispCOLUMNS / dispFONT_WIDTH)
#define dispMAX_PAGE	(dispPAGES - 1)
#define dispMAX_COL		(dispCOLUMNS - 1)
#define dispBUFSIZE		8*dispCOLUMNS

#define dispRESET					0x08

#define dispHORIZONTAL_ADDRESSING_MODE		0x00
#define dispVERTICAL_ADDRESSING_MODE		0x01
#define dispPAGE_ADDRESSING_MODE			0x02
#define dispMEMORY_ADDRESSING_MODE	0x20

#define dispCOLUMN_ADDRESS			0x21
#define dispPAGE_ADDRESS			0x22
#define dispCONTRAST				0x81
#define dispSEGMENT_REMAP_START		0xA0
#define dispSEGMENT_REMAP_END		0xA1
#define dispDISPLAY_RAM				0xA4
#define dispENABLE					0xA5
#define dispDISPLAY_NORMAL			0xA6
#define dispDISPLAY_INVERSE			0xA7
#define dispMUX_RATIO				0xA8

#define dispIREF_INTERNAL					0x00
#define dispIREF_EXTERNAL					0x10
#define dispSELECT_IREF				0xAD

#define dispPOWER_OFF				0xAE
#define dispPOWER_ON				0xAF
#define dispCMD_MODE				0x80
#define dispDATA_MODE				0x40

#define dispVCOMH_DESELECT_0_65				0x00
#define dispVCOMH_DESELECT_0_77				0x20
#define dispVCOMH_DESELECT_0_83				0x30
#define dispVCOMH_DESELECT_LEVEL	0xDB

#define dispPAGE_START_ADDRESS		0xB0

#define dispOFFSET			0xD3
#define dispCLOCK			0xD5
#define dispPRE_CHARGE_PERIOD		0xD9
#define dispCOM_PINS				0xDA

void vDisplayInit (void);
void vDisplaySendCommand (unsigned char cmd);
void vDisplaySendCommands (unsigned char len, unsigned char *cmds);
void vDisplaySendData (unsigned char data);

void vDisplaySetColumn (unsigned char col);
void vDisplaySetColumnAddress (unsigned char start, unsigned char end);
void vDisplaySetContrast (unsigned char contrast);
void vDisplaySetDisplayClock (unsigned char clock);
void vDisplaySetDisplayOn (void);
void vDisplaySetDisplayOff (void);
void vDisplaySetIref (unsigned char ref);
void vDisplaySetLine (unsigned char line);
void vDisplaySetMemoryAddressingMode (unsigned char mode);
void vDisplaySetMuxRatio (unsigned char ratio);
void vDisplaySetPageAddress (unsigned char start, unsigned char end);
void vDisplaySetPosition (unsigned char line, unsigned char col);
void vDisplaySetPreChargePeriod (unsigned char period);
void vDisplaySetVcomDeselectLevel (unsigned char level);

void vDisplayClear ( void );
void vDisplayClearLine ( unsigned char cLine );
void vDisplayPutString ( const unsigned char cLine, const signed char * const pcString, unsigned short usStringLength );
void vDisplayPutChar ( const unsigned char cLine, const unsigned char cPos, const unsigned char cOutChar );

void vDisplayBufferInit ( void );
void vDisplayBufferWrite ( void );
void vDisplayBufferSwap ( void );
void vDisplayBufferClear ( void );

#endif /* OLED_H_ */