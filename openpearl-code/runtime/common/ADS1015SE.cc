/*
 [A "BSD license"]
 Copyright (c) 2016      Rainer Mueller
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


#include "Dation.h"
#include "ADS1015SE.h"
#include "Log.h"
#include "Signals.h"
#include "BitString.h"
#include <iostream>

/**
 \brief Implementation of the ads1015 i2c-device  basic  systemdation 
   as single ended adc

*/
namespace pearlrt {

   ADS1015SE::ADS1015SE(I2CProvider * provider, int addr, int ch, int g) {
      channel = ch;
      gain = g;
 
      // check of adress, start and width is done by IMC
      dationStatus = CLOSED;

      this->addr = addr;
      this->provider = provider;
   }

   ADS1015SE* ADS1015SE::dationOpen(const char * idf, int params) {

      if (idf) {
         Log::error("ADS1015SE: no IDF allowed");
         throw theDationParamSignal;
      }

      if (params & ~(RST | IN )) {
         Log::error("ADS1015SE: only RST allowed");
         throw theDationParamSignal;
      }

      if (dationStatus != CLOSED) {
         Log::error("ADS1015SE: Dation already open");
         throw theOpenFailedSignal;
      }

      dationStatus = OPENED;
      return this;
   }

   void ADS1015SE::dationClose(int params) {

      if (dationStatus != OPENED) {
         Log::error("ADS1015SE: Dation not open");
         throw theDationNotOpenSignal;
      }

      if (params & ~(RST | IN)) {
         Log::error("ADS1015SE: only RST allowed");
         throw theDationParamSignal;
      }

      dationStatus = CLOSED;
   }

   void ADS1015SE::dationWrite(void* data, size_t size) {
         Log::error("ADS1015SE: no write supported");
         throw theInternalDationSignal;
   }

   void ADS1015SE::dationRead(void* data, size_t size) {
      uint8_t channelSelect[3];
      uint8_t readData[2];
      int resultValue;
      int loopCounter = 0;

      //check size of parameter!
      // it is expected that a Fixed(15)
      if (size != sizeof(Fixed<15>)) {
         Log::error("ADS1015SE: illegal data size (got %d byte data)",
                    (int)size);
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("ADS1015SE: Dation not open");
         throw theDationNotOpenSignal;
      }

      channelSelect[0] = 1; // select config register
      channelSelect[1] = 0xC1 | (channel << 4) | (gain << 1);
      channelSelect[2] = 0xe3; // 3300 sps, others are default
      readData[0] = 0;

      try {
         // select channel and start conversion
         provider->writeData(addr, 3, channelSelect);

         // wait for converion complete
         while ((readData[0] & 0x80) == 0) {
            loopCounter ++;
            provider->readData(addr, 2, readData);
         }

         // select conversiopn register
         channelSelect[0] = 0; 
         provider->writeData(addr, 3, channelSelect);

         // read conversion result
         provider->readData(addr, 2, readData);

      } catch (WritingFailedSignal s) {
         Log::error("ADS1015SE: Dation not ready");
         throw theReadingFailedSignal;
      }
          
      // use shift operator to be independent on byte ordering
      resultValue = readData[0] << 8 | readData[1] ;

      *(int16_t *)data = resultValue;
   }

   int ADS1015SE::capabilities() {
      int cap =  IN;
      return cap;
   }
}

