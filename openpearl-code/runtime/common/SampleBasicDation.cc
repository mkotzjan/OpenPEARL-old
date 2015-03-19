/*
 [The "BSD license"]
 Copyright (c) 2015      Rainer Mueller
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


#include "SampleBasicDation.h"
#include "Log.h"
#include "Signals.h"
/**
 \brief Implementation of example basic  systemdation


*/
namespace pearlrt {

   SampleBasicDation::SampleBasicDation() {
      dationStatus = CLOSED;
   }

   void SampleBasicDation::internalDationOpen() {
      if (dationStatus != CLOSED) {
         Log::error("SampleBasicDation: Dation already open");
         throw theNotAllowedSignal;
      }

      dationStatus = OPENED;
   }
   void SampleBasicDation::internalDationClose() {

      if (dationStatus != OPENED) {
         Log::error("SampleBasicDation: Dation not open");
         throw theNotAllowedSignal;
      }

      dationStatus = CLOSED;
   }

   void SampleBasicDation::dationWrite(void* data, size_t size) {
      //check size of parameter!
      // it is expected that a BitString<width> object is passed
      // with a maximum of 8 bits. This fits into 1 byte.
      // Therefore size must be 1
      if (size != 1) {
         Log::error("SampleBasicDation: 1 byte expected");
         throw theIllegalParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("SampleBasicDation: Dation not open");
         throw theNotAllowedSignal;
      }

      // write data to application memory
      echo = *(char*)data;
   }

   void SampleBasicDation::dationRead(void* data, size_t size) {

      //check size of parameter!
      // it is expected that a BitString<width> object is passed
      // with a maximum of 8 bits. This fits into 1 byte.
      // Therefore size must be 1
      if (size != 1) {
         Log::error("SampleBasicDation: 1 byte expected");
         throw theIllegalParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("SampleBasicDation: Dation not open");
         throw theNotAllowedSignal;
      }

      // write data to application memory
      *(char*)data = echo;
   }

}

