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
#ifndef PCF8574POOL_H_INCLUDED
#define PCF8574POOL_H_INCLUDED
/**
\file

\brief PCF8574 I2C device exchange data


*/

#include "SystemDationB.h"
#include "Fixed.h"
#include "Signals.h"
#include "I2CProvider.h"
#include "Mutex.h"

#include <stdint.h>
/**
\file
*/

namespace pearlrt {

   /**

   \brief Basic device for an i2c element pcf8574  dation

   The different dations on an PCF8574 chip must know about the usage of
   bits and theit last state. This class collects this data.
   All methods are statis and threadsafe by usage a mutex machanism.

   */

   class PCF8574Pool {

   private:
      static const int nbrOfEntries = 10;
      static struct PCF8574Usage {
         I2CProvider * provider;
         uint8_t     i2cAdr;
         uint8_t     usedBits;
         uint8_t     lastOutputValue;
         uint8_t     inputBitMask; 
      } used[nbrOfEntries];

      static Mutex mutex;
 
   public:
      /**
      register a PCF8574In or PCF874Out device

      \param provider pointer to the i2c-bus device object
      \param i2cAdr   the adress of the chip on the i2c bus
      \param start starting bit number
      \param width number of bits
      \param isInput 1, if the bits are input, 0, else

      \return handle to table concrete PCF8574 chip

      \throws DationParamSignal if at least one bit is already in use
      \throws InternalDationError if more than 'nbrOfEntries' are defined

      */
      static int registr(I2CProvider * provider, uint8_t i2cAdr,
                           int start, int width, int isInput);

      /**
      store new  output value

      \param handle of the PCF8574 chip
      \param newValue the new output value
      \param mask the mask of bits to modify
      */
      static void writeOutputValue(int handle, uint8_t newValue, uint8_t mask);

      /**
      get in/out register value

      \param handle of the PCF8574 chip
      \returns the value of the in/outut register
      */
      static uint8_t readInputValue(int handle);
  };
}
#endif

