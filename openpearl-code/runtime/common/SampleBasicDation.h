/*
 [The "BSD license"]
 Copyright (c) 2015 Rainer Mueller
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
#ifndef SAMPLEBASICDATION_H_INCLUDED
#define SAMPLEBASICDATION_H_INCLUDED
/**
\file

\brief Basic system device for the example basic dation

*/

#include "SystemDationB.h"
#include "Fixed.h"
#include "Signals.h"

#include <stdint.h>

namespace pearlrt {

   /**
   \file

   \brief Basic system device for an example basic dation

      This device provides a simple digital input and output
      of BIT(4) values.
      The input is always the last output value. The preset
      is '0000'B1.

   */

   class SampleBasicDation: public SystemDationB {

   private:
      int16_t echo;
      void internalDationOpen();
      void internalDationClose();

   public:
      /**
      constructor to create the bit group and set the
      bits to output direction

      \throws IllegalParamSignal in case of init failure

      */
      SampleBasicDation();

      /* destructor to remove the bit group
      */
      //~SampleBasicDation();

      /**
      Open the sample basic dation

      \param openParam open parameters if given
      \param idf pointer to IDF-value if given
      \returns pointer to the SampleDationB object itself as working
               object in the user dation

      \throws NotAllowedSignal, if  dation is not closed and rst is not given
      */
      SystemDationB* dationOpen(const char* idf = 0, int openParam = 0);

      /**
      Close the sample basic dation

      \param closeParam close parameters if given
      */
      void dationClose(int closeParam = 0);

      /**
      read  a Bit<width> value from the device

      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws IllegalParamSignal, if size != 1, since 1 byte is
                       expected for the Bit<4> value
      \throws NotAllowedSignal, if  dation is not opened
      */
      void dationRead(void * data, size_t size);

      /**
      send  a Bit<width> value to the device
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws IllegalParamSignal, if size != 1, since 1 byte is
                       expected for the Bit<4> value
      \throws NotAllowedSignal, if  dation is not opened
      */
      void dationWrite(void * data, size_t size);

      /**
      obtain the capabilities of the device

      This method returns :
            IN  OUT INOUT

      does not return:
            IDF  PRM CAN NEW  OLD ANY

      \returns available commands of the device
      */
      int capabilities();
   };
}
#endif

