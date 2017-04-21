/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

\brief put-formatting of types FLOAT(x)

\author R. Mueller

This module contains the output formatting of FIXED F(w,d,s)-format
*/

#ifndef PUTFLOAT_H_INCLUDED
#define PUTFLOAT_H_INCLUDED


#include <stdint.h>
#include <stdio.h>
#include "Float.h"
#include "Sink.h"
#include "compare.h"
#include "PutHelper.h"

using namespace std;
namespace pearlrt {
   /**
   \addtogroup io_common_helper
   @{
   */

   /**
   \brief Class containing the output formatting of float variables.

   \tparam S length of the Float type

   */
   template<int S> class PutFloat {
   private:
      /**
       no constructor needed, since the only static methods
       are provided. They need no objects.
      */
      PutFloat() {}
   public:

      /**
      convert the float value into a character string into
      a fixed point decimal-format.

      As stated in the language report, there are 2 formatting parameters:

      width denoting the total width of the output field<br>
      decimals denoting the dogits after the decimal point<br>

      The value will be set right adjusted into the output field.

      \param floatValue the data to be formatted
      \param w the width of the output field.
      \param d the number of post decimal point digits.
      \param sink the destination for the generated character sequence
      \throws FixedFormatSignal if
                format parameters are illegal<br>
       or rst value of FixedValueSignal
               if fixed value does not fit into format
      */
      static void toF(
         Float<S> floatValue,
         Fixed<31> w,
         Fixed<31> d,
         Sink & sink) {
         Float<S> x, y, absValue; // internal workspace
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
         x = floatValue;

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
         y = (Float<S>) 10;

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

      /**
      convert the float value into a character string into
      a exponential decimal-format.

      A stated in the language report, there are 3 formatting parameters:

      width denoting the total width of the output field<br>
      decimals denoting the digits after the decimal point<br>
      significance denoting the number of shown digits<br>

      The value will be set right adjusted into the output field.

      \todo move implementation into non template version

      \param floatValue the data to be formatted
      \param w the width of the output field.
      \param d the number of post decimal point digits.
      \param s the number of decimals shown.
      \param eSize the number of decimals in exponent (must be 2 or 3).
      \param sink the destination for the generated character sequence
      \throws ExpFormatSignal if format parameters are illegal<br>
       or  ExpValueSignal if value does not fit into format
      */
      static void toE(
         Float<S> floatValue,
         Fixed<31> w,
         Fixed<31> d,
         Fixed<31> s,
         Fixed<31> eSize,
         Sink & sink) {

         Float<53> x, y, testExponent; // internal workspace
         static const Float<53> one(1.0); // internal workspace
         static const Float<53> two(2.0); // internal workspace
         
         int32_t leadingSpaces;
         int32_t expValue; // value of the exponent field
         int32_t i,e; 
         int32_t sign=1; 
         int32_t prePointDigits, postPointDigits;
         int ch;


         // easy case: illegal parameter setting
         if (w.x <= 0) {
            Log::info("E: width < 0");
            throw theExpFormatSignal;
         }

         if ((s < d+(Fixed<31>)1).getBoolean() ) {
            Log::info("E: significance and decimals violation");
            throw theExpFormatSignal;
         }

         if ( eSize.x != 2 && eSize.x != 3) {
            Log::info("E: exponent field with illegal (%d)", eSize.x);
            throw theExpFormatSignal;
         }

         // exp field, decimal point and digits
         // w-s denote thenumber of digits in front of the decimal point
         // -3 due to decimal point and E and sign of exponent
         // eSize denotes the number of digits in the exponent
         leadingSpaces = w.x - s.x - 3 - eSize.x;

         x = floatValue;

         if ((x < (Float<53>)0).getBoolean()) {
            x = -x;
            sign = -1;
            leadingSpaces --;
         }

         if ((d < (Fixed<31>)0).getBoolean() || leadingSpaces < 0 ) {
            Log::info("E: width and decimals violation");
            throw theExpFormatSignal;
         }

         // determine the value of the 10th exponent
         expValue = 0;
         if ((x >= one).getBoolean()) {
            testExponent = GetHelper::pow10(100);
            if ((x >= testExponent).getBoolean()) {
               Log::info("E: number too large");
               throw theExpValueSignal;
            }
            e = 256;
            y = x;
            for (i=0; i<PutHelper::nbrBinExpValues; i++) {
               if ( (y >= PutHelper::binExpValues[i]).getBoolean()) {
                  y = y / PutHelper::binExpValues[i];
                  expValue += e;
               }
               e >>= 1;
            }    
         } else {
            e = 256;
            y = x;
            for (i=0; i<PutHelper::nbrBinExpValues; i++) {
               if ( (y * PutHelper::binExpValues[i] < two).getBoolean()) {
                  y = y * PutHelper::binExpValues[i];
                  expValue += e;
               }
               e >>= 1;
            }    
            expValue = -expValue;
         }

         // scale x to the desired exponent base according s and expValue
         if (expValue < -99) {
            x = (Float<53>)0.0;
            expValue = 0;
            prePointDigits = 1;
            postPointDigits = d.x;
            // result is zero --> modify output control variables
            sign=1;
            prePointDigits = 1;
            leadingSpaces = w.x - 6 - d.x; 
         } else {
            prePointDigits = (s.x - d.x);
            postPointDigits = d.x;
            expValue -= (s.x - d.x -1);
         }
         while(leadingSpaces > 0) {
            sink.putChar(' ');
            leadingSpaces --;
         }
         if (sign == -1) {
            sink.putChar('-');
         }


         x = x / Float<53>(GetHelper::pow10(expValue));
         y = GetHelper::pow10(prePointDigits-1);

         while (prePointDigits > 0) {
            ch = (x / y).x;
            sink.putChar(ch+'0');
            x = x - (Float<53>)ch * y;
            y = y / (Float<53>)10.0;
            prePointDigits --;
         }
         sink.putChar('.');

         y = (Float<53>)10.0;
         while (postPointDigits > 0) {
            x  = (x * y);
            ch = x.x;
            sink.putChar(ch+'0');
            x = x - (Float<53>)ch;
            postPointDigits --;
         }
         sink.putChar('E');
         if (expValue>=0) {
            sink.putChar('+');
         } else {
            sink.putChar('-');
            expValue = - expValue;
         }

         if (eSize.x == 3) { 
            // three digit exponent
            ch = expValue / 100 + '0';
            sink.putChar(ch);
            expValue /= 10;
         }

         ch = expValue / 10 + '0';
         sink.putChar(ch);
         ch = expValue % 10 + '0';
         sink.putChar(ch);
      }
   };
   /** @} */
}
#endif

