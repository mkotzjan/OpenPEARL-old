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
#ifndef RPIDIGITALIN_H_INCLUDED
#define RPIDIGITALIN_H_INCLUDED
/**
\file

\brief Basic system device for raspberry pi gpio Digital Input

*/

#include "SystemDationB.h"
#include "Fixed.h"
#include "Character.h"
#include "Signals.h"
#include "Log.h"


namespace pearlrt {

   /**
   \file

   \brief Basic system device for Raspberry Pi Digital Input

   */

   class RPiDigitalIn: public SystemDationB {

   private:
      const int start, width;

   public:
      /**
      constructor to create the bit group and set the
      bits to input direction

      \note throws exception in case of init failure

      \param start bit number for the leftmost bit  (7..0)
      \param width number of bits (1..8)
      \param pud pull-up pull-down indicator
      */
      RPiDigitalIn(int start, int width, char * pud);

      /**
      destructor of the bit group

      This is supplied only for testing purpose.
      PEARL applications have static system dations, the dtor is called
      at systems end.
      */
      ~RPiDigitalIn();

      /**
      Open the DigitalIn
      \param openParam open parameters if given
      \param idf pointer to IDF-value if given
      \throws DationParamSignal, if  open  parameters were specified
      \throws OpenFailedSignal, if  dation is not closed and rst is not given
      */
      SystemDationB* dationOpen(const char * idf = 0, int openParam = 0);

      /**
      Close the DigitalOut

      \param closeParam close parameters if given


      \throws DationParamSignal, if  close  parameters were specified
      \throws DationNotOpenSignal, if  dation is not opened
      */
      void dationClose(int closeParam = 0);

      /**
      write  a Bit<width> value to the device

      This method will always throw an exception
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws InternalDationSignal, if used at all
      */
      void dationWrite(void * data, size_t size);

      /**
      read  a Bit<width> value from the device
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws DationParamSignal, if size > 4
      \throws DationNotOpenSignal, if  dation is not opened

      */
      void dationRead(void * data, size_t size);

      /**
      obtain the capabilities of the device

      This method returns :
            IN ANY

      \returns available commands of the device
      */
      int capabilities();

   };


}
#endif
