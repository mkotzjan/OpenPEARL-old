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
#ifdef USE_DMA
#include "gpdma_17xx_40xx.h"
#endif

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
         Chip_SSP_SetMaster(LPC_SSP0, 1);
         Chip_SSP_SetFormat(LPC_SSP0, SSP_BITS_8, SSP_FRAMEFORMAT_SPI,
                            SSP_CLOCK_CPHA0_CPOL0);

#ifdef USE_DMA
         for (int i = 0; i < 512; i++) {
             ff[i] = 0xff;
         }

         Chip_GPDMA_Init(LPC_GPDMA);
         dmaTxChannel = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_SSP0_Tx);
         dmaRxChannel = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_SSP0_Rx);
#endif
         spiSelect(0);
      }

      void Lpc17xxSDCard::spiHighSpeed(int highSpeed) {
         if (highSpeed) {
            // Measurements with oscilloscope
            // 10 MHz --> results in 6.25 Mhz
            // 25 MHz --> results in 12.5MHz
            // 30 MHz			 	 12,5MHz
            // 50MHz				 12,5MHz
            // 100MHz				 12,5MHz
            // let's request 25MHz, since we dont't get more the 12.5MHz
            Chip_SSP_SetBitRate(LPC_SSP0, 25000000);
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
#ifdef USE_DMA
         int ret;
         char recv;
	 Chip_SSP_DMA_Enable(LPC_SSP0);
		 ret = Chip_GPDMA_Transfer(LPC_GPDMA, dmaRxChannel,
                  GPDMA_CONN_SSP0_Rx,
                  (uint32_t)(&recv),
                  GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
                  1);

         ret = Chip_GPDMA_Transfer(LPC_GPDMA, dmaTxChannel,
                           (uint32_t)(&data), 
                           GPDMA_CONN_SSP0_Tx,
                           GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
                           1);

         ret =0 ;
         while (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaTxChannel) != SUCCESS) ret++;
         ret =0 ;
         while (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaRxChannel) != SUCCESS) ret++;
         return recv;
#else
         int sr;
         Chip_SSP_SendFrame(LPC_SSP0 , data & 0x0ff);

         do {
            sr = Chip_SSP_GetStatus(LPC_SSP0, SSP_STAT_BSY); // wait while busy
         } while (sr); // wait while busy

         return Chip_SSP_ReceiveFrame(LPC_SSP0);
#endif
      }

      /**
      send a block of data to spi

      \param buffer the location of the data to send
      \param len the number of bytes to send
      */
      void Lpc17xxSDCard::spiSendBlock(char * buffer, int len) {
#ifdef USE_DMA
         int ret;
printf("GPDMATransfer write start: adr=%p   bytes=%d\n", buffer,  len);
	 Chip_SSP_DMA_Enable(LPC_SSP0);
         ret = Chip_GPDMA_Transfer(LPC_GPDMA, dmaTxChannel, (uint32_t)buffer, 
                           GPDMA_CONN_SSP0_Tx,
                           GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
                           len);
printf("GPDMATRansfer write %d SUCCESS=%d\n", ret, SUCCESS);
         ret =0 ;
         while (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaTxChannel) != SUCCESS) ret++;
printf("GPDMATRansfer write finished (%d)\n", ret);
#else
         int i;

         for (i = 0; i < len; i++) {
            spiRwByte(buffer[i]);
         }
#endif
      }

      /**
      read a block of data from spi

      \param buffer the location of the data to store
      \param len the number of bytes to read
      */
      void Lpc17xxSDCard::spiReadBlock(char * buffer, int len) {
#ifdef USE_DMA
         int ret;

printf("read GPDMATransfer start: adr=%p ff=%p bytes=%d\n", buffer, ff, len);

	 Chip_SSP_DMA_Enable(LPC_SSP0);
		 ret = Chip_GPDMA_Transfer(LPC_GPDMA, dmaRxChannel,
                  GPDMA_CONN_SSP0_Rx,
                  (uint32_t)buffer,
                  GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
                  len);

         ret = Chip_GPDMA_Transfer(LPC_GPDMA, dmaTxChannel, (uint32_t)ff, 
                           GPDMA_CONN_SSP0_Tx,
                           GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
                           len);

printf("read GPDMATRansfer %d SUCCESS=%d\n", ret, SUCCESS);
         ret =0 ;
         while (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaTxChannel) != SUCCESS) ret++;
printf("read GPDMATRansfer finished (%d)\n", ret);
         ret =0 ;
         while (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaRxChannel) != SUCCESS) ret++;
printf("read GPDMATRansfer finished (%d)\n", ret);
#else
         int i;

         for (i = 0; i < len; i++) {
            buffer[i] = spiRwByte(0x0ff);
         }
#endif
      }

      int Lpc17xxSDCard::spiIsCardInserted() {
         // the hardware works on negative logic
         return !Chip_GPIO_GetPinState(LPC_GPIO, 3, 25);
      }

   }
}

