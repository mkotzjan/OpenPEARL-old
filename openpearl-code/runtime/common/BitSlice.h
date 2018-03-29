/*
 [A "BSD license"]
 Copyright (c) 2018 Rainer Mueller
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

\brief Data type BitSlice

This class provides the BitString slices, which
is needed if the length of a BIT(start:end) is not computable by the
compiler.

The class is also useful for all slice operations.

*/
#ifndef BITSLICE_INCLUDED
#define BITSLICE_INCLUDED

#include "Signals.h"
#include "Fixed.h"
#include "BitString.h"

namespace pearlrt {
   /**
     \brief Data type BitSlice

     All slice operations line .BIT(s:e) on left hand side and
     right hand side are done with this helper call.

     PEARL:

     \code{.unparsed}
     DCL x BIT(10);
     DCL y BIT(3);

     y := x.BIT(3:5);  ! select digits# 345 from  x
     y.BIT(1:5).BIT(2:3) := x.BIT(1:2);

     ! pass shorter string to the proc
     CALL proc(x.BIT(1:3), y.BIT(1:5));

   proc: PROC(a BIT(4), b BIT(5) IDENT);
    DCL c BIT(6);
    c := b;
    b := b AND a;
   END;

     \endcode

     C++:
     \code{.unparsed}
     pearlrt::BitString<10> x;
     pearlrt::BitString<3> y;
     const pearlrt::Fixed<15> one(1), two(2), three(3), four(4), five(5);

     // y := x.BIT(3:5);  ! select digits# 345 from  x
     y = pearlrt::BitSlice(x)
            .getSlice(three, five)
            .mkBitString((pearlrt::BitString<3>*)0));

    // y.BIT(1:5).BIT(2:3) := x.BIT(1:2);
    pearlrt::BitSlice(y)
       .getSlice(one,five)
       .getSlice(two, three)
       .setSlice(pearlrt::BitSlice(x)
                    .getSlice(one,two)
                    .mkBitString((pearlrt::BitString<2>*)0);

    // function call
    {
       BitString<4> tmp;   // create temporary object for bit strings
                           // which are pased by value
       tmp = BitSlice(x)
   	        .getSlice(one, three)
                .mkBitString((BitString<4>*)0)
             );  // the remaining bits are padded with 0
       // pass a BitSlice for BitString types by IDENT
       proc(tmp, BitSlice(y));
    }
   /----------
   void proc(pearlrt::Task*me, pearlrt::BitString<4> a,
    pearlrt::BitSlice b) {    // b may be passed also by reference

    pearlrt::BitString<6> c;

    // the compiler knows that b must be BitString<5>
    c = b.mkBitString((pearlrt::BitString<5>*)0);

    // b = b AND a
    b.setSlice(CONST_FIXED_ONE,
               CONST_FIXED_FIVE,
               b.mkBitString((pearlrt::BitString<5>*)0)
                 .bitAnd(a)
    );
   }
     \endcode
   */
   class BitSlice {
   public:
      /**
         pointer to the data region of the bit string
      */
      void *data;

      /**
         at the end, we need the length of the primary data string.
         The number of bytes is enough.
      */
      size_t primaryDataLength;

      /**
         The length in bits of the current bit slice
      */
      Fixed<15> length;

      /**
         The starting bit number in the primary bit string;
         the starting bit number starts counting according PEARL at 1
      */
      Fixed<15> firstSelectedBit;

      /**
         create a BitSlice slice upon a complete BitString variable

         \param source is the BitString variable, which holds the data of the
                   BitSlice
         \tparam S the length of the source BitString string
      */
      template <int S>
      BitSlice(BitString<S>& source) {
         length = Fixed<15>(S);
         data   = &source.x;
         primaryDataLength = sizeof(source);
         firstSelectedBit = Fixed<15>(1);
      }

      /**
         create a BitString variable upon the data of the
         BitSlice object.

         The length of the return value must be identical or shorter
         as the current BitSlice length

         \param dummy a dummy parameter to transport the template parameter.
              It is ok that dummy is NULL
         \return pointer to a BitString<S> variable
         \tparam RESLEN the length of the result BitString
      */
      template<int RESLEN>
      BitString<RESLEN> mkBitString(BitString<RESLEN>* dummy) {

         if (RESLEN < length.x) {
            Log::error("assignment to smaller bit string");
            throw theInternalDatatypeSignal;
         }

         if (primaryDataLength == 1) {
            // create a BitString for the result
            // with the primary data shifted to the right side
            // the ctor of BitString ignores higher bits in the
            // parameter
            // thus shift all bits after the end of the BitSlice
            // to the right.
            // the last bit number in the slice is
            //    firstSelectedBit + length - 1
            // we must shift the total number of bits reduced by the end index
            BitString<RESLEN> result(
               (*(uint8_t*)data) >> (8 - firstSelectedBit.x - length.x + 1)
               << (RESLEN - length.x));
            return result;
         } else if (primaryDataLength == 2) {
            BitString<RESLEN> result(
               (*(uint16_t*)data) >> (16 - firstSelectedBit.x - length.x + 1)
               << (RESLEN - length.x));
            return result;
         } else if (primaryDataLength == 4) {
            BitString<RESLEN> result(
               (*(uint32_t*)data) >> (32 - firstSelectedBit.x - length.x + 1)
               << (RESLEN - length.x));
            return result;
         } else {
            // no other length is supported
            BitString<RESLEN> result(
               (*(uint32_t*)data) >> (64 - firstSelectedBit.x - length.x + 1)
               << (RESLEN - length.x));
            return result;
         }

      }

      /**
         obtain a slice of the BitSlice

         \param start the starting bit index (begins counting at 1)
         \param end the last bit index (begins counting at 1)

         \throws BitIndexOutOfRangeSignal if start or end
               is outside of the limits of the current object
         \return reference to the same object
      */
      BitSlice& getSlice(Fixed<15> start, Fixed<15> end) {
         static const Fixed<15> one(1);

         if (start.x < 1 || length.x < end.x ||
               start.x > end.x) {
            Log::error("illegal selection on BIT(%d): .BIT(%d:%d)",
                       length.x, start.x, end.x);
            throw theBitIndexOutOfRangeSignal;
         }

         length = end - start + one;
         firstSelectedBit = firstSelectedBit + start - one;
         return (*this);
      }

      /**
         write the content of rhs to the current object

         The object rhs may be smaller or equal in length.
         If rhs is smaller, the assigned substring is padded
         with spaces at the end

         \throws BitIndexOutOfRangeSignal if the rhs value would leave
             the lhs length
         \tparam RHSSIZE the length of the new substring
         \param rhs the new value of the substring, which is adressed
                 by the current object
      */
      template<int RHSSIZE>
      void setSlice(BitString<RHSSIZE>& rhs) {
         static const Fixed<15> one(1);

         // the BitString class provides the modification of
         // a substring
         // data->setSlice(firstSelectedBit, lastBitNumber, rhs);
         // we must just cast the type of data to the appropriate length
         // where only the length of the base types are relevant
         if (primaryDataLength == 1) {
            ((BitString<8>*)data)->setSlice(firstSelectedBit,
                                            firstSelectedBit + length - one,
                                            rhs);
         } else if (primaryDataLength == 2) {
            ((BitString<16>*)data)->setSlice(firstSelectedBit,
                                             firstSelectedBit + length - one,
                                             rhs);
         } else if (primaryDataLength == 4) {
            ((BitString<32>*)data)->setSlice(firstSelectedBit,
                                             firstSelectedBit + length - one,
                                             rhs);
         } else if (primaryDataLength == 8) {
            ((BitString<64>*)data)->setSlice(firstSelectedBit,
                                             firstSelectedBit + length - one,
                                             rhs);
         }

         // no other length is supported in OpenPEARL
      }

   };   // end of class  BitSlice

};

#endif
