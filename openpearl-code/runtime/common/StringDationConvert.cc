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

/**
\file 
\brief implementation of  StringDationConvert

*/
#include "StringDationConvert.h"
#include "Signals.h"
#include "Log.h"
#include "Fixed.h"

namespace pearlrt {

   static const pearlrt::Fixed<31> one (1);

   StringDationConvert::StringDationConvert(RefCharacter * string,
                                            bool isOutput)
 : sink(*string), source(*string) {
      this->string = string;
      this->isOutput = isOutput;

      setupIOFormats(&sink, &source);

      if (isOutput) {
          // clear target string
          string->clear();
	  string->fill();
          string->clear();
          sink.pos(0);
      } else {
          source.rewind();
      }
   }

   void StringDationConvert::dationUnGetChar(const char c) {
      source.unGetChar(c);
   }

   void StringDationConvert::checkCapacity(Fixed<31> n) {

      if (string->getCurrent()+n.x >= string->getMax() ) {
          Log::error("attempt to read/write past the string limits");
          throw theCharacterTooLongSignal;
      }
   }

   void StringDationConvert::toX(Fixed<31> n) {
       int i;

       checkCapacity(n);
       if (n.x > 0) {
           for (i=0; i<n.x; i++) {
              sink.putChar(' ');
           }
       } else {
          adv(n);
       }
   }

   void StringDationConvert::fromX(Fixed<31> n) {
       adv(n);
   }

   void StringDationConvert::adv(Fixed<31> n) {
       checkCapacity(n);
       if (isOutput) {
          Fixed<31> pos(sink.sop());
          pos = pos + n; 
          sink.pos(pos.x);
       } else {
          Fixed<31> pos(source.sop());
          pos = pos + n; 
          source.pos(pos.x);
       }
   }

   void StringDationConvert::pos(Fixed<31> n) {
       try {
          n = n - one;   // adjust numbering: PRL uses 1..n; runtime 0..
          if (isOutput) {
             sink.pos(n.x);
          } else {
             source.pos(n.x);
          }
       } catch(CharacterTooLongSignal s) {
          throw theDationIndexBoundSignal;
       }
   }

   void StringDationConvert::sop(Fixed<31> &n) {
       if (isOutput) {
          Fixed<31> pos(sink.sop());
          n = pos;
       } else {
          Fixed<31> pos(source.sop());
          n = pos;
       }
       n = n + one; // adjust numbering; PRL uses 1..n, runtime: 0...
   }


}
