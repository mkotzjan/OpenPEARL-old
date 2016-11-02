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
for B1,B3 and B4-format.
*/

#ifndef PUTBITSTRING_H_INCLUDED
#define PUTBITSTRING_H_INCLUDED


#include "Sink.h"
#include "BitString.h"

using namespace std;
namespace pearlrt {

   template<int S> class PutBits;
   template<> class PutBits<1> {
      public:
         static void toBit(Bits<1>::BitType data, int len, int w, int base, Sink &sink);
   };
   template<> class PutBits<2> {
      public:
         static void toBit(Bits<2>::BitType data, int len, int w, int base, Sink &sink);
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
   template<int S> class PutBitString {
   private:
      /**
       no constructor needed, since the only static methods
       are provided. They need no objects.
      */
      PutBitString() {}

      /**
      number of bytes in the data storage
      */
      static const int len=NumberOfBytes<S>::resultBitString;

   public:

      /**
      convert the bit string into a character string in hex-format.

      The bit string is treated in bunches of 4 bit, starting at
      the leftmost position. If more hex digits are required
      than the bit string contains, they are appended with 0 as
      long as required.

      \param bitstring the data to be formatted
      \param w the width of the output field.
      \param sink the destination for the generated character sequence
      \throws BitFormatSignal, if w is <= 0
      */
      static void toB4(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {

         PutBits<len>::toBit(bitstring.x,S, w.x, 4, sink);

         return;
      }

      static void toB1(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {
         PutBits<len>::toBit(bitstring.x, S, w.x, 1, sink);

         return;
      }
      static void toB2(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {

         PutBits<len>::toBit(bitstring.x, S, w.x, 2, sink);

         return;
      }
      static void toB3(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {

         PutBits<len>::toBit(bitstring.x, S, w.x, 3, sink);

         return;
      }
   };

}
#endif

