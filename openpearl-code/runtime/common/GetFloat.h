#ifndef GETFLOAT_H_INCLUDED
#define GETFLOAT_H_INCLUDED
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

\brief input formating  for the data type FLOAT


\author R. Mueller

*/
#include <stdio.h>

#include "Fixed.h"
#include "Float.h"
#include "Source.h"
#include "Signals.h"
#include "GetHelper.h"


namespace pearlrt {
   /**
      Input processing of Float values.
   */
   template <int S>
   class GetFloat {
   public:
      /**
      read a FLOAT value in F-format from the given data source.

      The input field is delimited by the parameter width and
      additionally by EndOfLine.

      if w <= 0, an error is produced; no data assignment

      if d < 0, an error is produced; no data assignment

      if no decimal point is encoundered and d > 0, the last d digits
      are treated as post point digits.

      if a decimal point is encountered, the parameter d is obsolete.

      \param f the variable which gets the read value
      \param w the input field width
      \param d the decimal point position 
      \param source the input stream

      \throws FixedFormatSignal if w is <= 0 or d<0
      \throws FixedFormatValueSignal if the value does not fit
               into the variable

      */
      static void fromF(Float<S> &f,
                        const Fixed<31> w,
                        const Fixed<31> d,
                        Source & source) {
         Float<53> value;

         if (w.x <= 0 || d.x < 0 || w.x < d.x) {
            throw theFixedFormatSignal;
         }

         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);
         // do all actions inside a helper method to reduce code size
         helper.readFloatByF(&value, d.x);
         f = value.fit(f);
         return;
      }

      /**
      read a FLOAT value in E-format from the given data source.

      The input field is delimited by the parameter width and
      additionally by EndOfLine.

      if w <= 0, an error is produced; no data assignment

      if d < 0, an error is produced; no data assignment

      if s < 0, an error is produced; no data assignment

      Any valid float constant is accepted. There is no implicit 
      decimal point position as in the F-format!

      The parameters d and s are abosolte in input direction!

      \param f the variable which gets the read value
      \param w the input field width
      \param d the decimal point position 
      \param s significance: the number of digits
      \param source the input stream

      \throws ExpFormatSignal if w is <= 0 or d<0
      \throws ExpFormatValueSignal if the value does not fit
               into the variable

      */
      static void fromE(Float<S> &f,
                        const Fixed<31> w,
                        const Fixed<31> d,
			const Fixed<31> s,
                        Source & source) {
         Float<53> value;

         if (w.x <= 0 || d.x < 0 || s.x < 1) {
            throw theExpFormatSignal;
         }

         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);
         // do all actions inside a helper method to reduce code size
         helper.readFloatByE(&value);
         f = value.fit(f);
         return;
      }
   };
}
#endif

