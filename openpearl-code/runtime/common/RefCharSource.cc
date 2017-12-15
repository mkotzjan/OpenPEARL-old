/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Rainer Mueller
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


#include "RefChar.h"
#include "RefCharSource.h"
#include "Signals.h"

namespace pearlrt {
   RefCharSource::RefCharSource(RefCharacter & s) {
      sourceObj =  & s;
     sourceObj->setCurrent(0);
      //readPtr = 0;
   }

   char RefCharSource::realGetChar(void) {
      char c;

      try {
         c = sourceObj->getNextChar();
//         readPtr ++;
      } catch (CharacterIndexOutOfRangeSignal & e) {
         throw theNoMoreCharactersSignal;
      } 

      return (c);
   }


   void RefCharSource::pos(size_t pos) {
      //readPtr = pos;
      sourceObj->setCurrent(pos);
      forgetUnGetChar();
   }

   void RefCharSource::rewind() {
 //     readPtr = 0;
      sourceObj->setCurrent(0);
      forgetUnGetChar();
   }

   size_t RefCharSource::sop() {
      return sourceObj->getCurrent();
   }

}

