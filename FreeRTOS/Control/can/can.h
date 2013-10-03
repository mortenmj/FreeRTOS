/*
 * can.h
 *
 * Created: 01.10.2013 19:26:47
 *  Author: mortenmj
 */ 


#ifndef CAN_H_
#define CAN_H_

/*
* CAN status
*/
typedef struct CanStatus {
	/* Interrupt register */
	uint8_t interrupt;
} xCanStatus;

/*
*  CAN frame
*/
typedef struct CanFrame {
	/* Message ID */
	uint32_t id;
	
	/* Message data, up to 8 bytes */
	uint8_t data[8];
	
	/* Length of data */
	uint8_t dlc;
} xCanFrame ;

void vCanInit ( void );
void vCanRead ( unsigned char ucAddr, unsigned char *pucInChar );
void vCanReceivePacket ( xCanFrame *pxInFrame );
void vCanSendPacket ( xCanFrame *pxOutFrame );
void vCanWrite ( unsigned char ucAddr, unsigned char ucOutChar );

#endif /* CAN_H_ */