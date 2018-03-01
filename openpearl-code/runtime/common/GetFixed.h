#ifndef GETFIXED_H_INCLUDED
#define GETFIXED_H_INCLUDED
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

\brief input formating  for the data type FIXED


\author R. Mueller

*/
#include <stdio.h>

#include "Fixed.h"
#include "Source.h"
#include "Signals.h"
#include "GetHelper.h"


namespace pearlrt {
   /**
   \addtogroup io_common_helper
   @{
   */

   /**
      \brief Input processing of Fixed values.
   */
   template <int S>
   class GetFixed {
   public:
      /**
      read a FIXED value from the given data source.

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
      static void fromF(Fixed<S> &f,
                        const Fixed<31> w,
                        const Fixed<31> d,
                        Source & source) {
         Fixed63 value;

         if (w.x <= 0 || d.x < 0 || w.x < d.x) {
            throw theFixedFormatSignal;
         }

         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);
         // do all actions inside a helper method to reduce code size
         helper.readFixedByF(&value, d.x);
         f = Fixed<S>(value.get());
         return;
      }
   };
   /** @} */
}
#endif

