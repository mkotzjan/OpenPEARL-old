/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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
#include "DationDim1.h"
#include "Fixed.h"
#include "Log.h"
#include "Signals.h"
#include "compare.h"

namespace pearlrt {

   DationDim1::DationDim1() : DationDim(0, 0, -1, 1, false) {
   }


   DationDim1::DationDim1(Fixed<31> c) : DationDim(0, 0, c, 1, true) {
      if ((c <= zero).getBoolean()) {
         Log::error("DationDim1: cols must be > 0");
         throw theInternalDationSignal;
      }
   }

   Fixed<31> DationDim1::getIndex() const {
      return (col);
   }

   void DationDim1::pos(const Fixed<31>c) {
      if (!(dationParams & Dation::DIRECT)) {
         Log::error("DationDim1: POS needs DIRECT DATION");
         throw theInternalDationSignal;
      }

      if ((c < zero).getBoolean()) {
         Log::error("DationDim1: POS < 0");
         throw theDationIndexBoundSignal;
      }

      if ((cols > zero).getBoolean() && (c >= cols).getBoolean()) {
         Log::error("DationDim1: POS beyond upper limit");
         throw theDationIndexBoundSignal;
      }

      col = c;
   }

   void DationDim1::adv(const Fixed<31> c) {

      if (dationParams & Dation::FORWARD) {
         if ((c < zero).getBoolean()) {
            Log::error("DationDim1: ADV on FORWARD DATION"
                       " needs non negativ value");
            throw thePositioningFailedSignal;
         }
      }

      if (dationParams & Dation::FORWARD && (c < zero).getBoolean()) {
         Log::error("negative direction at FORWARD");
         throw theInvalidPositioningSignal;
      }

      if (dationParams & Dation::CYCLIC) {
         col = col + c;
         col = col % cols;
      } else {
         if ((col < -c).getBoolean()) {
            Log::error("ADV beyond first element at NOCYCLIC");
            throw theDationIndexBoundSignal;
         }

         if ((cols > zero).getBoolean() && (col + c > cols).getBoolean()) {
            Log::error("ADV past last element at NOCYCLIC");
            throw theDationIndexBoundSignal;
         }

         col = col + c;
      }
   }

}
