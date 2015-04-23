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
   Class containing the output formatting of fixed variables
   all methods are defined as static.


   Sample usage in PEARL:
   \verbatim
   DCL x FIXED(15) INIT(2);
   ...
   PUT x TO console BY F(3);
   \endverbatim

   Should compile in C++ to:
   \verbatim
   //DCL x FIXED(15) INIT(2);
   pearlrt:Fixed<15> x(2);
   ...
   //PUT x TO console BY F(3);
   // the console object should provide access to the data sink object
   // width=3, decimals=0, scale=0
   pearlrt::PutFixed<15>:toF(x, 3, 0, 0, console.getSink());
   \endverbatim
   Producing the output of '__3' (where _ denotes a space

   Required output behavior depending an width, decimals and
   scale at the example x=123:

   <table>
   <tr>
       <td>value</td><td>width</td><td>decimals</td><td>scale</td>
       <td>output</td>
   </tr>
   <tr><td>123</td><td>3</td><td> 0</td><td>0</td><td>"123"</td></tr>
   <tr><td> 123</td><td> 5 </td><td> 0 </td><td> 0  </td>
       <td> "__123"</td></tr>
   <tr><td> 123</td><td> 7 </td><td> 2 </td><td> 0  </td>
       <td> "_123.00"</td></tr>
   <tr><td> 1234</td><td> 7 </td><td> 2 </td><td> 0 </td>
       <td> "1234.00"</td></tr>
   <tr><td> 12345</td><td> 7 </td><td> 2 </td><td> 0 </td>
       <td> "*******" , throwing a signal</td></tr>
   <tr><td> -123</td><td> 7 </td><td> 2 </td><td>  0 </td>
      <td> "-123.00"</td></tr>
   <tr><td>  123</td><td> 7 </td><td> 2  </td><td>  1 </td>
      <td> "1230.00"</td></tr>
   <tr><td>   123</td><td> 7 </td><td> 2 </td><td> -1 </td>
      <td> "__12.30"</td></tr>
    <tr><td> 123  </td><td> 7 </td><td> -2 </td><td> -1 </td>
      <td> nothing, throwing a signal</td></tr>
   <tr><td> 123  </td><td> 2 </td><td> 2 </td><td> 0 </td>
      <td> "**" (in all cases of (w<d or d<0) ) </td></tr>
    </table>
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
      scale giving a 'virtual' number of decimals to enable the user
      to avoid floating point
             variables<br>
      The value will be set right adjusted into the output field.

      \note in difference to the language report, d>=w causes
            an error behavior of "***"-output

      \note in difference to the language report, a signal is throw
            if the current value does not fit into the format

      \param fixedValue the data to be formatted
      \param w the width of the output field.
      \param d the number of post decimal point digits.
      \param s the scale value
      \param sink the destination for the generated character sequence
      \throws FixedFormatSignal if
                format parameters are illegal<br>
       or rst value of FixedValueSignal
               if fixed value does not fit into format
      */
      static void toF(
         Fixed<S> fixedValue,
         Fixed<15> w,
         Fixed<15> d,
         Fixed<15> s,
         Sink & sink) {
         Fixed<S> x, y, absValue; // internal workspace
         int16_t prePointDigits, postPointDigits;
         int16_t leadingSpaces, i, digits, ch;
         Fixed<15> digitsFixed;
         bool signNeeded;
         bool pointNeeded;

         // easy case: illegal parameter setting
         if (w.x <= 0) {
            Log::info("F: width < 0");
            throw theFixedFormatSignal;
         }

         if ((d < (Fixed<15>)0).getBoolean() || (d >= w).getBoolean()) {
            for (i = 0; i < w.x; i++) {
               sink.putChar('*');
            }

            return;
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
         prePointDigits = digits + s.x;

         if (prePointDigits  < 1) {
            prePointDigits = 1;
         }

         leadingSpaces = w.x -
                         ((signNeeded ? 1 : 0) +
                          (pointNeeded ? 1 : 0) +
                          prePointDigits +
                          postPointDigits
                         );

         if (leadingSpaces < 0) {
            for (i = 0; i < w.x; i++) {
               sink.putChar('*');
            }

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

         digitsFixed = (Fixed<15>)(digits - 1);
         x = y.pow(digitsFixed); // y is still 10

         // now: x ist the base of the first digit
         do {
            ch = (absValue / x).x;
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;                  // select next digit
            prePointDigits --;
         } while (prePointDigits > 0);

         if (pointNeeded) {
            sink.putChar('.');
         }

         while (postPointDigits > 0) {
            ch = (absValue / x).x;
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;                  // select next digit
            postPointDigits --;
         }

         return;
      }
   };
}
#endif

