#ifndef GETCLOCK_H_INCLUDED
#define GETCLOCK_H_INCLUDED
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
#include <stdio.h>

#include "Fixed.h"
#include "Clock.h"
#include "Source.h"

/**
\file

\brief input formating  for the data type CLOCK


\author R. Mueller

*/
#include "Clock.h"
#include "GetClock.h"
#include "Signals.h"


namespace pearlrt {
   /** 
   \addtogroup io_common_helper
   @{
   */

   /**
      \brief Input processing of clock values.
   */
   class GetClock {
   public:
      /**
      create a CLOCK value from the given data source.
      If illegal characters are encountered, the function conplains
      with a signal and exits.
      Leading spaces and trailing spaces are discarded.

      The clock format looks like:
         [digit]digit:digit digit:digit:digit digit[. digit +]

      Example:
          w=25 - leading and trailing spaces are omitted
          '  1:23:12.1234567890   '

      If d > 0 and no decimal point is given, a virtual decimal point is
      inserted before the last d digits of the seconds part - which must
      be long enough. If a decimal point is present, the parameter d is
      obsolete.

      The input field is delimited by the parameter width and
      additionally by EndOfLine.

      \param c the variable which gets the read value
      \param w the input field width
      \param d the number of decimals of the seconds part may be 0)
      \param source the input stream

      \returns rst value of ClockFormatSignal if w is <= 0 or d <0
      \returns rst value of ClockValueSignal if the input does
                 not match the pattern

      */
      static int fromT(Clock&c,
                       const Fixed<31> w,
                       const Fixed<31> d,
                       Source & source);
   };
   /** @} */
}
#endif

