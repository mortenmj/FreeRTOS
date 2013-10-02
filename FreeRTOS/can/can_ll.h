/*
 * can_ll.h
 *
 * Low-level abstraction layer between CAN module and device driver
 *
 * Created: 01.10.2013 19:32:02
 *  Author: mortenmj
 */ 


#ifndef CAN_LL_H_
#define CAN_LL_H_

#include "can.h"
#include "MCP2515.h"
#include "MCP2515define.h"

typedef enum
{
	canMODE_NORMAL,
	canMODE_SLEEP,
	canMODE_LOOPBACK,
	canMODE_LISTEN,
	canMODE_CONFIG
} eCanMode;

typedef enum
{
	canPRESCALE_1,
	canPRESCALE_2,
	canPRESCALE_4,
	canPRESCALE_8
} eCanPrescale;

void vCanLLCtrlAbortPending ( unsigned char ucEnable );
void vCanLLCtrlClkoutEnable ( unsigned char ucEnable );
void vCanLLCtrlClkoutPrescale ( eCanPrescale ePrescale );
void vCanLLCtrlMode ( unsigned char ucMode );
void vCanLLCtrlOneShot ( unsigned char ucEnable);

void vCanLLInit ( void );
void vCanLLRead ( unsigned char ucAddr, unsigned char *pucInChar );
void vCanLLReceivePacket ( xCanFrame *pxInFrame );
void vCanLLReset ( void );
void vCanLLSendPacket ( xCanFrame *pxOutFrame );
void vCanLLWrite ( unsigned char ucAddr, unsigned char ucOutChar );
void vCanLLTest ( mcp2515_can_frame_t *InFrame );

#endif /* CAN_LL_H_ */