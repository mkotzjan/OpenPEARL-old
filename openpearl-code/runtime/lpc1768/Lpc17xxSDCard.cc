/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Lpc17xxSDCard.h"
#include "Log.h"
#include "chip.h"


extern "C" {
   static const PINMUX_GRP_T pinMuxingSpi[] = {
      {1, 20,   IOCON_FUNC3 },  /* SCK0 */
      {1, 21,   IOCON_FUNC0},   /* SSEL is driven by this module */
      {1, 23,   IOCON_FUNC3},   /* MISO */
      {1, 24,   IOCON_FUNC3},   /* MOSI */
      {3, 25,   IOCON_FUNC0},   /* Card Detect */
      {3, 26,   IOCON_FUNC0}    /* SDCard Power */
   };

   namespace pearlrt {

      Lpc17xxSDCard::Lpc17xxSDCard() {
         Chip_IOCON_SetPinMuxing(LPC_IOCON, pinMuxingSpi,
                                 sizeof(pinMuxingSpi) / sizeof(pinMuxingSpi[0]));
         Chip_SPI_Init(LPC_SPI);
         Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 21); /* SSEL */
         Chip_GPIO_SetPinDIROutput(LPC_GPIO, 3, 26); /* Power */
         Chip_GPIO_SetPinOutLow(LPC_GPIO, 3, 26);   /* Power On */

         Chip_SSP_Init(LPC_SSP0);
         Chip_SSP_Enable(LPC_SSP0);
         spiSelect(0);
      }

      void Lpc17xxSDCard::spiHighSpeed(int highSpeed) {
         if (highSpeed) {
            //Chip_SSP_SetBitRate(LPC_SSP0,1000000);
            Chip_SSP_SetBitRate(LPC_SSP0, 50000000);
         } else {
            Chip_SSP_SetBitRate(LPC_SSP0, 400000);
         }
      }


      void Lpc17xxSDCard::spiSelect(int select) {
         if (select) {
            // select chip
            Chip_GPIO_SetPinOutLow(LPC_GPIO, 1, 21);
         } else {
            Chip_GPIO_SetPinOutHigh(LPC_GPIO, 1, 21);
            spiRwByte(0xff);   // perform a dummy read
         }
      }

      char Lpc17xxSDCard::spiRwByte(char data) {
         int sr;
         Chip_SSP_SendFrame(LPC_SSP0 , data & 0x0ff);

         do {
            sr = Chip_SSP_GetStatus(LPC_SSP0, SSP_STAT_BSY); // wait while busy
         } while (sr); // wait while busy

         return Chip_SSP_ReceiveFrame(LPC_SSP0);
      }

      /**
      send a block of data to spi

      \param buffer the location of the data to send
      \param len the number of bytes to send
      */
      void Lpc17xxSDCard::spiSendBlock(char * buffer, int len) {
         int i;

         for (i = 0; i < len; i++) {
            spiRwByte(buffer[i]);
         }
      }

      /**
      read a block of data from spi

      \param buffer the location of the data to store
      \param len the number of bytes to read
      */
      void Lpc17xxSDCard::spiReadBlock(char * buffer, int len) {
         int i;

         for (i = 0; i < len; i++) {
            buffer[i] = spiRwByte(0x0ff);
         }
      }

      int Lpc17xxSDCard::spiIsCardInserted() {
         // the hardware works on negative logic
         return !Chip_GPIO_GetPinState(LPC_GPIO, 3, 25);
      }

   }
}

