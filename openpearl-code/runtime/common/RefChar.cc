/*
 [A BSD license"]
 Copyright (c) 2012-2017 Rainer Mueller
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
\file

\brief Data type REF CHARACTER

This class provides the operations of the data type ref char.

*/

#include "RefChar.h"

namespace pearlrt {
   RefCharacter::RefCharacter() {
      max = 0;
      current = 0;
      data = 0;
   }

   void RefCharacter::clear() {
      current = 0;
   }

   char RefCharacter::getCharAt(size_t index) {
      // size-t is unsigned; max -1 become very large if max = 0
      // if index was given wrong (negative) the other clause  works
      if (index > max - 1 || index + 1 > max) {
         throw theCharacterIndexOutOfRangeSignal;
      }

      return data[index];
   }


   char * RefCharacter::getCstring() {
      if (current + 1 > max) {
         throw theCharacterTooLongSignal;
      }

      data[current] = '\0';
      return data;
   }

   void RefCharacter::fill() {
      for (size_t i = current; i < max; i++) {
         data[i] = ' ';
      }
   }

   void RefCharacter::add(const RefCharacter& rhs) {
      size_t newCurrent = current + rhs.current;

      if (newCurrent > max) {
         throw theCharacterTooLongSignal;
      }

      size_t j = 0;

      for (size_t i = current; i < newCurrent; i++) {
         data[i] = rhs.data[j++];
      }

      current = newCurrent;
   }

   void RefCharacter::add(const char rhs) {
      if (current + 1 > max) {
         throw theCharacterTooLongSignal;
      }

      data[current++] = rhs;
   }

   void RefCharacter::add(const char * rhs) {
      while (*rhs) {
         add(*rhs);
         rhs++;
      }
   }

   size_t RefCharacter::getMax() {
      return max;
   }

   size_t RefCharacter::getCurrent() {
      return current;
   }

   void RefCharacter::setCurrent(size_t newCurrent) {
      if (newCurrent >= max) {
         throw theCharacterTooLongSignal;
      }
      current = newCurrent;
      return;
   }
}
