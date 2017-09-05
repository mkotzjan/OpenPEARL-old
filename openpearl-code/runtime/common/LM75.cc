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
#include "LM75.h"
#include "Log.h"
#include "Signals.h"
#include "Fixed.h"
#include <iostream>

/**
 \brief Implementation of the lm75 i2c-device  basic  systemdation


*/
namespace pearlrt {

   LM75::LM75(I2CProvider * provider, int addr) {
      dationStatus = CLOSED;
      this->provider = provider;

      if (addr < 0x48 || addr > 0x4f) {
         Log::error("LM75: illegal addres (%x)", addr);
         throw theDationParamSignal;
      }

      this->addr = addr;
   }

   LM75* LM75::dationOpen(const char * idf, int params) {
      static uint8_t defaultValue[] = {1, 0};
      static uint8_t selectTempReg[] = {0};

      if (idf) {
         Log::error("LM75: no IDF allowed");
         throw theDationParamSignal;
      }

      if (params & ~(RST | IN | OUT | INOUT)) {
         Log::error("LM75: only RST allowed");
         throw theDationParamSignal;
      }

      if (dationStatus != CLOSED) {
         Log::error("LM75: Dation already open");
         throw theOpenFailedSignal;
      }


      // set configuration register to default (0)
      // and switsch back to default read register
      try {
         provider->writeData(addr, 2, defaultValue);
         provider->writeData(addr, 1, selectTempReg);
      } catch (WritingFailedSignal s) {
         Log::error("LM75: Dation not ready");
         throw theOpenFailedSignal;
      }

      dationStatus = OPENED;
      return this;
   }

   void LM75::dationClose(int params) {

      if (dationStatus != OPENED) {
         Log::error("LM75: Dation not open");
         throw theDationNotOpenSignal;
      }

      if (params & ~(RST | IN | OUT | INOUT)) {
         Log::error("LM75: only RST allowed");
         throw theDationParamSignal;
      }

      dationStatus = CLOSED;
   }

   void LM75::dationWrite(void* data, size_t size) {
      Log::error("LM75: no write supported");
      throw theInternalDationSignal;
   }

   void LM75::dationRead(void* data, size_t size) {
      int8_t tempValFromLm75[2];
      int temp;

      //check size of parameter!
      // it is expected that a Fixed<15> object is passed
      // with a maximum of 16 bits. This fits into 2 byte.
      // Therefore size must be 2
      if (size != sizeof(Fixed<15>)) {
         Log::error("LM75: Fixed<15> expected (got %d byte data)", (int)size);
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("LM75: Dation not open");
         throw theDationNotOpenSignal;
      }

      // write data to application memory
      // i2cproder expects unsigned int8 data - we pass int8 for conventient
      // calculation of the 16 bit temperature value from the 9 bit of
      // the sensor
      provider->readData(addr, 2, (uint8_t*)tempValFromLm75);
      temp = (tempValFromLm75[0] << 1) | ((tempValFromLm75[1] >> 7) & 0x01);

      temp *= 5; // 10th of degree
//      printf("--> %d\n", temp);
      Fixed<15> returnValue(temp);

      *(Fixed<15>*)data = returnValue;
   }

   int LM75::capabilities() {
      int cap =  IN;
      return cap;
   }
}

