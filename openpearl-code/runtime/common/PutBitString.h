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

using namespace std;
namespace pearlrt {

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
   pearlrt::PutBitString<3>::toB4(x, 1, console.getSink());
   \endverbatim

   */
   template<int S> class PutBitString {
   private:
      /**
       no constructor needed, since the only static methods
       are provided. They need no objects.
      */
      PutBitString() {}
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
         int w,
         Sink & sink) {
         if (w <= 0) {
            throw theBitFormatSignal;
         }

         typename BitString<S>::DataType h, m;
         int i;
         int shifts;
         char s;
// calculation of mask
// mask must 1111000....0000, according the value of S
// S=1..8: m.x = 0x0f0    0x0f << 4
// S=9..16: m.x=0x0f000   0x0f <<12
// S=17..31 m.x = 0x0f0000000 0x0f << 28
// --> shift by sizeof(*bitstring) * 8 - 4
         m = 0x0f;
         shifts = sizeof(bitstring.x) * 8 - 4;
         h = bitstring.x;
         m  <<= shifts;
// PEARL requires the FIRST 4 bits for the first hex-digit
// --> round shift
         shifts = (shifts / 4) * 4;

         for (i = 0; i < w; i++) {
            if (shifts >= 0) {
               s = ((h & m) >> shifts) + '0';

               if (s > '9') {
                  s += 'A' - '9' - 1;
               }

               sink.putChar(s);
               m >>= 4;
               shifts -= 4;
            } else {
               sink.putChar('0');
            }
         }

         return;
      }

      /**
      convert the bit string into a character string in hex-format.

      The bit string is treated in bunches of 4 bit, starting at
      the leftmost position. The length of the output field depends on the
      length of the bit string.

      \param bitstring the data to be formatted
      \param sink the destination for the generated character sequence
      \throws signal in case of trouble
      */
      static void toB4(
         BitString<S> &bitstring,
         Sink & sink) {
         int w = (S + 3) / 4;
         toB4(bitstring, w, sink);
      }

      /**
      convert the bit string into a character string in binary-format.

      The bit string is treated in single bits, starting at
      the leftmost position. If more digits are required
      than the bit string contains, they are appended with 0 as
      long as required.

      \param bitstring the data to be formatted
      \param w the width of the output field.
      \param sink the destination for the generated character sequence
      \throw BitFormatSignal, if w is <= 0
      */
      static void toB1(
         BitString<S> &bitstring,
         int w,
         Sink & sink) {
         if (w <= 0) {
            throw theBitFormatSignal;
         }

         typename BitString<S>::DataType h, m;
         int i;
         h = bitstring.x;
         m = 0x01;
         m <<= (sizeof(bitstring.x) * 8 - 1);

         for (i = 0; i < w; i++) {
            if (h) {
               sink.putChar('0' + !!(h & m));
               m >>= 1;
            } else {
               sink.putChar('0');
            }
         }

         return;
      }

      /**
      convert the bit string into a character string in binary-format.

      The bit string is treated in single bits, starting at
      the leftmost position.

      The output field width is according the bit string length.
      than the bit string contains, they are appended with 0 as
      long as required.

      \param bitstring the data to be formatted
      \param sink the destination for the generated character sequence
      \throws BitFormatSignal, if w is <= 0
      */
      static void toB1(
         BitString<S> &bitstring,
         Sink & sink) {
         toB1(bitstring, S, sink);
      }
   };

}
#endif

