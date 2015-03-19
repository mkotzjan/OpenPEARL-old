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


\brief safe FIXED(63) operations   (for internal use)

*/
// this code is based on the great work of
//   LeBlanc, David. Integer Handling with the C++ SafeInt Class.
//   http://msdn.microsoft.com/library/default.asp?
//        url=/library/en-us/dncode/html/secure01142004.asp
//   (2004).

#include "Fixed63.h"
#include "Signals.h"

namespace pearlrt {


   /**
   construct to zero
   */
   Fixed63::Fixed63() {
      x = 0;
   }

   /** construct with a given value

   \param y the value of type long long (e.g. 27LL)
   */
   Fixed63::Fixed63(Fixed63_t y) {
      x = y;
   }

   /**
   construct with a given double value

   be warned that the precission of a double value is less than the number
   of decimals of a 64 bit integer. There will be a loss of information
   when converting from Fixed63 -> double -> Fixed63

   \param y the value

   \throws ArithmeticUnderflowSignal ArithmeticOverflowSignal

   */

   Fixed63::Fixed63(double y) {
      if (y < MinInt) {
         throw theArithmeticUnderflowSignal;
      } else if (y > MaxInt) {
         throw theArithmeticOverflowSignal;
      }

      x = y;

      if (x < 0 && y > 0) {
         throw theArithmeticOverflowSignal;
      }

      if (x > 0 && y < 0) {
         throw theArithmeticUnderflowSignal;
      }
   }


   /**
   return the internal value as Fixed64_t (long long)

   \returns the value
   */
   Fixed63::Fixed63_t Fixed63::get() const {
      return x;
   }

   /**
   increment the internal value with the given parameter

   \param rhs the value to be added

   \return reference to this object containing the result

   \throws  ArithmeticUnderflowSignal ArithmeticOverflowSignal

   */
   Fixed63 Fixed63::operator+=(const Fixed63& rhs) {
      if (!((x ^ rhs.x) < 0)) {                    //test for +/- combo
         //either two negatives or two positives
         if (rhs.x < 0) {
            //two negatives
            if (x < MinInt - rhs.x) {              //remember rhs < 0
               throw theArithmeticUnderflowSignal;
            }

            //ok
         } else {
            //two positives
            if (MaxInt - x < rhs.x) {
               throw theArithmeticOverflowSignal;
            }

            //OK
         }
      }

      //else overflow not possible
      x += rhs.x;
      return *this;
   }

   /**
   add two values as in a + b

   \param rhs the value to be added to the current objects value

   \returns a new object containing the result
   \throws  ArithmeticUnderflowSignal ArithmeticOverflowSignal
   */
   Fixed63 Fixed63::operator+(const Fixed63& rhs) const {
      return Fixed63(*this) += rhs;
   }

   /**
   reduce the internal value with the given parameter

   \param rhs the value to be subtracted

   \return reference to this object containing the result

   \throws  ArithmeticUnderflowSignal ArithmeticOverflowSignal

   */
   Fixed63 Fixed63::operator-=(const Fixed63& rhs) {
      // Note: lhs is *this
      // we have essentially 4 cases:
      //
      // 1) lhs positive, rhs positive - overflow not possible
      // 2) lhs positive, rhs negative - equivalent to addition
      //                                 - result >= lhs or error
      // 3) lhs negative, rhs positive - check result <= lhs
      // 4) lhs negative, rhs negative - overflow not possible
      Fixed63_t tmp = (Fixed63_t)(
                         (unsigned Fixed63_t)x - (unsigned Fixed63_t)rhs.x);

      // Note - ideally, we can order these so that true conditionals
      // lead to success, which enables better pipelining
      // It isn't practical here
      if (x >= 0 && rhs.x < 0 && tmp < x) {   // condition 2
         throw theArithmeticOverflowSignal;
      }

      if (rhs.x >= 0 && tmp > x) {             // condition 3
         throw theArithmeticUnderflowSignal;
      }

      x = tmp;
      return *this;
   }


   /**
   subtract two values as in a - b

   \param rhs reference to the value to be subtracted from
          the current objects value
   \returns a new object containing the result
   \throws  ArithmeticUnderflowSignal ArithmeticOverflowSignal
   */
   Fixed63 Fixed63::operator-(const Fixed63& rhs) const {
      return Fixed63(*this) -= rhs;
   }

   /**
   arithmetic invertion of the value	 just like  -a

   \return a new object containing the negative value of this objekt

   \throws ArithmeticOverflowSignal
   */
   Fixed63 Fixed63::operator-() const {
      if (x == MinInt) {
         throw theArithmeticOverflowSignal;
      }

      return Fixed63(-x);
   }


   /**
   divide the current value

   \param rhs reference to the object beeing the denominator

   \returns reference to the current obeject containing the result

   \throws ArithmeticDivideByZeroSignal
   */
   Fixed63 Fixed63::operator/=(const Fixed63& rhs) {
      if (rhs.x == 0) {
         throw theArithmeticDivideByZeroSignal;
      }

      if (x == MinInt && rhs.x == -1) {
         throw theArithmeticOverflowSignal;
      }

      x /= rhs.x;
      return *this;
   }


   /**
   divide two value like a / b

   \param rhs reference to the denominator
   \returns new object containing the result
   \throws ArithmeticDivideByZeroSignal
   */
   Fixed63 Fixed63::operator/(const Fixed63& rhs) const {
      return Fixed63(*this) /= rhs;
   }


   /**
   modulo operation

   in PEARL it is called REM (reminder)

    PEARL requires  a == (a // b) * b + a REM b;
    where // denotes the integer division<br>
      10 //  3 *  3 =  9 ==>  10 REM  3 =  1 	<br>
     -10 //  3 *  3 = -9 ==> -10 REM  3 = -1 	 <br>
      10 // -1 * -3 =  9 ==>  10 REM -3 =  1	<br>
     -10 // -3 * -3 = -9 ==> -10 REM -3 = -1	<br>
    result has same sign as first operand		<br>
    this behavior is the same as in C++		<br>

   \param rhs reference to the denominator

   \returns reference to the current object containing the result

   \throws ArithmeticDivideByZeroSignal
   */

   Fixed63 Fixed63::operator%=(const Fixed63& rhs) {
      if (rhs.x == 0) {
         throw theArithmeticDivideByZeroSignal;
      }

      x = (rhs.x == -1) ? 0 : x % rhs.x;
      return (*this);
   }


   /**
   modulo operation

   in PEARL it is called REM (reminder)

    PEARL requires  a == (a // b) * b + a REM b;
    where // denotes the integer division<br>
      10 //  3 *  3 =  9 ==>  10 REM  3 =  1 	<br>
     -10 //  3 *  3 = -9 ==> -10 REM  3 = -1 	 <br>
      10 // -1 * -3 =  9 ==>  10 REM -3 =  1	<br>
     -10 // -3 * -3 = -9 ==> -10 REM -3 = -1	<br>
    result has same sign as first operand		<br>
    this behavior is the same as in C++		<br>

   \param rhs reference to the denominator

   \returns new containing the result

   \throws ArithmeticDivideByZeroSignal
   */
   Fixed63 Fixed63::operator%(const Fixed63& rhs) const {
      return Fixed63(*this) %= rhs;
   }


   /**
   internal 64 bit multiplication with exception throwing
   in case of range overflow
   */
   void Fixed63::regMultiply(const UFixed63_t& a,
                             const UFixed63_t& b, UFixed63_t *ret) {
      __uint32 aHigh, aLow, bHigh, bLow;
      // Consider that a*b can be broken up into:
      // (aHigh * 2^32 + aLow) * (bHigh * 2^32 + bLow)
      // => (aHigh * bHigh * 2^64) +
      //          (aLow * bHigh * 2^32) + (aHigh * bLow * 2^32) +
      //          (aLow * bLow)
      // Note - same approach applies for 128 bit math on a 64-bit system
      aHigh = (__uint32)(a >> 32);
      aLow  = (__uint32)a;
      bHigh = (__uint32)(b >> 32);
      bLow  = (__uint32)b;
      *ret = 0;

      if (aHigh == 0) {
         if (bHigh != 0) {
            *ret = (UFixed63_t)aLow * (UFixed63_t)bHigh;
         }
      } else if (bHigh == 0) {
         if (aHigh != 0) {
            *ret = (UFixed63_t)aHigh * (UFixed63_t)bLow;
         }
      } else {
         throw theArithmeticOverflowSignal;
      }

      if (*ret != 0) {
         UFixed63_t tmp;

         if ((__uint32)(*ret >> 32) != 0) {
            throw theArithmeticOverflowSignal;
         }

         *ret <<= 32;
         tmp = (UFixed63_t)aLow * (UFixed63_t)bLow;
         *ret += tmp;

         if (*ret < tmp) {
            throw theArithmeticOverflowSignal;
         }

         return;
      }

      *ret = (UFixed63_t)aLow * (UFixed63_t)bLow;
   }


   /**
   multiplication operation


   \param rhs reference to the seconds multiplicator

   \returns reference to the current object containing the result

   \throws ArithmeticOverflowSignal
   */
   Fixed63 Fixed63::operator*=(const Fixed63& rhs) {
      UFixed63_t tmp;
      Fixed63_t y = rhs.x;
      bool lhsNegative = false;
      bool rhsNegative = false;

      if (x < 0) {
         lhsNegative = true;

         if (x == MinInt && y != 1) {
            throw theArithmeticOverflowSignal;
         }

         x = -x;
      }

      if (y < 0) {
         rhsNegative = true;

         if (y == MinInt && x != 1) {
            throw theArithmeticOverflowSignal;
         }

         y = -y;
      }

      regMultiply((unsigned Fixed63_t)x, (unsigned Fixed63_t)y, &tmp);
      x = (Fixed63_t)tmp;

      if (x < 0) {
         throw theArithmeticOverflowSignal;
      }

      if (lhsNegative ^ rhsNegative) {
         x = -x;
      }

      return (*this);
   }



   /**
   multiplication operation


   \param rhs reference to the seconds multiplicator

   \returns new object containing the result

   \throws ArithmeticOverflowSignal
   */
   Fixed63 Fixed63::operator*(const Fixed63& rhs) const {
      return Fixed63(*this) *= rhs;
   }

   /**
   compare operation

   in order to keep the class small, only one compare operation is available

   \param rhs reference to the seconds parameter

   \returns >0, if this objects value > rhs value <br>
   =0, if both values are equal<br>
   <0, if this objects value < rhs value

   */
   int Fixed63::compare(const Fixed63& rhs) const {
      if (x < rhs.x) {
         return -1;
      } else if (x > rhs.x) {
         return +1;
      }

      return 0;
   }

}
