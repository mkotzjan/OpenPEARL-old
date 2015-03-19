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

#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

/**
\file

\brief operation for the data type CLOCK

Realize the data type CLOCK on top of a linux system.
The clock is based on the gettimeofday()-service of the linux system.
Internally, a clock variable contains the number of micro seconds since start
of a new day. Without special treatment of a time zone, GMT is used.


\author R. Mueller

*/


#include <string>
#include "Fixed63.h"
#include "BitString.h"

namespace pearlrt {
   class Clock;
}

#include "Duration.h"
namespace pearlrt {
   /**
   \file

   \brief PEARL data type CLOCK

   */

   /**
   This class provides all required operations for the elements of
   the type CLOCK.

   Clock values have a resolution of 1 micro second.
   The maximum value is defined by the limits of signed 64 bit integer values.

   The class uses no static variables.
   Therefore the all methods a threadsafe without need of mutexes.

   \note The language reference tells that clock values are treated
   modulo 24 HRS at clock constants.

   \note There is no specification about the required precission
   */
   class Clock {
   private:
      Fixed63 intval;  //< the internal value

      /** adjust intermediate values to 24h		*/
      void adjust();
   public:
      /**
      return the current time of the day

      \returns the current clock value
      */
      static Clock now(void);

      /**
       construct with 0 (midnight)
       */
      Clock();

      /**
      construct with the given number of seconds since midnight

      \param x number of seconds (with decimals)
      */
      Clock(double x);

      /**
      return full seconds part of the clock value

      \returns full seconds
      */
      int getSec(void) const;

      /**
      return micro seconds part of the clock value

      \returns micro seconds
      */
      int getUsec(void) const;

      /**
      retrieve the internal value

      the clock value is just the number of micro seconds since
      start of the day

      \returns internal value
      */
      const Fixed63& get(void) const;

      /**
      increment the current clock by the given duration value

      The result is remapped to the same day.

      \param d the right hand side parameter
      \returns reference to the updated clock duration
      */
      Clock& operator+(const Duration& d);

      /**
      decrement the current clock by the given duration value

      The result is remapped to the same day.

      \param d the right hand side parameter
      \returns reference to the updated clock duration
      */
      Clock& operator-(const Duration& d);

      /**
      Calculate the difference time between the internal and the given
      clock values.

      \param c the right hand side parameter
      \returns duration between the two time values
      */
      Duration operator-(const Clock& c);

      /**
       compare operation

       in order to keep the class small, only one compare
       operation is available

       \param rhs reference to the seconds parameter
       \returns >0, if this objects value > rhs value <br>
       =0, if both values are equal<br>
       <0, if this objects value < rhs value

       */
      int compare(const Clock& rhs) const;

      /**
        compare less

       \param rhs reference to the seconds parameter
       \returns '1'B1, if this objects value < rhs value <br>'0'B1, else
       */
      BitString<1> operator<(const Clock& rhs) const;

      /**
        compare less or equal

       \param rhs reference to the seconds parameter
       \returns '1'B1, if this objects value <= rhs value <br>'0'B1, else
       */
      BitString<1> operator<=(const Clock& rhs) const;

      /**
        compare equal

       \param rhs reference to the seconds parameter
       \returns '1'B1, if this objects value == rhs value <br>'0'B1, else
       */
      BitString<1> operator==(const Clock& rhs) const;

      /**
       compare not equal

      \param rhs reference to the seconds parameter
      \returns '1'B1, if this objects value != rhs value <br>'0'B1, else
      */
      BitString<1> operator!=(const Clock& rhs) const;

      /**
       compare greater or equal

      \param rhs reference to the seconds parameter
      \returns '1'B1, if this objects value < rhs value <br>'0'B1, else
      */
      BitString<1> operator>=(const Clock& rhs) const;

      /**
       compare greater

      \param rhs reference to the seconds parameter
      \returns '1'B1, if this objects value > rhs value <br>'0'B1, else
      */
      BitString<1> operator>(const Clock& rhs) const;
   };
}
#endif

