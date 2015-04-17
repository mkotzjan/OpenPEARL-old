/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014-2014 Rainer Mueller
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
\brief implementation of  PG-userdation

*/
#include "Dation.h"
#include "UserDation.h"
#include "DationPG.h"
#include "Signals.h"
#include "Log.h"
#include "DationDim.h"
#include "SystemDationNB.h"
#include "SystemDationNBSink.h"
#include "SystemDationNBSource.h"
#include "Fixed.h"
#include "PutHelper.h"
#include "PutClock.h"
#include "GetClock.h"
#include "PutDuration.h"

namespace pearlrt {

   DationPG::DationPG(SystemDationNB* parent,
                      int params,
                      DationDim * dimensions)
      : UserDationNB(parent, params, dimensions, UserDationNB::ALPHIC) {
      dationStatus = CLOSED;

      if (params & CYCLIC) {
         Log::error("DationPG: does not support CYCLIC");
         throw theIllegalParamSignal;
      }
   }

   void DationPG::dationRead(void * destination, size_t size) {
      char * d = (char*)destination;
      assertOpen();

      if (!(dationParams & (IN | INOUT))) {
         Log::error("DationPG: dation not opened as input");
         throw theNotAllowedSignal;
      }

      if (!(system->capabilities() & (IN | INOUT))) {
         Log::error("DationPG: device does not support read");
         throw theNotAllowedSignal;
      }

      // get first character from unget buffer
      *d = source.getChar();

      // read remaining from device
      if (size > 1) {
         work->dationRead(d + 1, size - 1);
      }
   }

   void DationPG::dationWrite(void * destination, size_t size) {
      char * d = (char*)destination;
      assertOpen();

      if (!(dationParams & (OUT | INOUT))) {
         Log::error("DationPG: Only writing is allowed");
         throw theNotAllowedSignal;
      }

      if (!(system->capabilities() & (OUT | INOUT))) {
         Log::error("DationPG: device does not support write");
         throw theNotAllowedSignal;
      }

      // send data to sink
      for (size_t i = 0; i < size; i++) {
         sink.putChar(*d);
         d++;
      }
   }

   void DationPG::dationSeek(const Fixed<31> & p, const int dationParam) {
      assertOpen();
      source.forgetUnGetChar();
      work->dationSeek(p, dationParam);
   }


   void DationPG::dationUnGetChar(const char c) {
      source.unGetChar(c);
   }

   void DationPG::internalOpen() {
      if (dationParams & (OUT | INOUT)) {
         sink.setSystemDationNB((SystemDationNB*)work);
      }

      if (dationParams & (IN | INOUT)) {
         source.setSystemDationNB((SystemDationNB*)work);
      }
   }

   void DationPG::internalClose() {
   }

   void DationPG::doPutChar(int length, RefCharacter * rc) {
      PutHelper::doPutChar(length, rc, &sink);
   }

   void DationPG::toT(const Clock f,
                      const Fixed<15> w,
                      const Fixed<15> d) {
      PutClock::toT(f, w, d, sink);
   }

   void DationPG::toD(const Duration& f,
                      const Fixed<15> w,
                      const Fixed<15> d) {
      PutDuration::toD(f, w, d, sink);
   }

   void DationPG::fromT(Clock & f,
                        const Fixed<15> w,
                        const Fixed<15> d) {
      GetClock::fromT(f, w, d, source);
   }

}
