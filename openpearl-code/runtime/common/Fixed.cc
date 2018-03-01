/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

#include "Fixed.h"
#include "Log.h"
#include "Signals.h"

namespace pearlrt {
   void assignIntToFixedViaVoidPointer(
           void* voidPointerToFixed,
           size_t lengthOfFixed, int valueToAssign) {
      // test if the valueToAssign is larger than the Fixed-type may
      // store
      // Example with FIXED(3)
      //    minimum value: 1...11111000  (-8)
      //    maximum value: 0...00000111  (+7)
      //    mask:          1...11111000
      //    value AND mask:must be ether 0 or mask to fit in the data type
      unsigned int mask = (unsigned int) - 1; // creates all bits as 1
      unsigned int maskedValue;
      unsigned int unsignedValue = *(unsigned int*)&valueToAssign;

      mask <<= lengthOfFixed;

      maskedValue = unsignedValue & mask;

      if (maskedValue == 0 || maskedValue == mask) {
         if (lengthOfFixed <= 7) {
            *(int8_t*)voidPointerToFixed = valueToAssign;
         } else if (lengthOfFixed <= 15) {
            *(int16_t*)voidPointerToFixed = valueToAssign;
         } else if (lengthOfFixed <= 31) {
            *(int32_t*)voidPointerToFixed = valueToAssign;
         } else if (lengthOfFixed <= 63) {
            *(int64_t*)voidPointerToFixed = valueToAssign;
         } else {
            Log::error("illegel length for int to Fixed Assign (%zu)",
                       lengthOfFixed);
            throw theInternalDationSignal;
         }
      } else {
         Log::error("overflow in int to fixed assign");
         throw theFixedRangeSignal;
      }
   }
}

