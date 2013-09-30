/*
  
  u8g_com_atmega_parallel.c

  Universal 8bit Graphics Library
  
  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  

  PIN_D0 8
  PIN_D1 9
  PIN_D2 10
  PIN_D3 11
  PIN_D4 4
  PIN_D5 5
  PIN_D6 6
  PIN_D7 7

  PIN_CS1 14
  PIN_CS2 15
  PIN_RW 16
  PIN_DI 17
  PIN_EN 18
  
  u8g_Init8Bit(u8g, dev, d0, d1, d2, d3, d4, d5, d6, d7, en, cs1, cs2, di, rw, reset)
  u8g_Init8Bit(u8g, dev,  8,    9, 10, 11,   4,   5,   6,   7, 18, 14, 15, 17, 16, U8G_PIN_NONE)

*/

#include "u8g.h"

#if defined(__AVR__)

volatile unsigned char *ucAddress = (unsigned char *) 0x4000;

static void u8g_com_atmega_parallel_write(u8g_t *u8g, uint8_t val) U8G_NOINLINE;
static void u8g_com_atmega_parallel_write(u8g_t *u8g, uint8_t val)
{
  *ucAddress = val;
}


uint8_t u8g_com_atmega_parallel_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  switch(msg)
  {
    case U8G_COM_MSG_INIT:
	  /* Initialize external memory interface */
	  XMCRA |= (1 << SRE);
	
	  /* Release PC7-PC2 */
	  XMCRB |= (1 << XMM2) | (1 << XMM1);

      break;
    case U8G_COM_MSG_STOP:
      break;
    case U8G_COM_MSG_CHIP_SELECT:
       break;
    case U8G_COM_MSG_WRITE_BYTE:
      u8g_com_atmega_parallel_write(u8g, arg_val);
      break;
    case U8G_COM_MSG_WRITE_SEQ:
      {
        register uint8_t *ptr = arg_ptr;
        while( arg_val > 0 )
        {
          u8g_com_atmega_parallel_write(u8g, *ptr++);
          arg_val--;
        }
      }
      break;
    case U8G_COM_MSG_WRITE_SEQ_P:
      {
        register uint8_t *ptr = arg_ptr;
        while( arg_val > 0 )
        {
          u8g_com_atmega_parallel_write(u8g, u8g_pgm_read(ptr));
          ptr++;
          arg_val--;
        }
      }
      break;
    case U8G_COM_MSG_ADDRESS:                     /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
 	  if ( arg_val )
	  {
		  /* Set address to data */
		  ucAddress = (unsigned char *) 0x4200;
	  }
	  else
	  {
		  /* Set address to command */
		  ucAddress = (unsigned char *) 0x4000;
	  }		  		  
      break;
    case U8G_COM_MSG_RESET:
      break;
  }
  return 1;
}

#else

uint8_t u8g_com_atmega_parallel_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  return 1;
}

#endif /* ARDUINO */

