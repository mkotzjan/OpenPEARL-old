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
#ifndef ADS1015SE_H_INCLUDED
#define ADS1015SE_H_INCLUDED
/**
\file

\brief Basic system device for the ADS1015 I2C basic dation as single ended ADC

*/

#include "SystemDationB.h"
#include "Fixed.h"
#include "Signals.h"
#include "I2CProvider.h"

#include <stdint.h>

namespace pearlrt {
   /**
   \addtogroup io_common_drivers
   @{
   */

   /**

   \brief Basic system device for an i2c element pcf8574  basic dation

      This device provides a simple input and output of an bit(x) values

   */

   class ADS1015SE: public SystemDationB {

   private:
      int16_t addr;
      int handle;
      uint8_t channel, gain;
      I2CProvider * provider;

   public:
      /**
      constructor to create the bit group and set the
      bits to output direction

      \param provider pointer to the i2cbus device
      \param _addr i2c adress of the device
      \param ch channel numbner (0..3)
      \param g gain (0=2/3,1,2,4,8,16)

      \throws DationParamSignal in case of init failure

      */
      ADS1015SE(I2CProvider * provider, int _addr, int ch, int g);

      /**
      Open the  dation

      \param openParam open parameters if given
      \param idf pointer to IDF-value if given
      \returns pointer to the SampleDationB object itself as working
               object in the user dation

      \throws DationNotOpenSignal, if  dation is not closed and rst is not given
      */
      ADS1015SE* dationOpen(const char* idf = 0, int openParam = 0);

      /**
      Close the dation

      \param closeParam close parameters if given
      */
      void dationClose(int closeParam = 0);

      /**
      read  a FIXED(15) value from the device.


      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws DationParamSignal, if size != 2
      \throws DationNotOpenSignal, if  dation is not opened
      */
      void dationRead(void * data, size_t size);

      /**
      send  a  value to the device
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws InternalDationSignal, in any case
      */
      void dationWrite(void * data, size_t size);

      /**
      obtain the capabilities of the device

      This method returns :
            IN 

      does not return:
            OUT IDF  PRM CAN NEW  OLD ANY INOUT 

      \returns available commands of the device
      */
      int capabilities();
   };
   /** @} */
}
#endif

