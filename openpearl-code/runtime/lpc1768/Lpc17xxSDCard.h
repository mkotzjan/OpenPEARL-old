/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

/**
\file

\brief LPC1768 board specific part of spi support
*/

#ifndef LPC17XXSDCARD_INCLUDED
#define LPC17XXSDCARD_INCLUDED
#include "SDCardSpi.h"

/*
3.7.2017 (rm): Attempt to use DMA for sector transfer aborted.
    There was no mode found where command transmission worked after 
    the first sector was read. The reading of CID and CSD worked with DMA
    Consequence: Switch back to polling mode.
*/
//#define USE_DMA

#ifdef USE_DMA
#include <stdint.h>
#endif

namespace pearlrt {
   /**
    Class for SD-card mass storage devices 
   */
   class Lpc17xxSDCard : public SDCardSpi {
   private:
#ifdef USE_DMA
      uint8_t dmaTxChannel, dmaRxChannel;
      char ff[512];
#endif

   public:
      Lpc17xxSDCard();

      void spiHighSpeed(int highSpeed);
      void spiSelect(int select);
      char spiRwByte(char data);
      void spiSendBlock(char*buffer, int len);
      void spiReadBlock(char*buffer, int len);
      int spiIsCardInserted();
   };
}
#endif
