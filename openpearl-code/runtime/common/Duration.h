/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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

#ifndef DURATION_H_INCLUDED
#define DURATION_H_INCLUDED

#include "Fixed63.h"
#include "Float.h"
#include "BitString.h"

/**
\file

\brief operation for the data type DURATION


Realize the data type duration on top of a linux system.
Internally, a duration variable contains the number of micro seconds
since start
of a new day. Without special treatment of a time zone, GMT is used.

\author R. Mueller

*/

namespace pearlrt {

   class Duration;
}

#include "Clock.h"

namespace pearlrt {
   /**
   \file

   \brief PEARL data type DURATION
   */

   /**

     This class provides all required operations for the elements
     of the type DURATION.
     Different constructors are available for convenient usage
     by the compiler and the i/o system.
     In case an operation would lead to data range overflow,
     an exception will by thrown. This
     mechanism is provided by the class Fixed63.

     Duration values have a resolution of 1 micro second.
     The maximum value is defined by the
     limits of signed 64 bit integer values.

   \note The language refence makes no restrictions about the precission
   of a duration.

   \note Arithmetic with durations is done up to the full range of values.

   */
   class Duration {
   private:
      Fixed63 intval;

   public:
      /**
      create a DURATION value of 0 */
      Duration();

      /**
      create a DURATION from a double value secifying the seconds

      \param d the seconds, may by > 60

      \return the duration
      \throws DurationRangeSignal
      */

      Duration(const double d);

      /**
      retrieve the interval value

      \returns the internal value
      */
      Fixed63 get() const;

      /**
      retrieve full seconds of the given duration

      \returns full seconds of the given duration
      */
      int getSec() const;

      /**
      retrieve micro seconds part of the given duration


      \returns micro seconds part of the given duration
      */
      int getUsec(void) const;

      /**
      increment with a duration

      \param rhs the increment
      \returns reference to the added duration


      \throws DurationRangeSignal
      */
      Duration& operator+=(const Duration& rhs);

      /**
      add two durations

      \param rhs the increment
      \returns sum of the two durations as a new object

      \throws DurationRangeSignal
      */
      const Duration operator+(const Duration& rhs) const;

      /**
      reduce by a duration

      \param rhs the decrement
      \returns reference to the reduced duration


      \throws DurationRangeSignal
      */
      Duration& operator-=(const Duration& rhs);

      /**
      subtract two durations

      \param rhs the increment
      \returns difference between the two durations as a new object

      \throws DurationRangeSignal
      */
      const Duration operator-(const Duration& rhs) const;


      /**
      monadic minus

      \returns the inverted duration as a new object

      \throws DurationRangeSignal
      */
      const Duration operator-() const;


      /**
      multiply with Float

      \param rhs the multiplicator
      \returns reference to the multiplied duration

      \throws DurationRangeSignal
      */
      template <int S>Duration& operator*=(const Float<S>& rhs) {
         intval = (double)(intval.get()) * rhs.x;
         return *this;
      }

      /**
      multiply with Float

      \param rhs the multiplicator
      \returns new object containg the product

      \throws DurationRangeSignal
      */
      template<int S>const Duration  operator*(const Float<S>& rhs) const {
         return Duration(*this) *= rhs;
      }
      /**
      divide by Float

      \param rhs the denominator
      \returns reference to the divided duration

      \throws DurationRangeSignal
      */
      template <int S>Duration& operator/=(const Float<S>& rhs) {
         if ((rhs == Float<S>(0.0)).getBoolean()) {
            throw theDurationDivideByZeroSignal;
         }

         Float<S> help;

         try {
            help = Float<S>(intval.get()) / rhs;
         } catch (FloatIsINFSignal &s) {
            throw theDurationRangeSignal;
         }

         intval = Fixed63(help.x);
         return (*this);
      }

      /**
      divide by Float

      \param rhs the denominator
      \returns sum of the two durations as a new object

      \throws DurationRangeSignal
      */
      template <int S>Duration operator/(const Float<S>& rhs) {
         return Duration(*this) /= rhs;
      }
      /**
      divide by durations

      calculate the ratio between two durations

      \param rhs the denominator
      \returns the ratio as float

      \throws DurationDivideByZeroSignal
      */
      Float<24> operator/=(const Duration& rhs);

      /**
       divide two durations

       calculate the ratio between two durations

       \param rhs the denominator
       \returns the ratio as float

       \throws DurationDivideByZeroSignal
       */
      const Float<24> operator/(const Duration& rhs) const;

      /**
       compare operation

       easy comparision method

       \param rhs reference to the seconds parameter

       \returns >0, if this objects value > rhs value <br>
       =0, if both values are equal<br>
       <0, if this objects value < rhs value
       */
      int compare(const Duration& rhs) const;

      /**
       compare equal operation

       \param rhs reference to the seconds parameter

       \returns '1'B1, if both are equal<br>'0'B1 else
       */
      BitString<1> operator==(const Duration& rhs) const;


      /**
       compare not equal operation

       \param rhs reference to the seconds parameter

       \returns '1'B1, if both are not equal<br>'0'B1 else
       */
      BitString<1> operator!=(const Duration& rhs) const;


      /**
       compare greater than operation

       \param rhs reference to the seconds parameter

       \returns '1'B1, if this is farger than rhs<br>'0'B1 else
       */
      BitString<1> operator>(const Duration& rhs) const;


      /**
       compare less than  operation

       \param rhs reference to the seconds parameter

       \returns '1'B1, if this object is small than rhs<br>'0'B1 else
       */
      BitString<1> operator<(const Duration& rhs) const;


      /**
       compare greater or equal operation

       \param rhs reference to the seconds parameter

       \returns '1'B1, if this object is greater or equal as rhs <br>'0'B1 else
       */
      BitString<1> operator>=(const Duration& rhs) const;


      /**
       compare less or equal operation

       \param rhs reference to the seconds parameter

       \returns '1'B1, if this object is less or equal as rhs<br>'0'B1 else
       */
      BitString<1> operator<=(const Duration& rhs) const;

      /**
       abs operation

       \returns this object, if it is  >=0<br>-this, if this is < 0
       */
      Duration abs() const;

      /**
       sign operation

       \returns +1, if this object >0<br>0 if it is 0<br>-1, if it is < 0
       */
      Fixed<1> sign() const;


   };

   /**
   multiply with float*dur

   \param lhs the multiplicand
   \param rhs the multiplicator
   \returns new object containg the product

   \throws DurationRangeSignal
   */
   template<int S>const Duration  operator*(const Float<S>& lhs,
         const Duration &rhs) {
      Duration result(rhs);
      result *= lhs;
      return result;
   }
}

#endif
