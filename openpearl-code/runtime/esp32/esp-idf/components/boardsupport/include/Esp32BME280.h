/*
 [A "BSD license"]
 Copyright (c) 2015 Rainer Mueller
 Copyright (c) 2018 Michael Kotzjan
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
#ifndef ESP32BME280_H_INCLUDED
#define ESP32BME280_H_INCLUDED
/**
\file

\brief Basic system device for Lpc17xx GPIO Digital Input

*/

#include <stdint.h>
#include "SystemDationB.h"
#include "Fixed.h"
#include "Signals.h"


namespace pearlrt {

   /**
   \file

   \brief Basic system device for Lpc17xx GPIO Digital Input

      This device provides a simple digital input on the
      port bits

   */

   class Esp32BME280: public SystemDationB {

   private:

   public:
      /**
      constructor to create the bit group and set the
      bits to output direction

      \throws InternalDationSignal in case of init failure

      \param port is the port number (0..4)
      \param start is the starting bit number (31..0)
      \param width is the number of bits (1..32)
      */
      Esp32BME280();


      /**
      Open the DigitalIn

      \param openParam open parameters if given
      \param idf pointer to IDF-value if given
      \returns pointer to this object itself as working object in the 
               user dation
      \throws DationParamSignal, if  dation rst od IDF is not given
      \throws OpenFailedSignal, if  dation is not closed
      */
      SystemDationB* dationOpen(const char * idf, int openParam);

      /**
      Close the DigitalIn

      \param closeParam close parameters if given

      \throws CloseFailedSignal, if  dation is not opened
      \throws DationParamSignal, if close parameters are specified
      */
      void dationClose(int closeParam = 0);

      /**
      read  a Bit<width> value from the device

      This method will always throw an exception
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws InternalDationSignal, if size is smaller than the number
                       of bits of the inout device
      \throws DationNotOpenSignal, if  dation is not opened
      */
      void dationRead(void * data, size_t size);

      /**
      send  a Bit<width> value to the device
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws InternalDationSignal, if used at all
      */
      void dationWrite(void * data, size_t size);

      /**
      \returns available commands of the device
      */
      int capabilities();

   };
}
#endif

