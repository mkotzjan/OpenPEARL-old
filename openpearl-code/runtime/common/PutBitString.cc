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

/**
\file

\brief put-formatting of types BIT(x)

\author R. Mueller

This module contains the output formatting von BitStrings
for B1,B2,B3 and B4-format.
*/

#include <stdint.h>

#include "Sink.h"
#include "BitString.h"
#include "Signals.h"
#include "PutBitString.h"

using namespace std;
namespace pearlrt {
#ifdef DELETED
   template<int S> class PutBits;
   template<> class PutBits<1> {
      public:
         static void toBit(Bits<1>::BitType data, int len, int w, int base, Sink &sink);
   };
   template<> class PutBits<2> {
      public:
         static void toBit(Bits<4>::BitType data, int len, int w, int base, Sink &sink);
   };

   template<> class PutBits<4> {
      public:
         static void toBit(Bits<4>::BitType data, int len, int w, int base, Sink &sink);
   };

   template<> class PutBits<8> {
      public:
         static void toBit(Bits<8>::BitType data, int len, int w, int base, Sink &sink);
   };

   /**
   Class containing the output formatting of bit string variables
   all methods are defined as static.


   Sample usage in PEARL:
   \verbatim
   DCL x BIT(3) INIT('011'B1);
   ...
   PUT x TO console BY B4(1);
   \endverbatim

   Should compile in C++ to:
   \verbatim
   //DCL x BIT(3) INIT('011'B1);
   pearlrt:BitString<3> x(3);
   ...
   //PUT x TO console BY B4(1);
   // the console object should provide access to the data sink object
   pearlrt::PutBitString<3>::toB4(x, (Fixed<31>)(w), console.getSink());
   \endverbatim

   */
#endif
   static const int dataMask[] = {0x01, 0x03, 0x07, 0x0f};
   static const char hexChar[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

   void PutBits<1>::toBit(BitString<8>::DataType bits, int len, int width,
                          int base, Sink & sink) {

        BitString<32>::DataType b;
        b = bits;
        b <<= 24; 
//printf("as Bit(32): %x len=%d, width=%d base=%d\n", b, len, width, base);
        PutBits<4>::toBit(b, len+24, width, base, sink);
#ifdef DELETD
      int mask;
      int digits;
      int shiftSize;

      if (width <= 0) {
         Log::error("toBit: illegal width (%d)", width);
         throw theBitFormatSignal;
      }

      digits = (len+base-1)/base;
      shiftSize = (digits-1)*base+(8-len)/base;
      mask = dataMask[base-1] << shiftSize;
     
     for (int i=0; i<width; i++) {
        if (digits > 0) {
           int ch = (bits&mask)>>(shiftSize);  
//           printf("ch = %x\n", ch);
           bits <<= base;
           digits --;
           sink.putChar(hexChar[ch]); 
        } else {
           sink.putChar('0'); 
        }
     }
#endif
   }

   void PutBits<2>::toBit(BitString<16>::DataType bits, int len, int width,
                          int base, Sink & sink) {
        BitString<32>::DataType b;
        b = bits;
        b <<= 16; 
        PutBits<4>::toBit(b, len+16, width, base, sink);
   }

   void PutBits<4>::toBit(BitString<32>::DataType bits, int len, int width,
                          int base, Sink & sink) {

      uint32_t mask;
      int digits;
      int shiftSize;

      if (width <= 0) {
         Log::error("toBit: illegal width (%d)", width);
         throw theBitFormatSignal;
      }

      digits = (len+base-1)/base;
      shiftSize = (digits-1)*base+(32-len)/base;
      mask = dataMask[base-1] << shiftSize;
     
     for (int i=0; i<width; i++) {
        if (digits > 0) {
           int ch = (bits&mask)>>(shiftSize);  
//           printf("ch = %x\n", ch);
           bits <<= base;
           digits --;
           sink.putChar(hexChar[ch]); 
        } else {
           sink.putChar('0'); 
        }
     }
   }


   void PutBits<8>::toBit(BitString<64>::DataType bits, int len, int width,
                          int base, Sink & sink) {

      uint64_t mask;
      int digits;
      int shiftSize;

      if (width <= 0) {
         Log::error("toBit: illegal width (%d)", width);
         throw theBitFormatSignal;
      }

      digits = (len+base-1)/base;
      shiftSize = (digits-1)*base+(64-len)/base;
      mask = dataMask[base-1] << shiftSize;
     
     for (int i=0; i<width; i++) {
        if (digits > 0) {
           int ch = (bits&mask)>>(shiftSize);  
//           printf("ch = %x\n", ch);
           bits <<= base;
           digits --;
           sink.putChar(hexChar[ch]); 
        } else {
           sink.putChar('0'); 
        }
     }
   }
}

