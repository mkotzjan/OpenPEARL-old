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
#ifndef PCAN_H_INCLUDED
#define PCAN_H_INCLUDED
/**
\file

\brief CAN driver for PEAK Can adapters

*/

#include "SystemDationB.h"
#include "Fixed.h"
#include "Signals.h"
#include "Can2AMessage.h"

#include <stdint.h>
#include <libpcan.h>

namespace pearlrt {
   /**
   \addtogroup io_linux_driver
   @{
   */

   /**

   \brief system device for the PEAK CAN device adapters.

      All versions of the adapet are supported. The PEAK
      library and device nodes must be installed before 
      compilation and the search paths for include and lib
      must be updates.

      This device provides  native access to CAN 2A messages.
      The transfer data is defined in the Can2AMessage struct.

   */

   class PCan: public SystemDationB {

   private:
      HANDLE h;
      char * deviceNode;
      int bitRate;
      int openCount;
      void internalDationOpen();
      void internalDationClose();

   public:
      /**
      constructor  of the device

      \param devicePath is the path name of the corresponding linux device node
      \param speed the transmission speed, which may be one of 125000, 250000,
         500000 or 1000000

      \throws DationParamSignal in case of init failure

      */
      PCan(char* devicePath, int speed);

      /* destructor 
         cleanup of the device setting
      */
      //~PCan();

      /**
      Open the dation

      \param openParam open parameters if given
      \param idf pointer to IDF-value if given
      \returns pointer to the PCan object itself as working
               object in the user dation

      \throws dationParamSignal, if open parameters are specified 
        or if the bitRate is not possible
      \throws OpenFailedSignal, if  dation is not closed or the device
        may not be opened
      */
      SystemDationB* dationOpen(const char* idf = 0, int openParam = 0);

      /**
      Close the dation

      \param closeParam close parameters if given
      \throws CloseFailedSignal, if the dation is not open
      \throws DationParamSignal, if close parameters are specified
      */
      void dationClose(int closeParam = 0);

      /**
      read  a Can2AMessage struct from the device

      the method blocks teh calling thread until one message is available

      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws DationParamSignal, if size is not equal
                       to the length of the Can2AMessage 
      \throws DationNotOpenSignal, if  dation is not opened
      \throws ReadingFailedSignal in case of read errors
      */
      void dationRead(void * data, size_t size);

      /**
      send  a Can2AMessage to the device
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws DationParamSignal, if size is not equal
                       to the length of the Can2AMessage 
      \throws DationNotOpenSignal, if  dation is not opened
      \throws WritingFailedSignal in case of write errors
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
   /** @} */
}
#endif

