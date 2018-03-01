#ifndef PUTCLOCK_H_INCLUDED
#define PUTCLOCK_H_INCLUDED
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

\brief output formating  for the data type CLOCK


\author R. Mueller

*/
#include "Clock.h"
#include "Sink.h"
#include "Fixed.h"


namespace pearlrt {
   /**
   \addtogroup io_common_helper
   @{
   */

   /**
      \brief Output formatting of clock values
   */
   class PutClock {
   public:
      /**
      create a string representing the given CLOCK value
      the format is as defined in the PEARL language reference

      Illegal values of w and d cause a reinterpretation to common values

      \param x the claock value which should be send to the output
      \param w the width of the output field
      \param d the decimals of the seconds; d must between 1 and 6
      \param sink the object, which collects the output data

      \throws ClockFormatSignal if parameters are illegal
      */
      static void toT(const Clock & x,
                      const Fixed<31>& w, const Fixed<31>& d,
                      Sink& sink) ;
   };
   /** @} */
}
#endif

