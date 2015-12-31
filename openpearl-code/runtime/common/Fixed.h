#ifndef FIXED_H_INCLUDED
#define FIXED_H_INCLUDED
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

\brief The data types FIXED(i=1..63)

\author R. Mueller


The data type FIXED(x) is realized completly with templates.
Depending on the requested number of mantissa bits,
the next most integral data type will be used to store the internal value.

*/


#include <stdio.h>

#include <stdint.h>
#include "Signals.h"
#include "Fixed63.h"


namespace pearlrt {
#include "IfThenElseTemplate.h"
#include "NumberOfBytes.h"

   template<int S> class FixedTypes;

   /**
       \brief type definitions for 8 bit fixed

       helper class to define the used data type for FIXED variables
       which fit into one byte
   */
   template<> class FixedTypes<1> {
   public:
      /**
         wrapper of the used native data type of the data value
         \returns the data type for the data value
      */
      typedef int8_t NativeFixedType;

      /**
         wrapper of the unsigned representation of the native data type
         of the data value
         \returns the data type for the data value
      */
      typedef uint8_t UNativeFixedType;

      /**
         wrapper of the next larger native data type as the data value
         \returns the data type for the next larger type
      */
      typedef int16_t LongerNativeFixedType;
   };

   /**
       \brief type definitions for 16 bit fixed

       helper class to define the used data type for FIXED variables
       which fit into two byte
   */
   template<> class FixedTypes<2> {
   public:
      /**
         wrapper of the used native data type of the data value
         \returns the data type for the data value
      */
      typedef int16_t NativeFixedType;

      /**
         wrapper of the unsigned representation of the native data type
         of the data value
         \returns the data type for the data value
      */
      typedef uint16_t UNativeFixedType;

      /**
         wrapper of the next larger native data type as the data value
         \returns the data type for the next larger type
      */
      typedef int32_t LongerNativeFixedType;
   };

   /**
       \brief type definitions for 32 bit fixed

       helper class to define the used data type for FIXED variables
       which fit into four byte
   */
   template<> class FixedTypes<4> {
   public:
      /**
         wrapper of the used native data type of the data value
         \returns the data type for the data value
      */
      typedef int32_t NativeFixedType;

      /**
         wrapper of the unsigned representation of the native data type
         of the data value
         \returns the data type for the data value
      */
      typedef uint32_t UNativeFixedType;

      /**
         wrapper of the next larger native data type as the data value
         \returns the data type for the next larger type
      */
      typedef int64_t LongerNativeFixedType;
   };

   /**
       \brief type definitions for 64 bit fixed

       helper class to define the used data type for FIXED variables
       which fit into eight byte
   */
   template<> class FixedTypes<8> {
   public:
      /**
         wrapper of the used native data type of the data value
         \returns the data type for the data value
      */
      typedef int64_t NativeFixedType;

      /**
         wrapper of the unsigned representation of the native data type
         of the data value
         \returns the data type for the data value
      */
      typedef uint64_t UNativeFixedType;

      /**
         wrapper of the next larger native data type as the data value
         \returns the data type for the next larger type
      */
      typedef int64_t LongerNativeFixedType;  // only needed for consistency
   };

   /**
    \brief PEARL data type FIXED

    Depending on the length of the fixed type, a different data
    conatiners is used.
    All lengths from 1 to 63 are supported.

    1-7 bit are stored in one byte,
    8-15 bits are stored in 16 bit integer,
    16-31 bits are stored in a 32 bit integer.
    32-63 bits are stored in a 64 bit integer.

    The class is templated with the requested length of bits
    (not including the sign bit). Each operation is check whether
    the result fits into the specified number of bits.
   */
   template<int S> class Fixed {
   private:
      /**
       intermediate value, which contains the number of bytes used
       for storing the value
      */
      static const int len = NumberOfBytes < S + 1 >::resultFixed;

   public:

      /**
      Synonym for the data type of the internal storage

      just used as short form of the primary definition

      \returns just the data type definition
      */
      typedef typename FixedTypes<len>::NativeFixedType NativeType;

      /**
      Synonym for the data type with usinged interpretation
      of the internal storage

      just used as short form of the primary definition

      \returns just the data type definition
      */
      typedef typename FixedTypes<len>::UNativeFixedType UNativeType;

      /**
      Synonym for the next larger data type as the internal storage

      just used as short form of the primary definition

      \returns just the data type definition
      */
      typedef typename FixedTypes<len>::LongerNativeFixedType LongerNativeType;

      /**
      the data storage

      The data storage is kept public for easy and fast access - no
      get or set methods are provided.
      This is ok, since the compiler will just generate code, which
      accesses the defined templates.

      \returns is the internal representation of the value
      */
      NativeType x;

   private:

      struct LessThan32Bits {
         static Fixed add(const Fixed & lhs,
                          const Fixed & rhs)  {
            Fixed h;
            LongerNativeType r;
            h.x = rhs.x + lhs.x;
            r = rhs.x;
            r += lhs.x;

            if (h.x != r || r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }

            return h;
         }

         static Fixed complement(const Fixed & lhs)  {
            Fixed h;
            LongerNativeType r;
            h.x = -lhs.x;
            r = 0;
            r -= lhs.x;

            if (h.x != r || r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }

            return h;
         }

         static Fixed substract(const Fixed & lhs,
                                const Fixed & rhs) {
            Fixed h;
            LongerNativeType r;
            h.x = lhs.x - rhs.x;
            r = lhs.x;
            r -= rhs.x;

            if (r != h.x || r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }

            return h;
         }

         static Fixed multiply(const Fixed & lhs,
                               const Fixed & rhs) {
            Fixed h;
            LongerNativeType r;
            r = rhs.x;
            r *= lhs.x;

            if (r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }

            h = r;
            return h;
         }

         static Fixed divide(const Fixed & lhs,
                             const Fixed & rhs) {
            Fixed h;
            LongerNativeType r;

            if (rhs.x == 0) {
               throw theFixedDivideByZeroSignal;
            }

            r = lhs.x;
            r /= rhs.x;

            if (r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }

            h.x = r;
            return h;
         }



         static Fixed modulo(const Fixed & lhs, const Fixed & rhs) {
            Fixed h;

            if (rhs.x == 0) {
               throw theFixedDivideByZeroSignal;
            }

            if (lhs.x == minValue && rhs.x == -1) {
               return 0;
            }

            h.x = lhs.x % rhs.x;
            return h;
         }

      };
      struct MoreThan31Bits {
         static Fixed add(const Fixed & lhs,
                          const Fixed & rhs)  {
            Fixed r;
            Fixed63 h(lhs.x);
            h += Fixed63(rhs.x);
            r.x = h.get();

            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }

            return r;
         }

         static Fixed complement(const Fixed& lhs) {
            Fixed r;
            Fixed63 h;
            h =  -Fixed63(lhs.x);
            r.x = h.get();

            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }

            return r;
         }

         static Fixed substract(const Fixed& lhs, const Fixed & rhs) {
            Fixed r;
            Fixed63 h(lhs.x);
            h -= Fixed63(rhs.x);
            r.x = h.get();

            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }

            return r;
         }

         static Fixed multiply(const Fixed& lhs, const Fixed& rhs) {
            Fixed r;
            Fixed63 h(lhs.x);
            h *= Fixed63(rhs.x);
            r.x = h.get();

            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }

            return r;
         }

         static Fixed divide(const Fixed& lhs, const Fixed& rhs) {
            Fixed r;
            Fixed63 h(lhs.x);

            if (rhs.x == 0) {
               throw theFixedDivideByZeroSignal;
            }

            h /= Fixed63(rhs.x);
            r.x = h.get();

            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }

            return r;
         }


         static Fixed modulo(const Fixed& lhs, const Fixed& rhs) {
            Fixed r;
            Fixed63 h(lhs.x);
            h %= Fixed63(rhs.x);
            r.x = h.get();

            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }

            return r;
         }
      };


   private:
      static const NativeType maxValue =
         (((UNativeType)(-1))) >> (sizeof(x) * 8 - S);
      static const NativeType minValue =
         (((UNativeType)(-1)) >> S) << S;
      static const LongerNativeType mask = ((LongerNativeType)minValue) << 1;

   public:
      /**
       default constructor initializes the data field with an
      empty bit string (all bits 0)
      */
      Fixed() {
         x = 0;
      }

      /**
       construct with given preset value as corresponding C++-int-type.
       The alignment of the data is done within the constructor.

       If the current values need more bits than the Fixed can store
       an exception is thrown.

       \throws FixedRangeSignal if value is out of range

       \param y the preset value, given as a c++ native integral value
      */
      Fixed(LongerNativeType y) {
         if (y > maxValue || y < minValue) {
            throw theFixedRangeSignal;
         }

         x = y;
      }

      /**
      operator=

      assignment operator of mixed types

      if the target variable is not large enough to store the actual value
      an exception will be thrown

      \param y the value to be assigned
      \returns the assigned value
      \throws FixedRangeSignal in case of exeeding the data range
      */
      template<int P> Fixed operator=(const Fixed<P> & y) {
         if (y.x > maxValue || y.x < minValue) {
            throw theFixedRangeSignal;
         }

         x = y.x;
         return *this;
      }

      /**
      cast operator

      convert the Fixed<S> into a Fixed<T>
      
      \returns the value in type Fixed<P>
      \throws FixedRangeSignal in case of exeeding the data range
      */
      template<int P> operator Fixed<P> () const {
         Fixed<P> result(x);
         return result;
      } 

      /**
      return the abs value of the Fixed

      \throws FixedRangexception, if x had the  minimum value

      \returns the abolute value of the Fixed
      */
      Fixed abs() {
         Fixed y(x);

         if (x < 0) {
            y = Fixed(-x);
         }

         return y;
      }

      /**
      return the sign of the Fixed

      The type of the return value is Fixed<1>

      \returns the sign value of the Fixed
       \returns -1, if negative
       \returns 0, if zero
       \returns +1, if positive
      */
      Fixed<1> sign() {
         if (x < 0) {
            return Fixed<1>(-1);
         } else if (x > 0) {
            return Fixed<1>(1);
         }

         return Fixed<1>(0);
      }

      /**
      add operation (dyadic)

      \param rhs the second paramater of the difference
      \returns difference between the internal value and the parameter
      \throws FixedRangeSignal if the difference would exceed the value
              range of the fixed type
      */
      Fixed add(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::SELECT_CLASS::add(*this, rhs)
                );
      }

      /**
      monadic minus operation

      \returns the negative value of the internal value
      \throws FixedRangeSignal if the sum would exceed the value
              range of the fixed type
      */
      Fixed operator- () const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::SELECT_CLASS::complement(*this)
                );
      }

      /**
      minus operation (dyadic)

      \param rhs the second paramater of the difference
      \returns difference between the internal value and the parameter
      \throws FixedRangeSignal if the difference would exceed the value
              range of the fixed type
      */
      Fixed substract(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::substract(*this, rhs)
                );
      }

      /**
      multiply operation

      \param rhs the second paramater of the product
      \returns product of the internal value and the parameter
      \throws FixedRangeSignal if the difference would exceed the value
              range of the fixed type
      */
      Fixed multiply(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::multiply(*this, rhs)
                );
      }

      /**
      divide operation

      \param rhs the second paramater of the quotient
      \returns quotient of the internal value and the parameter
      \throws FixedRangeSignal if the difference would exceed the value
              range of the fixed type
      */
      Fixed divide(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::divide(*this, rhs)
                );
      }

      /**
      modulo operation

      \param rhs the second paramater of the modulo operation
      \returns modulus of the internal value and the parameter
      \throws FixedRangeSignal if the difference would exceed the value
              range of the fixed type
      */
      Fixed modulo(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::modulo(*this, rhs)
                );
      }

      /** exponentiate

       detailed description of the behavior:<br>
       <em>anything</em> pow 0 returns 1 (as stated in IEE854)<br>
       <em>anything</em> pow  <em>anything > 0 </em> returns the result
                           or throws exception<br>
       <em>|anything| == 1 </em> pow  <em>anything < -1 </em> returns
                  <em>anything</em> pow <em>-exp</em><br>
       <em>|anything| > 1 </em> pow <em>anything < -1</em> returns 0<br>
       <em>*anything </em> pow <em>-1</em> returns 1/ <em>anything </em><br>

       \param rhs the exponent
       \returns result  of the operation
       \throws FixedRangeSignal, if numerical overflow or pow(0,0)
      */
      template<int P>Fixed pow(const Fixed<P>& rhs)const {
         Fixed result(1);
         Fixed base(x);
         int64_t exp = rhs.x;

         if (exp == 0) {
            if (x != 0) {
               result.x = 1;
            } else {
               throw  theFixedRangeSignal;
            }
         } else  if (exp > 0) {
            try {
               while (exp) {
                  if (exp & 1) {
                     result = result * base;
                  }

                  exp >>= 1;

                  if (exp) {
                     base = base * base;
                  }
               }
            } catch (ArithmeticOverflowSignal e) {
               throw  theFixedRangeSignal;
            }

            if (result.x > maxValue || result.x < minValue) {
               throw  theFixedRangeSignal;
            }
         } else if (exp == -1) {
            result.x = 1 / x;
         } else if (exp < -1) {
            if (x ==  1 || x == -1) {  // (+/- 1)**exp
               if (exp & 1) {
                  // -1 ** odd number --> -1
                  // +1 ** odd number --> 1
                  result.x = x;
               } else {
                  // -1 ** even negativ --> +1
                  result.x = 1;
               }
            } else {
               result.x = 0;;
            }
         }

         return result;
      }

      /**
      FIT

      change size of a Fixed to an other size

      \param rhs the fixed (constant) which defines the new size
      \returns the value of lhs in the type of rhs
      \throws FixedRangeSignal if the data does not fit in the new type
      */
      template<int P>
      Fixed<P> fit(const Fixed<P>& rhs) {
         Fixed<P> result;
         result = *this;
         return result;
      }

   };
   /**
   add operation

   \param rhs the second paramater of the addition
   \returns sum of internal value and the parameter
   \throws FixedRangeSignal if the sum would exceed the value
           range of the fixed type
   */
   template<int S, int P>
   Fixed < (S > P) ? S : P > operator+ (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.add(r);
   }

   /**
   minus operation

   \param rhs the second paramater of the substraction
   \returns difference of internal value and the parameter
   \throws FixedRangeSignal if the difference would exceed the value
           range of the fixed type
   */
   template<int S, int P>
   Fixed < (S > P) ? S : P > operator- (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.substract(r);
   }

   /**
   multiply operation

   \param rhs the second paramater of the multiplication
   \returns product of internal value and the parameter
   \throws FixedRangeSignal if the product would exceed the value
           range of the fixed type
   */
   template<int S, int P>
   Fixed < (S > P) ? S : P > operator* (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.multiply(r);
   }

   /**
   divide operation

   \param rhs the second parameter of the division (divisor)
   \returns fraction of internal value and the parameter
   \throws FixedRangeSignal if the product would exceed the value
           range of the fixed type
   \throws FixedDivideByZeroSignal if the divisor was 0
   */
   template<int S, int P>
   Fixed < (S > P) ? S : P > operator/ (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.divide(r);
   }

   /**
   modulo operation

   \param lhs the first parameter of the modulo
   \param rhs the second parameter of the modulo
   \returns reminder of internal value and the parameter
   \throws FixedDivideByZeroSignal if the divisor was 0
   */
   template<int S, int P>
   Fixed < (S > P) ? S : P > operator% (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.modulo(r);
   }
}
#endif

