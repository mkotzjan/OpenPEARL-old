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

\brief PCF8574 I2C device exchange data


*/

#include "PCF8574Pool.h"
#include "SystemDationB.h"
#include "Fixed.h"
#include "Signals.h"
#include "I2CProvider.h"
#include "Log.h"

#include <stdint.h>

namespace pearlrt {

   static uint8_t bitMask[8] = {0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

   struct PCF8574Pool::PCF8574Usage PCF8574Pool::used[nbrOfEntries]={0};
   Mutex PCF8574Pool::mutex;
    
   int PCF8574Pool::registr(I2CProvider * provider, uint8_t i2cAdr,
                           int start, int width, int isInput) {
       int i;
       uint8_t bits;

       mutex.lock();
       int found = -1;
       for (i=0; i<nbrOfEntries && found < 0; i++) {
           if (used[i].provider == provider &&
               used[i].i2cAdr   == i2cAdr) {
              found = i;
           }
       }
       
       if (found < 0) {
          // new chip to register
          for (i=0; i<nbrOfEntries && found < 0; i++) {
              if (used[i].provider == NULL &&
                  used[i].i2cAdr   == 0) {
                 found = i;
                 used[found].provider = provider;
                 used[found].i2cAdr = i2cAdr;
              }
          }
       }

       if (found < 0) {
          Log::error("too many PCF8574 system dations defined");
          mutex.unlock();
          throw theInternalDationSignal;
       }

      bits = bitMask[width-1] >> (7-start); 
      if (used[found].usedBits & bits) {
         Log::error("PCF8574: Bits already in use (i2cadr=%x"
                    " start=%d,width=%d)",
                    used[found].i2cAdr, start, width);
          mutex.unlock();
         throw theDationParamSignal;
      }

      used[found].usedBits |= bits;
      if (isInput) {
         used[found].inputBitMask |= bits;
      }

      mutex.unlock();
      return found;   // return index in table
   }

   void PCF8574Pool::writeOutputValue(int handle, uint8_t newValue, uint8_t mask) {
      struct PCF8574Usage * u;
      mutex.lock();
      u = &used[handle];
      u->lastOutputValue &= ~mask;
      u->lastOutputValue |= newValue & mask;
      u->lastOutputValue |= u->inputBitMask;

      u->provider->writeData(u->i2cAdr, 1, &(u->lastOutputValue));
      mutex.unlock();
   }

   uint8_t PCF8574Pool::readInputValue(int handle) {
      uint8_t ret;
      struct PCF8574Usage * u;
      u = &used[handle];

      mutex.lock();
      u->provider->readData(u->i2cAdr, 1, &ret);
      mutex.unlock();
      return ret;
   }
}
