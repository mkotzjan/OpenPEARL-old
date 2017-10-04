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
#include "PCF8574Out.h"
#include "PCF8574Pool.h"
#include "Log.h"
#include "Signals.h"
#include "BitString.h"
#include <iostream>

/**
 \brief Implementation of the pcf8574(t) i2c-device  basic  systemdation


*/
namespace pearlrt {

   PCF8574Out::PCF8574Out(I2CProvider * provider, int addr, int s, int w) {
      start = s;
      mask = 0;
      for (int i=0; i<w; i++) {
         mask |= (1<<(start-i));
      }
 
      // check of adress, start and width is done by IMC
      handle=PCF8574Pool::registr(provider, addr,s,w,0);  // output
      dationStatus = CLOSED;

   }

   PCF8574Out* PCF8574Out::dationOpen(const char * idf, int params) {

      if (idf) {
         Log::error("PCF8574Out: no IDF allowed");
         throw theDationParamSignal;
      }

      if (params & ~(RST | IN | OUT | INOUT)) {
         Log::error("PCF8574Out: only RST allowed");
         throw theDationParamSignal;
      }

      if (dationStatus != CLOSED) {
         Log::error("PCF8574Out: Dation already open");
         throw theOpenFailedSignal;
      }


      dationStatus = OPENED;
      return this;
   }

   void PCF8574Out::dationClose(int params) {

      if (dationStatus != OPENED) {
         Log::error("PCF8574Out: Dation not open");
         throw theDationNotOpenSignal;
      }

      if (params & ~(RST | IN | OUT | INOUT)) {
         Log::error("PCF8574Out: only RST allowed");
         throw theDationParamSignal;
      }

      dationStatus = CLOSED;
   }

   void PCF8574Out::dationWrite(void* data, size_t size) {
      uint8_t byte;

      //check size of parameter!
      // it is expected that a BIT(8) as maximum
      if (size != sizeof(BitString<8>)) {
         Log::error("PCF8574Out: illegal data size (got %d byte data)",
                (int)size);
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("PCF8574Out: Dation not open");
         throw theDationNotOpenSignal;
      }

      // write data to device memory
      byte = *(uint8_t*)data;
      byte >>= (7-start);
      byte &= mask;
      PCF8574Pool::writeOutputValue(handle, byte, mask);
   }

   void PCF8574Out::dationRead(void* data, size_t size) {

      Log::error("PCF8574Out: no read supported");
      throw theInternalDationSignal;
   }

   int PCF8574Out::capabilities() {
      int cap = OUT;
      return cap;
   }
}

