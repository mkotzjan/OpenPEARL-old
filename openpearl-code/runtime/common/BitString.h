#ifndef BITSTRING_H_INCLUDED
#define BITSTRING_H_INCLUDED
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

\brief The data types BIT(x)

\author R. Mueller


The data type BIT(x) is realized completly with templates.
Depending on the length of the requested bit string,
the next most integral data type will be used.

*/


#include <stdio.h>
#include <stdint.h>

#include "Fixed.h"
//#include "Fixed63.h"
#include "Signals.h"
#include "Log.h"

namespace pearlrt {
#include "NumberOfBytes.h"

   template<int S> class Bits;

   /** helper class to set the used data type for bit string
       which fit into one byte
   */
   template<> class Bits<1> {
   public:
      /**
      type definition of the native integral data type
      to use for storing BIT(1) .. BIT(8) values
      \returns just defines the type
      */
      typedef uint8_t BitType;
   };

   /** helper class to set the used data type for bit string
       which fit into two bytes
   */
   template<> class Bits<2> {
   public:
      /**
      type definition of the native integral data type
      to use for storing BIT(9) .. BIT(16) values
      \returns just defines the type
      */
      typedef uint16_t BitType;
   };

   /** helper class to set the used data type for bit string
       which fit into four bytes
   */
   template<> class Bits<4> {
   public:
      /**
      type definition of the native integral data type
      to use for storing BIT(17) .. BIT(32) values
      \returns just defines the type
      */
      typedef uint32_t BitType;
   };

   /** helper class to set the used data type for bit string
       which fit into eight bytes
   */
   template<> class Bits<8> {
   public:
      /**
      type definition of the native integral data type
      to use for storing BIT(33) .. BIT(64) values
      \returns just defines the type
      */
      typedef uint64_t BitType;
   };

#include "IfThenElseTemplate.h"

   /**
   BitString contains a fixed number of bits.
   Depending on the length of the bit string, different data conatiners are
   used. 1-8 bit are stored in one byte,
   9-16 bits are stored in 16 bit integer,
   17-32 bits are stored in a 32 bit integer.
   33-64 bits are stored in a 64 bit integer.

   The bits are stored adjusted at the msb of the data container.

   The methodes are named as bitAnd / bitOr / ... since and, or, not,...
   are predefined keywords in the C++ standard for the corresponding
   logical operators.

   \note On creation of an bit string from an fixed value, no range
   tests are done, since shifting bits out of a bit string
   is a normal operation. Therefore no BitRangeSignal exists.

   */
   template<int S> class BitString {

   private:
      /**
       intermediate value, which contains the number of bytes used
       for storing the bit string
      */
      static const int len = NumberOfBytes<S>::resultBitString;

   public:
      /**
      Synonym for the data type of the internal storage

      \returns just the data type definition
      */
      typedef typename Bits<len>::BitType DataType;

      /**
      the data storage

      The data storage is kept public for easy and fast access - no
      get or set methods are provided.
      This is ok, since the compiler will just generate code, which
      accesses the defined templates.

      \returns is the internal representation of the bit string
      */
      DataType x;

   private:
      static const int shiftSize = sizeof(x) * 8 - S;
      static const DataType mask = ((DataType)(-1) >> shiftSize)
                                   << shiftSize;

   public:
      /**
       default constructor initializes the data field with an
      empty bit string (all bits 0)
      */
      BitString() {
         x = 0;
      }

      /**
       construct with given preset value as corresponding C++-int-type.
       The alignment of the data is done within the constructor.
       If the preset value needs more bits than the BitString,
        the most significant bits of the preset are discarded.

       \param y the preset value, given as a c++ native integral value

      \note the length S must be derived from the given PEARL initial
            value. Eg. '011'B1 denotes a BIT(3) value and '0'B4 is a BIT(4)
            value.
      */
      BitString(DataType y) {
         x = y;
         x <<= shiftSize;
         x &= mask;
      }

      /**
      copy contructor

      \param y the BitString which should be copied
      \tparam P the length of the BitString
      */
      template <int P> BitString(BitString<P> y) {
         *this = y;
      }

      /**
      shift the bit string by a number of bits.
      This shift direction depends on the sign of the
      shift amount.

      \param l number of bit positions to shift; if negative shift right,
                 if positive shift left
      \returns the result of the shift operation
       */
      BitString<S> bitShift(const Fixed<15> l) {
         BitString<S> retval;
         retval.x = x;

         if (l.x > 0) {
            retval.x <<= l.x;
         } else {
            retval.x >>= -l.x;
         }

         retval.x &= mask;
         return (retval);
      }


      /**
      shift the bit string by a number of bits in a circular way.
      This shift direction depends on the sign of the
      shift amount.

      \param l number of bit positions to shift; if negative shift right,
                 if positive shift left
      \returns the result of the shift operation
       */
      BitString<S> bitCshift(const Fixed<15> l) {
         BitString<S> retval;
         retval.x = x;

         if (l.x > 0) {
            retval.x <<= l.x;
            retval.x |= (x >> (S - l.x));
         } else {
            retval.x >>= -l.x;
            retval.x |= x << (S + l.x);
         }

         retval.x &= mask;
         return (retval);
      }

      /**
      invert all bits in the bit string (one's complement)

      \returns a bit string of the same size as the current one.
               The returned value is the one's complement of the
               current bit string
      */
      BitString<S> bitNot() {
         BitString z;
         z.x = ~ x;
         z.x &= mask;
         return (z);
      }

      /**
      concatinate two bit strings

      the resulting bit string has the length of the sum of the two
      given components.

      \param y the second part of the resulting bit string

      \returns the concatenated bit string
      */
      template<int P> BitString < S + P >
      bitCat(BitString<P> y) {
         BitString < S + P > z;
         z.x = x << ((sizeof(z) - sizeof(x)) * 8);
         z.x |= y.x << ((sizeof(z) - sizeof(x)) * 8 - S);
         return z;
      }

      /**
        get slice from the bitstring

        \param start starting index; the leftmost bit is adressed by 1
        \param dummy a dummy parameter just to indicate the size of the
                  requested slice
        \tparam P the length of the slice
      \returns the selected bit slice
             \throws BitIndexOutOfRangeSignal if the start is too low, or slice
                     would be too long for the bitstring (e.g. start-1+P <= S)
          */
      template <int P> BitString<P>
      getSlice(const Fixed<15>start, BitString<P> dummy) {
         if (start.x - 1 + P > S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }

         DataType s;
         s = x >> (sizeof(s) * 8 - (start.x - 1 + P));
         BitString<P> returnValue(s);
         return returnValue;
      }

      /**
        set slice of the bitstring

        \param start starting index; the leftmost bit is adressed by 1
        \param slice the new value of the slice
        \tparam P the length of the slice
        \throws BitIndexOutOfRangeSignal if the start is too low, or slice
                would be too long for the bitstring (e.g. start-1+P <= S)
      */
      template <int P> void
      setSlice(const Fixed<15>start, const BitString<P> slice) {
         if (start.x - 1 + P >= S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }

         DataType s, m;
         static const int lengthAdjust = (sizeof(s) - sizeof(slice)) * 8;
         s = slice.x;
         s <<= lengthAdjust;        // adjust in case of S>P
         s >>= start.x - 1;
         // mask x depending of start and P -->
         //    first start.x-1 bits are 1; (counting starts at 1!!!)
         //    P zeros following;
         //    1's till the end
         // eg. start=2; P=3; S=9
         // xxxx'xxxx'x
         // 0111'0000'0000'0000
         m = mask;
         m <<= start.x - 1;
         m >>= start.x - 1;
         m >>= (sizeof(m) * 8 - P - start.x + 1 + lengthAdjust);
         m <<= (sizeof(m) * 8 - P - start.x + 1 + lengthAdjust);
         x &= ~m;
         x |= s;
         return;
      }

      /**
        get single bit from the bitstring

        \param start starting index; the leftmost bit is adressed by 1
      \returns the adressed bit as Bit<1>
             \throws BitIndexOutOfRangeSignal if the start is too low, or slice
                     would be too long for the bitstring (e.g. start-1+P <= S)
          */
      BitString<1>
      getBit(const Fixed<15>start) const {
         if (start.x > S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }

         DataType s;
         s = x >> (sizeof(s) * 8 - start.x);
         BitString<1> returnValue(s);
         return returnValue;
      }

      /**
        set single bit from the bitstring

        \param start starting index; the leftmost bit is adressed by 1
      \param newValue the new value for the adressed bit
        \throws BitIndexOutOfRangeSignal if the start is too low, or slice
                would be too long for the bitstring (e.g. start-1+P <= S)
      */
      void setBit(const Fixed<15>start, const BitString<1>newValue) {
         if (start.x > S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }

         DataType s;
         s = 1 << (sizeof(s) * 8 - start.x);

         if (newValue.x) {
            x |= s;
         } else {
            x &= ~s;
         }

         return;
      }

      /**
      create a fixed value from the bit string.

      The fixed value is the binary representation of the bit string
      with proper alignment for the fixed value.

      The sign bit of the fixed value is always zero (positive)

      \returns the bits as positive Fixed<S> value
      */
      Fixed<S> toFixed() const {
         Fixed<S> returnValue;
         returnValue.x = x >> shiftSize;
         return returnValue;
      }

      /**
      return a C++ boolean (bool) value of the bit string

      \returns true, if at least one bit is set
      \returns false, if all bits are cleared
      */
      bool inline getBoolean() {
         return (!!x);
      }

   private:
      /*-----------------------------------------------------*/
      /* mixed operations                                    */
      /*   in mixed operations the shorter bitstring must be */
      /*   extended to the length of the longer one, adding  */
      /*   0-bits. The result type is the longer type        */
      /*-----------------------------------------------------*/

      /**
      concrete definition of the code for condition true

      The condition is 'left string' < 'right string'. This means that the
      first parameter must be extended to the size of the second parameter.

      This behavior must be available for 'and', 'or' and 'xor' operation. This
      is realized by 3 template functions
      */
      template<int P> struct THENAND {
         static BitString < (S < P) ? P : S >
         bitAnd(BitString<S>x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;
            z.x &= y.x;
            return z;
         }

         static BitString < (S < P) ? P : S >
         bitOr(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;
            z.x |= y.x;
            return z;
         }

         static BitString < (S < P) ? P : S >
         bitXor(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;
            z.x ^= y.x;
            return z;
         }

         static void
         bitAssign(BitString<S>* x, BitString<P> y) {
            Log::error("assignment only allowed to larger or equal length "
                       " Bit(%d) := Bit(%d)", S, P);
            throw theInternalDatatypeSignal;
         }

         static BitString<1>
         bitCompare(const BitString<S> x, const BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;

            if (z.x == y.x) {
               BitString<1> result(1);
               return result;
            } else {
               BitString<1> result(0);
               return result;
            }
         }
      };


      /**
      concrete definition of the code for condition false

      The condition is 'left string' < 'right string'. This means that the
      second parameter must be extended to the size of the first parameter.

      This behavior must be available for 'and', 'or' and 'xor' operation. This
      is realized by 3 template functions
      */
      template<int P> struct ELSEAND {
         static BitString < (S < P) ? P : S >
         bitAnd(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;
            z.x &= x.x;
            return z;
         }

         static BitString < (S < P) ? P : S >
         bitOr(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;
            z.x |= x.x;
            return z;
         }

         static BitString < (S < P) ? P : S >
         bitXor(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;
            z.x ^= x.x;
            return z;
         }

         static void
         bitAssign(BitString<S>* x, BitString<P> y) {
            x->x = y.x;
            x->x <<= (sizeof(*x) - sizeof(y)) * 8;
            return;
         }

         static BitString<1>
         bitCompare(const BitString<S> x, const BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;

            if (z.x == x.x) {
               BitString<1> result(1);
               return result;
            } else {
               BitString<1> result(0);
               return result;
            }
         }
      };

   public:
      /**
      operator=

      assignment of two bit string.
      The rhs must be shorter (or equal) to the left hands side (this)
      otherwise an exception is created.

      \param y the right hand side
      \tparam S the result length of  the resulting BitString
      \tparam P the length of the right hand side

      \returns the length adjusted copy of the value y

      */
      template< int P>
      BitString <S> operator= (const BitString<P> y) {
         IF < (S < P), THENAND<P>, ELSEAND<P> >::
         SELECT_CLASS::bitAssign(this, y);
         return *this;
      }

      /**
      bitAnd

      realizes a bitwise AND of two bit string.
      The shorter bit string will be extended at the right side to the
      length of the longer bit string.
      The result type is given by the longer bit string.
      */
      template<int P>
      BitString < (S < P) ? P : S > bitAnd(BitString<P> y) {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitAnd(*this, y);
      }

      /**
      bitOr

      realizes a bitwise OR of two bit string.
      The shorter bit string will be extended at the right side to the
      length of the longer bit string.
      The result type is given by the longer bit string.
      */
      template<int P>
      BitString < (S < P) ? P : S > bitOr(BitString<P> y) {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitOr(*this, y);
      }

      /**
      bitXor

      realizes a bitwise EXCLUSIVE OR (XOR) of two bit string.
      The shorter bit string will be extended at the right side to the
      length of the longer bit string.
      The result type is given by the longer bit string.
      */
      template< int P>
      BitString < (S < P) ? P : S > bitXor(BitString<P> y) {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitXor(*this, y);
      }


      /**
      compare equal

      compare two bit strings

      The shorter bit string is extended with 0 at the right side
      up to the length of the longer bit string

      \returns '1'B1  if both are equal, '0'B1 else.
      */
      template<int P>
      BitString<1> operator== (const BitString<P> y) const {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitCompare(*this, y);
      }

      /**
      compare not equal

      compare two bit strings

      The shorter bit string is extended with 0 at the right side
      up to the length of the longer bit string
      \returns '1'B1  if both are equal, '0'B1 else.
      */
      template<int P>
      BitString<1> operator!= (const BitString<P> y) const {
         return (IF < (S < P), THENAND<P>, ELSEAND<P> >::
                    SELECT_CLASS::bitCompare(*this, y)).bitNot();
      }

   };

}
#endif

