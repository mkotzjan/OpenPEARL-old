/*
 [A "BSD license"]
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

\brief put-formatting of types FIXED(x)

\author R. Mueller

This module contains the output formatting of FIXED F(w,d,s)-format
*/

#ifndef PUTFIXED_H_INCLUDED
#define PUTFIXED_H_INCLUDED


#include <stdint.h>
#include <stdio.h>
#include "Fixed.h"
#include "Sink.h"
#include "compare.h"

using namespace std;
namespace pearlrt {
   /**
   \addtogroup io_common_helper
   @{
   */
 
   /**
   \brief Class containing the output formatting of fixed variables.

   \tparam S the length of the Fixed variable
   */
   template<int S> class PutFixed {
   private:
      /**
       no constructor needed, since the only static methods
       are provided. They need no objects.
      */
      PutFixed() {}
   public:

      /**
      convert the fixed value into a character string into
      a fixed point decimal-format.

      A stated in the language report, there are 3 formatting parameters:

      width denoting the total width of the output field<br>
      decimals denoting the dogits after the decimal point<br>
      The value will be set right adjusted into the output field.

      \todo move implementation into non template version

      \param fixedValue the data to be formatted
      \param w the width of the output field.
      \param d the number of post decimal point digits.
      \param sink the destination for the generated character sequence
      \throws FixedFormatSignal if
                format parameters are illegal<br>
       or rst value of FixedValueSignal
               if fixed value does not fit into format
      */
      static void toF(
         Fixed<S> fixedValue,
         Fixed<31> w,
         Fixed<31> d,
         Sink & sink) {
         Fixed<S> x, y, absValue; // internal workspace
         int32_t prePointDigits, postPointDigits;
         int32_t leadingSpaces, digits, ch;
         Fixed<31> digitsFixed;
         bool signNeeded;
         bool pointNeeded;

         // easy case: illegal parameter setting
         if (w.x <= 0) {
            Log::info("F: width < 0");
            throw theFixedFormatSignal;
         }

         if ((d < (Fixed<31>)0).getBoolean() || (d >= w).getBoolean()) {
            Log::info("F: width and decimals violation");
            throw theFixedFormatSignal;
         }

         // test whether number fits into format
         signNeeded = false;
         x = fixedValue;

         if ((x < (Fixed<S>)0).getBoolean()) {
            x = -x;
            signNeeded = true;
         }

         absValue = x;
         pointNeeded = false;

         if (d.x > 0) {
            pointNeeded = true;
         }

         // get number of digits in current value
         digits = 1;
         y = (Fixed<S>) 10;

         while ((x >= y).getBoolean()) {
            x = x / y;
            digits ++;
         }

         postPointDigits = d.x;
         prePointDigits = digits;

         leadingSpaces = w.x -
                         ((signNeeded ? 1 : 0) +
                          (pointNeeded ? 1 : 0) +
                          prePointDigits +
                          postPointDigits
                         );

         if (leadingSpaces < 0) {
            Log::info("F: width too small");
            throw theFixedValueSignal;
         }

         while (leadingSpaces > 0) {
            sink.putChar(' ');
            leadingSpaces --;
         }

         if (signNeeded) {
            sink.putChar('-');
         }

         digitsFixed = (Fixed<31>)(digits - 1);
         x = y.pow(digitsFixed); // y is still 10

         // now: x ist the base of the first digit
         do {
            if (x.x == 0) {
               ch = 0;
            } else {
               ch = (absValue / x).x;
            }
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;                  // select next digit
            prePointDigits --;
         } while (prePointDigits > 0);

         if (pointNeeded) {
            sink.putChar('.');
         }

         while (postPointDigits > 0) {
            if (x.x == 0) {
               ch = 0;
            } else {
               ch = (absValue / x).x;
            }
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;                  // select next digit
            postPointDigits --;
         }

         return;
      }
   };
   /** @} */
}
#endif

