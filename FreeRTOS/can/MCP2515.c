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

/*! \file MCP2515.c
    \brief API for Microchip MCP2515 CAN controller.

    This CAN controller use the SPI link. For that reason, SPI initiation and data
    control should custom by the user.

     \author Sy Sech VONG
             Frédéric Nadeau
 */

#include "FreeRTOSConfig.h"

#include "MCP2515define.h"
#include "MCP2515.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

/*! \fn uint8_t mcp2515_spi_port_init(void)
 *  \brief	Provide the initiation of the SPI port to be use for the MCP2515 transmission.
 */
extern void mcp2515_spi_port_init(void);

/*! \fn void mcp2515_spi_select(void)
 *  \brief	Provide the selection of the MCP2515 on the SPI bus to be use for the transmission.
 */
extern void mcp2515_spi_select(void);

/*! \fn void mcp2515_spi_unselect(void)
 *  \brief	Provide the remove of the MCP2515 on the SPI bus.
 *  \param
 *  \return
 */
extern void mcp2515_spi_unselect(void);

/*! \fn uint8_t mcp2515_spi_transfer(uint8_t dataOut, uint8_t *dataIn)
 *  \brief Provide the tranmission service on the SPI port to MCP2515.
 *  \param dataOut Data to be send.
 *  \param dataIn Pointer to 8 bits data space. Read value will be saved at specified address.
 */
extern void mcp2515_spi_transfer(uint8_t dataOut, uint8_t *dataIn);

/*! \fn uint8_t MCP2515canIDRead(uint32_t *canID)
 *  \brief Read the Std or Ext ID from the correct register.
 *
 *   This is use along with the bust method which meant that the "!CS" is
 *   already lowed ( by mcp2515_spi_select() )and the next address of the
 *   register must XXXnSIDH.
 *  \param canID is the address of the value Std or Ext ID with 29th bit = 0 for Std ID or
 *   29th bit = 1 for Ext ID. It returns the CAN ID locate in *canID provided.
 */
static void id_read(uint32_t *canID)
{
    uint8_t dataIn;
    uint32_t uwID = 0;

    mcp2515_spi_transfer(0, &dataIn);//read XXXnSIDH
    uwID = (((uint32_t)dataIn) << 21);
    mcp2515_spi_transfer(0, &dataIn);//read XXXnSIDL
    uwID |= ( ((uint32_t)(dataIn & 0xE0)) << 13);
    uwID |= ( ((uint32_t)(dataIn & 0x03)) << 16);

    if (dataIn & 0x08) //Ext. ID? (bit mask on bit3)
        uwID |= 0x20000000;

    mcp2515_spi_transfer(0, &dataIn);//read XXXnEID8
    uwID |= (((uint32_t)dataIn) << 8);
    mcp2515_spi_transfer(0, &dataIn);//read XXXnEID0
    uwID |= (uint32_t)dataIn;
    *canID = uwID;
}

/*! \fn uint8_t MCP2515canIDWrite(uint32_t canID)
 *  \brief Write the Std or Ext ID to the correct register.
 *
 *   This is use along with the bust method which meant that the "!CS" is
 *   already lowed ( by mcp2515_spi_select() )and the next address of the
 *   register must XXXnSIDH.
 *  \param canID is the Std or Ext ID with 29th bit = 0 for Std ID or 29th bit = 1 for Ext ID.
 */
static void id_write(uint32_t canID)
{
    uint8_t ubtempo = 0;

    mcp2515_spi_transfer((uint8_t) (canID >> 21), NULL);//send XXXnSIDH
    ubtempo = ( (uint8_t)(canID >> 13) ) & 0xE0;
    ubtempo |= ( (uint8_t)(canID >> 16) ) & 0x03;

    if (canID & 0x20000000)//Ext. ID?
        ubtempo |= 0x08;

    mcp2515_spi_transfer(ubtempo, NULL);//send XXXnSIDL
    mcp2515_spi_transfer((uint8_t) (canID >> 8), NULL);//Send XXXnEID8
    mcp2515_spi_transfer((uint8_t) canID, NULL);//Send XXXnEID0
}

void mcp2515_reset(void)
{
    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_RESET, NULL);
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_read(uint8_t regAddr, uint8_t *dataAddr)
{
    assert(dataAddr != 0);

    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_READ, NULL);
    mcp2515_spi_transfer(regAddr, NULL);
    mcp2515_spi_transfer(0, dataAddr);
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_read_bust(uint8_t regAddr, uint8_t *dataAddr, uint8_t size)
{
    uint8_t i;

    assert((dataAddr != NULL) && (size != 0));

    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_READ, NULL);
    mcp2515_spi_transfer(regAddr, NULL);

    for (i=0; i <= size; i++)
        mcp2515_spi_transfer(0, dataAddr++);

    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus


}

void mcp2515_read_rx_buf(uint8_t nBuf, mcp2515_can_frame_t *canData)
{
    uint8_t i;

    assert(canData != NULL);

    switch (nBuf)
    {
        case MCP2515_RX_BUF_0:
        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_READ_BUF_RXB0SIDH, NULL);
        break;
        case MCP2515_RX_BUF_1:
        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_READ_BUF_RXB1SIDH, NULL);
        break;
        default:
        assert(0);
        //break;
    }
    id_read(&canData->id);

    mcp2515_spi_transfer(0, &canData->dlc);//read DLC
    canData->dlc &= 0x0F;
    for (i = 0; i < canData->dlc; i++)
    {
        mcp2515_spi_transfer(0, &canData->data[i]);
    }
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_write(uint8_t regAddr, uint8_t data)
{
    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_WRITE, NULL);
    mcp2515_spi_transfer(regAddr, NULL);
    mcp2515_spi_transfer(data, NULL);
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_write_bust(uint8_t regAddr, uint8_t *dataAddr, uint8_t size)
{
    uint8_t i;

    assert(dataAddr != NULL);

    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_WRITE, NULL);
    mcp2515_spi_transfer(regAddr, NULL);

    for (i=0; i < size; i++)
        mcp2515_spi_transfer(dataAddr[i], NULL);

    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_load_tx_buf(uint8_t nBuf, mcp2515_can_frame_t *canData)
{
    uint8_t i;

    assert(canData != NULL);

    switch (nBuf)
    {
    case MCP2515_TX_BUF_0:
        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_LOAD_BUF_TXB0SIDH, NULL);
        break;
    case MCP2515_TX_BUF_1:
        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_LOAD_BUF_TXB1SIDH, NULL);
        break;
    case MCP2515_TX_BUF_2:
        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_LOAD_BUF_TXB2SIDH, NULL);
        break;
    default:
        assert(0);
        //break;
    }

    id_write(canData->id);
    mcp2515_spi_transfer(canData->dlc & 0x0F, NULL);
    for (i = 0; (i < canData->dlc) && (i < MCP2515_MAX_BYTE_CANFRM); i++)
    {
        mcp2515_spi_transfer(canData->data[i], NULL);
    }
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_rts(uint8_t nBuf)
{
    //uint8_t ubRetVal = SPI_OK;
    uint8_t dataIn;

    mcp2515_spi_select();//Select the MCP2515 on the SPI bus

    switch (nBuf)
    {
    case MCP2515_TX_BUF_0:
        mcp2515_spi_transfer(MCP2515_RTS_TXB0, &dataIn);
        break;
    case MCP2515_TX_BUF_1:
        mcp2515_spi_transfer(MCP2515_RTS_TXB1, &dataIn);
        break;
    case MCP2515_TX_BUF_2:
        mcp2515_spi_transfer(MCP2515_RTS_TXB2, &dataIn);
        break;
    default:
        assert(0);
        //break;
    }
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_read_rxtx_status(uint8_t* canRxTxStatus)
{
    assert(canRxTxStatus != NULL);

    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_READ_RXTX_STATUS, NULL);
    mcp2515_spi_transfer(0, canRxTxStatus);

    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_rx_status(uint8_t* canRxStatus)
{

    assert(canRxStatus != NULL);

    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_READ_RXTX_STATUS, NULL);
    mcp2515_spi_transfer(0, canRxStatus);
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}

void mcp2515_bit_modify(uint8_t regAddr, uint8_t bitMask, uint8_t val)
{
    mcp2515_spi_select();//Select the MCP2515 on the SPI bus
    mcp2515_spi_transfer(MCP2515_BIT_MODIFY, NULL);
    mcp2515_spi_transfer(regAddr, NULL);
    mcp2515_spi_transfer(bitMask, NULL);
    mcp2515_spi_transfer(val, NULL);
    mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
}



void mcp2515_id_multi_read(uint8_t regAddr, uint32_t *canID)
{
    switch (regAddr)
    {
    case MCP2515_RXF0SIDH:
    case MCP2515_RXF1SIDH:
    case MCP2515_RXF2SIDH:
    case MCP2515_RXF3SIDH:
    case MCP2515_RXF5SIDH:
    case MCP2515_RXM0SIDH:
    case MCP2515_RXM1SIDH:
    case MCP2515_RXB0SIDH:
    case MCP2515_RXB1SIDH:
    case MCP2515_TXB0SIDH:
    case MCP2515_TXB1SIDH:
    case MCP2515_TXB2SIDH:
        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_READ, NULL);
        mcp2515_spi_transfer(regAddr, NULL);
        id_read(canID);
        mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
        break;
    default:
        assert(0);
    }
}

void mcp2515_id_multi_write(uint8_t regAddr, uint32_t canID)
{
    switch (regAddr)
    {
    case MCP2515_RXF0SIDH:
    case MCP2515_RXF1SIDH:
    case MCP2515_RXF2SIDH:
    case MCP2515_RXF3SIDH:
    case MCP2515_RXF5SIDH:
    case MCP2515_RXM0SIDH:
    case MCP2515_RXM1SIDH:
    case MCP2515_RXB0SIDH:
    case MCP2515_RXB1SIDH:
    case MCP2515_TXB0SIDH:
    case MCP2515_TXB1SIDH:
    case MCP2515_TXB2SIDH:
        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_WRITE, NULL);
        mcp2515_spi_transfer(regAddr, NULL);
        id_write(canID);
        mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
        break;
    default:
        assert(0);
    }
}

void mcp2515_set_baudrate(uint32_t CANBaudRate, uint8_t syncSJW)
{
    uint8_t ubCNF1 = 0;
    uint8_t ubCNF2 = 0x80; //activate user define PS2 & 1 sample
    uint8_t ubCNF3 = 0;
    uint8_t ubBRP = 0;
    uint8_t ubnTQ; //Total of TQ in a CAN bus bit time
    uint8_t ubnTQtempo; //nTQ temporally

    if ( !((--syncSJW) > MCP2515_MAX_SJW) ); //SJW
        ubCNF1 = (syncSJW << 0x06);

    if ( (CANBaudRate != 0) && (CANBaudRate <= 1000000L) )
    {

        //Set the Bit Rate Prescale (BRP).
        //total TQ = F_CPU / (2*(BRP+1)*F_CAN)
        do{
            ubnTQ = (uint8_t) ( F_CPU / ((uint32_t)((ubBRP + 1) << 1) * CANBaudRate) ) ;
        }
        while ((++ubBRP <= MCP2515_MAX_BRP) && (ubnTQ > MCP2515_MAX_TQ));

        ubCNF1 |= --ubBRP;

        //For Phase segment 2. Total TQ / 4 to have 75% sampling point.
        ubnTQtempo = ubnTQ >> 2;
        ubCNF3 |= (ubnTQtempo - 1);

        //For Phase segment 1. (rest of TQ) / 2
        ubnTQ = ubnTQ - ubnTQtempo;
        ubnTQtempo = ubnTQ >> 1;
        ubCNF2 |= ((ubnTQtempo - 1) << 3);

        //For Propagation segment.
        ubnTQ = ubnTQ - ubnTQtempo;
        ubCNF2 |= ubnTQ - 2;// -2 bcause 1TQ for Sync segment and 1TQ offset in register.

        mcp2515_spi_select();//Select the MCP2515 on the SPI bus
        mcp2515_spi_transfer(MCP2515_WRITE, NULL);
        mcp2515_spi_transfer(MCP2515_CNF3, NULL);
        mcp2515_spi_transfer(ubCNF3, NULL);
        mcp2515_spi_transfer(ubCNF2, NULL);
        mcp2515_spi_transfer(ubCNF1, NULL);
        mcp2515_spi_unselect();//Unselect the MCP2515 on the SPI bus
    }
}
