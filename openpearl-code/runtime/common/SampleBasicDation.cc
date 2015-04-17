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


#include "Dation.h"
#include "SampleBasicDation.h"
#include "Log.h"
#include "Signals.h"
#include <iostream>

/**
 \brief Implementation of example basic  systemdation


*/
namespace pearlrt {

   SampleBasicDation::SampleBasicDation() {
      dationStatus = CLOSED;
   }

   SystemDationB* SampleBasicDation::dationOpen(const char * idf, int params) {
      if (idf) {
         Log::error("SampleBasicDation: no IDF allowed");
         throw theNotAllowedSignal;
      }
      if (params & ~(RST | IN | OUT | INOUT) ) {
         Log::error("SampleBasicDation: only RST allowed");
         throw theNotAllowedSignal;
      }
      if (dationStatus != CLOSED) {
         Log::error("SampleBasicDation: Dation already open");
         throw theNotAllowedSignal;
      }

      dationStatus = OPENED;
      return this;
   }

   void SampleBasicDation::dationClose(int params) {

      if (dationStatus != OPENED) {
         Log::error("SampleBasicDation: Dation not open");
         throw theNotAllowedSignal;
      }
      if (params & ~(RST | IN | OUT | INOUT) ) {
         Log::error("SampleBasicDation: only RST allowed");
         throw theNotAllowedSignal;
      }

      dationStatus = CLOSED;
   }

   void SampleBasicDation::dationWrite(void* data, size_t size) {
      //check size of parameter!
      // it is expected that a Fixed<15> object is passed
      // with a maximum of 16 bits. This fits into 2 byte.
      // Therefore size must be 2
      if (size != 2) {
         Log::error("SampleBasicDation: 2 byte expected (got %d)", (int)size);
         throw theIllegalParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("SampleBasicDation: Dation not open");
         throw theNotAllowedSignal;
      }

      // write data to application memory
      echo = *(int16_t*)data;
      std::cout << "SampleBasicDation::dationWrite: " << echo << std::endl;

   }

   void SampleBasicDation::dationRead(void* data, size_t size) {

      //check size of parameter!
      // it is expected that a Fixed<15> object is passed
      // with a maximum of 16 bits. This fits into 2 byte.
      // Therefore size must be 2
      if (size != 2) {
         Log::error("SampleBasicDation: 2 byte expected (got %d)", (int)size);
         throw theIllegalParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("SampleBasicDation: Dation not open");
         throw theNotAllowedSignal;
      }

      // write data to application memory
      std::cout << "SampleBasicDation::dationRead: " << echo << std::endl;
      *(int16_t*)data = echo;
   }

   int SampleBasicDation::capabilities() {
      int cap =  IN  | OUT | INOUT;
      return cap;
   }
}

