/* Copyright (c) 2007 Sy Sech VONG
   Copyright (c) 2008,2009 Frédéric Nadeau

   All rights reserved.

   Redistribution and use in source and binary forms,
   with or without modification, are permitted provided
   that the following conditions are met:

   1.Redistributions of source code must retain the above
   copyright notice, this list of conditions and the following
   disclaimer.

   2.Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3.Neither the name of the AVR-DRV nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */


/*! \file MCP2515.h
    \brief API for Atmel AVR MCU to control the Microchip MCP2515 CAN controller.
     This CAN controller use the SPI link. For that reason, SPI initiation and data
     control should custom by the user.

     \author Sy Sech VONG
             Frédéric Nadeau
*/


#ifndef MCP2515_H_
#define MCP2515_H_

#include <stdint.h>

/*! \struct mcp2515_can_frame_s
 *  Structure of a CANbus frame.
 */
typedef struct mcp2515_can_frame_s{
    uint32_t id;        /**< Standard or Extented ID*/
    uint8_t data[8];    /**< Data*/
    uint8_t dlc;        /**< Data Length Code*/
}mcp2515_can_frame_t;   /**< Typedef of #mcp2515_can_frame_s*/


/*! \fn uint8_t mcp2515_reset(void)
 *  \brief Send the RESET command over SPI bus. It has the same effect as
 *  lowing the RESET pin of MCP2515.
 */
void mcp2515_reset(void);

/*! \fn uint8_t mcp2515_read(uint8_t regAddr, uint8_t *dataAddr)
 *  \brief Send the Read command over SPI bus. It can read any register
 *  byte accordingly of the address provided.
 *  \param regAddr is the register address of the MCP2515.
 *  \param dataAddr is the address of the data to be writed on.
 */
void mcp2515_read(uint8_t regAddr, uint8_t *dataAddr);

/*! \fn uint8_t mcp2515_read_bust(uint8_t regAddr, uint8_t *dataAddr, uint8_t size)
 *  \brief Send the READ command over SPI bus. It can read any register
 *  byte accordingly of the address provided. It provides a bust read so please
 *  be careful on the read data that not overlap other MCP2515 registers's data.
 *  \param regAddr is the register address of the MCP2515.
 *  \param dataAddr is the address of the data to be saved.
 *  \param size is the data byte size to be read (n bytes).
 */
void mcp2515_read_bust(uint8_t regAddr, uint8_t *dataAddr, uint8_t size);

/*! \fn uint8_t mcp2515_read_rx_buf(uint8_t nBuf, mcp2515_can_frame_t *canData)
 *  \brief Send the Read RX Buffer command over SPI bus. This reads the whole CAN frame.
 *  \param nBuf buffer of #MCP2515_RX_BUF_0 (buffer RX 0) or #MCP2515_RX_BUF_1 (buffer RX 1).
 *  \param canData is the address of mcp2515_can_frame_t type.
 */
void mcp2515_read_rx_buf(uint8_t nBuf, mcp2515_can_frame_t *canData);

/*! \fn uint8_t mcp2515_write(uint8_t regAddr, uint8_t data)
 *  \brief	Send the WRITE command over SPI bus. It can write any register
 *  byte accordingly of the address provided.
 *  \param	regAddr is the register address of the MCP2515.
 *  \param	data is the data to be send.
 */
void mcp2515_write(uint8_t regAddr, uint8_t data);

/*! \fn uint8_t mcp2515_write_bust(uint8_t regAddr, uint8_t *dataAddr, uint8_t size)
 *  \brief Send the WRITE command over SPI bus. It can write any register
 *  byte accordingly of the address provided. It provides a bust write so please
 *  be careful on the write data that not overlap other MCP2515 registers's data.
 *  \param regAddr is the register address of the MCP2515.
 *  \param dataAddr is the begin address of the data byte to be send.
 *  \param size is the data byte size to be send (n bytes).
 */
void mcp2515_write_bust(uint8_t regAddr, uint8_t *dataAddr, uint8_t size);

/*! \fn uint8_t mcp2515_load_tx_buf(uint8_t nBuf, mcp2515_can_frame_t *canData)
 *  \brief Send the Load TX Buffer command over SPI bus. This loads the whole CAN frame
 *   to the buffer.
 *  \param nBuf is buffer of #MCP2515_TX_BUF_0/1/2 (buffer TX 0,1 or 2).
 *  \param canData is the begin address of mcp2515_can_frame_t type.
 */
void mcp2515_load_tx_buf(uint8_t nBuf, mcp2515_can_frame_t *canData);

/*! \fn uint8_t mcp2515_rts(uint8_t nBuf)
 *  \brief Send the Request-To-Send (RTS) command over SPI bus. This sets a RTS to
 *   the TX buffer.
 *  \param nBuf is buffer of #MCP2515_TX_BUF_0/1/2 (buffer 0,1 or 2).
 */
void mcp2515_rts(uint8_t nBuf);

/*! \fn uint8_t mcp2515_read_rxtx_status(uint8_t* canRxTxStatus)
 *  \brief Send the Read Status command over SPI bus. This requests the CAN controller
 *   to give the RX/TX buffer status.
 *
 *   The return value points by canRxTxStatus will be as below:
 *   MSB<-|7|6|5|4|3|2|1|0|<-LSB
 *   bit7: CANINTF.TX2IF      bit3: CANINTF.TX0IF
 *   bit6: TXB2CNTRL.TXREQ    bit2: TXB0CNTRL.TXREQ
 *   bit5: CANINTF.TX1IF      bit1: CANINTFL.RX1IF
 *   bit4: TXB1CNTRL.TXREQ    bit0: CANINTF.RX0IF
 *  \param canRxTxStatus is the address of the status value to be saved.
 */
void mcp2515_read_rxtx_status(uint8_t* canRxTxStatus);

/*! \fn uint8_t mcp2515_rx_status(uint8_t* canRxStatus)
 *  \brief Send the RX Status command over SPI bus.
 *
 *  This requests the CAN controller
 *  to give the RX buffer status. It gives more information than
 *  MCP2515canReadRxTxStatus(). Please see the MCP2515.pdf p.67 for more information.
 *  \param canRxStatus is the address of the status value to be saved.
 */
void mcp2515_rx_status(uint8_t* canRxStatus);

/*************************************************************************************************/
/*! \fn void mcp2515_bit_modify(uint8_t regAddr, uint8_t bitMask, uint8_t val)
 *  \brief Send the Bit Modify command over SPI bus.
 *
 *  This changes individual bit at
 *  the given register address. Not all registers can be changed that way.
 *  Please see the MCP2515.pdf p.61 and p.64 for more information.
 *  \param regAddr is the register address.
 *  \param bitMask is the bit mask.
 *  \param val is the value to be set.
 *************************************************************************************************/
void mcp2515_bit_modify(uint8_t regAddr, uint8_t bitMask, uint8_t val);

/*! \fn uint8_t mcp2515_id_multi_read(uint8_t regAddr, uint32_t *canID)
 *  \brief Read any Std or Ext ID from the begin address of the register XXXnSIDH.
 *  \param regAddr is the begin address of any CAN ID register with XXXnSIDH. e.i.: RXF3SIDH,
 *   RXM0SIDH, TXB1SIDH, etc.
 *  \param canID is the address of the value Std or Ext ID with 29th bit = 0 for Std ID or
 *   29th bit = 1 for Ext ID. It returns the CAN ID locate in *canID provided.
 */
void mcp2515_id_multi_read(uint8_t regAddr, uint32_t *canID);

/*************************************************************************************************/
/*! \fn void mcp2515_id_multi_write(uint8_t regAddr, uint32_t canID)
 *  \brief Write any Std or Ext ID from the begin address of the register XXXnSIDH.
 *  \param regAddr is the begin address of any CAN ID register with XXXnSIDH. e.i.: RXF3SIDH,
 *   RXM0SIDH, TXB1SIDH, etc.
 *  \param canID is the value Std or Ext ID with 29th bit = 0 for Std ID or
 *   29th bit = 1 for Ext ID.
 *************************************************************************************************/
void mcp2515_id_multi_write(uint8_t regAddr, uint32_t canID);

/*************************************************************************************************/
/*! \fn uint8_t mcp2515_set_baudrate(uint32_t CANBaudRate, uint8_t syncSJW)
 *  \brief Set the speed of the CAN bus.
 *
 *  Some portions of the segment are fixed in CAN bus bit:
 *  SyncSeg = 1TQ. Always.
 *  PropSeg = variable.
 *  PS1 = variable.
 *  PS2 = variable.
 *  SJW = variable. User define.
 *  *Note: CAN bus bit MIN = 7TQ ; MAX = 25TQ. It tries to have at 75% sampling point in a CAN bit
 *  time.
 *  If the CANBaudrate = 0, it will leave the default configuration of the MCP2515.
 *  \param CANBaudrate is the baudrate that you would like to run on the CAN bus.
 *  \param SJW is the Synchronization Jump Width for CAN bus sync. correction (1 to 4TQ).
 *************************************************************************************************/
void mcp2515_set_baudrate(uint32_t CANBaudRate, uint8_t syncSJW);

#endif /*MCP2515_H_*/