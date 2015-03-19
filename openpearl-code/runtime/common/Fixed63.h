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

#ifndef FIXED63_INCLUDED
#define FIXED63_INCLUDED
// this code is based on the great work of
//   LeBlanc, David. Integer Handling with the C++ SafeInt Class.
//   http://msdn.microsoft.com/library/default.asp?
//        url=/library/en-us/dncode/html/secure01142004.asp
//   (2004).

#include <stdint.h>

namespace pearlrt {

   /**
   \file

   \brief safe FIXED(63) arithmetic  (for internal use)

   The type FIXED(63) is used to store clock and duration values.
   */

   /**
   Helper class for range sensitive 64 bit integer arithemtic

   This class implements integer operations, which throw exceptions if the
   result would be wrong due to the limited number of bits or other undefined
   operation like divide by zero.

   This code is based on the great work of
      LeBlanc, David.Integer Handling with the C++ SafeInt Class. <br>
      http://msdn.microsoft.com/library/default.asp?
          url=/library/en-us/dncode/html/secure01142004.asp
      (2004).

      In case an operation would leave the range of 64 bit,
      an exception will be thrown.
      <ul>
      <li>theArithemeticUnderflow
      <li>theArithmeticOverflow
      <li>theArithemeticDivideByZero
      </ul>

   */
   class Fixed63 {
   private:
      /**

      C datatype for 64 bit values
      */
      typedef uint64_t UFixed63_t;

   public:
      /**
        C++ type for Fixed(63) constants
        \returns is just the data type for the internal storage
      */
      typedef int64_t Fixed63_t;
   private:
      /**
      32 bit integer type

      for internal usage within the class
      */
      typedef int32_t __int32;

      /**
      unsigned 32 bit integer type

      for internal usage within the class
      */

      typedef uint32_t __uint32;
      void regMultiply(const UFixed63_t& a,
                       const UFixed63_t& b, UFixed63_t *ret) ;
      Fixed63_t x;
      static const Fixed63_t MinInt = 0x8000000000000000LL;
      static const Fixed63_t MaxInt = 0x7fffffffffffffffLL;
   public:
      Fixed63();
      Fixed63(Fixed63_t y);
      Fixed63(double y);
      Fixed63_t get() const;

      Fixed63 operator+=(const Fixed63& rhs);
      Fixed63 operator+(const Fixed63& rhs) const;

      Fixed63 operator-=(const Fixed63& rhs);
      Fixed63 operator-(const Fixed63& rhs) const;
      Fixed63 operator-() const;

      Fixed63 operator/=(const Fixed63& rhs);
      Fixed63 operator/(const Fixed63& rhs) const;

      Fixed63 operator%=(const Fixed63& rhs);
      Fixed63 operator%(const Fixed63& rhs) const;

      Fixed63 operator*=(const Fixed63& rhs);
      Fixed63 operator*(const Fixed63& rhs) const;

      int compare(const Fixed63& rhs) const;

//        Fixed63& operator*=(const double& rhs);
//        Fixed63& operator*(const double& rhs) const;

// * --> 1844 + 2270
// * und / mit double
   };
}
#endif
