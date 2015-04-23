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

/**
\file

\brief operation for the data type DURATION

\author R. Mueller

*/

#include "Duration.h"
#include "Signals.h"

using namespace std;

namespace pearlrt {

   Fixed63 Duration::get() const {
      return intval;
   }

   Duration::Duration() {
      intval = (Fixed63::Fixed63_t)0;
   }


   int Duration::getSec() const {
      int sec;
      sec =  intval.get() / 1000000LL;
      return (sec);
   }


   int Duration::getUsec() const {
      int usec;
      usec =  intval.get() % 1000000LL;
      return (usec);
   }


   Duration::Duration(const double d) {
      try {
         if (d > 0) {
            intval = Fixed63(d * 1000000.0 + 0.5);
         } else {
            intval = Fixed63(d * 1000000.0 - 0.5);
         }
      } catch (ArithmeticOverflowSignal &s) {
         throw theDurationRangeSignal;
      }
   }

   Duration& Duration::operator+=(const Duration& rhs) {
      try {
         intval += rhs.intval;
      } catch (ArithmeticOverflowSignal &s) {
         throw theDurationRangeSignal;
      } catch (ArithmeticUnderflowSignal &s) {
         throw theDurationRangeSignal;
      }

      return (*this);
   }

   const Duration Duration::operator+(const Duration& rhs) const {
      return Duration(*this) += rhs;
   }

   Duration& Duration::operator-=(const Duration& rhs) {
      try {
         intval -= rhs.intval;
      } catch (ArithmeticOverflowSignal &s) {
         throw theDurationRangeSignal;
      } catch (ArithmeticUnderflowSignal &s) {
         throw theDurationRangeSignal;
      }

      return (*this);
   }


   const Duration Duration::operator-(const Duration& rhs) const {
      return Duration(*this) -= rhs;
   }

   const Duration Duration::operator-() const {
      return Duration(Duration(0) - (*this));
   }


   Float<24> Duration::operator/=(const Duration& rhs) {

      if (rhs.intval.get() == 0) {
         throw theDurationDivideByZeroSignal;
      }

      Float<24> result(intval.get());
      result = result / Float<24>(rhs.intval.get());
      return result;
   }

   const Float<24> Duration::operator/(const Duration& rhs) const {
      return Duration(*this) /= rhs;
   }

   int Duration::compare(const Duration& rhs) const {
      return intval.compare(rhs.intval);
   }

   BitString<1> Duration::operator==(const Duration& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) == 0);
   }

   BitString<1> Duration::operator!=(const Duration& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) != 0);
   }

   BitString<1> Duration::operator<(const Duration& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) < 0);
   }

   BitString<1> Duration::operator>(const Duration& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) > 0);
   }

   BitString<1> Duration::operator<=(const Duration& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) <= 0);
   }

   BitString<1> Duration::operator>=(const Duration& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) >= 0);
   }

   Duration Duration::abs() const {
      if (intval.compare(Fixed63::Fixed63_t(0)) < 0) {
         return -(*this);
      } else {
         return (*this);
      }
   }

   Fixed<1> Duration::sign() const {
      int ret = intval.compare(Fixed63::Fixed63_t(0));

      if (ret < 0) {
         return Fixed<1>(-1);
      } else if (ret == 0) {
         return Fixed<1>(0);
      } else {
         return Fixed<1>(1);
      }
   }

}

