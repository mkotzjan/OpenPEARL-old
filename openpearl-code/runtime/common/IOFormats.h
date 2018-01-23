/*
 [A "BSD license"]
 Copyright (c) 2014-2017 Rainer Mueller
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
#ifndef IOFORMATS_H_INCLUDED
#define IOFORMATS_H_INCLUDED
/**
\file

\brief format routines for PUT/GET-userdation and CONVERT

*/
#include "Sink.h"
#include "Source.h"
#include "PutFixed.h"
#include "GetFixed.h"
#include "GetFloat.h"
#include "PutFloat.h"
#include "PutCharacter.h"
#include "GetCharacter.h"
#include "PutBitString.h"
#include "GetBitString.h"
#include "UserDationNB.h"
#include "IOJob.h"

namespace pearlrt {

   /**
   \file

   \brief I/O Formats for DationPG and CONVERT
   */

   /**
   \addtogroup io_common
   @{
   */

   /**
     \brief I/O-Formats for DationPG and CONVERT

     This class provides the formating routines for DationPG and CONVERT.
     In order to decouple the formating stuff from the low level input and
     output, all methods operate on specialized Source or Sink objects.

      */
   class IOFormats {

   private:

      Sink  *sink;
      Source  *source;

   public:
      /**
        configure source and sink object to perfom the i/o
        operations

        \param _sink pointer to the output data stream
        \param _source pointer to the input data stream
      */
      void setupIOFormats(Sink * _sink, Source* _source);

      /**
      Check if enough space/data is available for the operation

      This real implementation of this method is called before
      each i/o format statement to enshure enough space in the
      input or output stream. If there is not enough space or data
      an exception must be thrown.

      \param n number of bytes which are wanted to read or write

      \throws an exception if the dation/string has less space
      */
      virtual void checkCapacity(Fixed<31> n) = 0;

      /**
      output format A

      \param s string to be printed
      \tparam S size of the string
      */
      template<size_t S>
      void toA(Character<S> & s) {
         toA(s, S);
      };

      /**
      input format A

      \param s string to be read
      \tparam S size of the string
      */
      template<size_t S>
      void fromA(Character<S> & s) {
         fromA(s, S);
      };

      /**
      output format A(w)

      \param s string to be printed
      \param w width of the output field. This field limits the number of
               characters to be written
      \tparam S size of the string
      */
      template<size_t S>
      void toA(Character<S> &s, Fixed<31> w) {
         checkCapacity(w);
         RefCharacter rc;
         rc.setWork(s);

         // performs checks and output
         PutHelper::doPutChar(w.x, &rc, sink);
      }

      
      /**
      output formats A and A(w) for the IOJob interface

      \param s pointer to the data to be printed
      \param len length of the CHAR-type to be printed
      \param w width of the output field. This field limits the number of
               characters to be written
      */
      void toA(void *s, size_t len, Fixed<31> w);

      /**
      input formats A and A(w) for the IOJob interface

      \param s pointer to the data to be printed
      \param len length of the CHAR-type to be printed
      \param w width of the output field. This field limits the number of
               characters to be written
      */
      void fromA(void *s, size_t len, Fixed<31> w);

      /**
      input format A(w)

      \param s string to be read
      \param w width of the input field. This field limits the number of
               characters to be read
      \tparam S size of the string
      */

      template<size_t S>
      void fromA(Character<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetCharacter<S>::fromA(s, w, *source);
         return;
      }

      /**
      output formats Bx and Bx(w) for the IOJob interface, with x=1,2,3,4

      \param s pointer to the data to be printed
      \param index index, if an array slice is used
      \param len length of the BIT-type to be printed (1..64)
      \param base of the output format (1,2,3 or 4)
      \param w width of the output field. This field limits the number of
               characters to be written
      */
      void toBit(void *s, size_t index,
                 size_t len, int base,
                 const Fixed<31> w);

      /**
      output format B1(w)

      \param s string to be printed
      \param w width of the output field
      \tparam S size of the string
      */
      template<int S>
      void toB1(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB1(s, w, *sink);
      }

      /**
      input format B1(w)

      \param s string to be read
      \param w width of the input field
      \tparam S size of the string
      */
      template<int S>
      void fromB1(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB123(s, w, 1, *source);
         return;
      }

      /**
      output format B2(w)

      \param s string to be printed
      \param w width of the output field
      \tparam S size of the string
      */
      template<int S>
      void toB2(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB2(s, w, *sink);
      }

      /**
      input format B2(w)

      \param s string to be read
      \param w width of the input field
      \tparam S size of the string
      */
      template<int S>
      void fromB2(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB123(s, w, 2, *source);
         return;
      }

      /**
      output format B3(w)

      \param s string to be printed
      \param w width of the output field
      \tparam S size of the string
      */
      template<int S>
      void toB3(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB3(s, w, *sink);
      }

      /**
      input format B3(w)

      \param s string to be read
      \param w width of the input field
      \tparam S size of the string
      */
      template<int S>
      void fromB3(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB123(s, w, 3, *source);
         return;
      }

      /**
      output format B4(w)

      \param s string to be printed
      \param w width of the output field
      \tparam S size of the string
      */
      template<int S>
      void toB4(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB4(s, w, *sink);
      }

      /**
      input format B4(w)

      \param s string to be read
      \param w width of the input field
      \tparam S size of the string
      */
      template<int S>
      void fromB4(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB4(s, w, *source);
         return;
      }


      /**
       output format F with Fixed

       \param f value to be printed
       \param w width of the output field
       \param d number of decimals to be used
       \tparam  S width of the fixed value type
       */
      template<int S>
      void toF(Fixed<S>  f,
               const Fixed<31> w,
               const Fixed<31> d = 0) {
         checkCapacity(w);
         PutFixed<S>::toF(f, w, d, *sink);
      };

      /**
       output format F with Float with the IOJob interface

       \param f pointer to the FLOAT value to be printed
       \param len the length of the FLOAT type
       \param index the current index if we are in an array
       \param w width of the output field
       \param d number of decimals to be used
       \tparam  S width of the fixed value type
       */
      void toFloatF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      /**
       input format F with Float with the IOJob interface

       \param f pointer to the FLOAT value to be read
       \param len the length of the FLOAT type
       \param index the current index if we are in an array
       \param w width of the output field
       \param d number of decimals to be used
       \tparam  S width of the fixed value type
       */
      void fromFloatF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      /**
       output format F with Fixed with the IOJob interface

       \param f pointer to the FIXED value to be printed
       \param len the length of the FIXED type
       \param index the current index if we are in an array
       \param w width of the output field
       \param d number of decimals to be used
       \tparam  S width of the fixed value type
       */
      void toFixedF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      /**
       input format F with Fixed with the IOJob interface

       \param f pointer to the FIXED value to be read
       \param len the length of the FIXED type
       \param index the current index if we are in an array
       \param w width of the output field
       \param d number of decimals to be used
       \tparam  S width of the fixed value type
       */
      void fromFixedF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      /**
      input format F with FIXED

      \param f value to be read
      \param w width of the input field
      \param d number of decimals to be used
      \tparam  S width of the fixed value type
      */
      template<int S>
      void fromF(Fixed<S> & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0) {
         checkCapacity(w);
         GetFixed<S>::fromF(f, w, d, *source);
      };

      /**
       output format F with FLOAT

       \param f value to be printed
       \param w width of the output field
       \param d number of decimals to be used
       \tparam  S width of the float value type
       */
      template<int S>
      void toF(Float<S>  f,
               const Fixed<31> w,
               const Fixed<31> d = 0) {
         checkCapacity(w);
         PutFloat<S>::toF(f, w, d, *sink);
      };

      /**
      input format F with FLOAT

      \param f value to be read
      \param w width of the input field
      \param d number of decimals to be used
      \tparam  S width of the float value type
      */
      template<int S>
      void fromF(Float<S> & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0) {
         checkCapacity(w);
         GetFloat<S>::fromF(f, w, d, *source);
      };


      /**
       output format E with FLOAT

       \note On output, the E-format element has an additional parameter
             for the number of decimals in the exponent field.
             On input any valid number is accepted.

       \param f value to be printed
       \param w width of the output field
       \param d number of decimals to be used
       \param s number of significant digits (number of digits in from of the
               decimal point)
       \param e number of digits in exponent field
       \tparam  S width of the float value type
       */
      template<int S>
      void toE(Float<S>  f,
               const Fixed<31> w,
               const Fixed<31> d,
               const Fixed<31> s,
               const Fixed<31> e) {
         checkCapacity(w);
         PutFloat<S>::toE(f, w, d, s, e, *sink);
      };

      /**
      input format E with FLOAT

       \note On output, the E-format element has an additional parameter
             for the number of decimals in the exponent field.
             On input any valid number is accepted.
      \note the parameters w and s are only used to check
            the valid values - during the input processing they are ignored

      \param f value to be read
      \param w width of the input field
      \param d number of decimals to be used
      \param s number of significant digits
      \tparam  S width of the fixed value type
      */
      template<int S>
      void fromE(Float<S> & f,
                 const Fixed<31> w,
                 const Fixed<31> d,
                 const Fixed<31> s) {
         checkCapacity(w);
         GetFloat<S>::fromE(f, w, d, s, *source);
      };


      /**
       output format T

       \param f value to be printed
       \param w width of the output field
       \param d number of decimals to be used
       */
      void toT(const Clock f,
               const Fixed<31> w,
               const Fixed<31> d = 0);


      /**
      input format T

      \param f value to be read
      \param w width of the input field
      \param d number of decimals to be used
      */
      void fromT(Clock & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0) ;

      /**
       output format D

       \param f value to be printed
       \param w width of the output field
       \param d number of decimals to be used
       */
      void toD(const Duration f,
               const Fixed<31> w,
               const Fixed<31> d = 0);

      /**
       input format D

       \param f value to be read
       \param w width of the input field
       \param d number of decimals to be used
       */
      void fromD(Duration& f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0);

      /**
       treat one output job entry, which must be a data format element

       \param me pointer to the calling task
       \param dataEntry  pointer to the current entry
       \param index current index for array elements of simple types
       \param loopOffset current offset in loops for structs
       \param format pointer to the format entry to be used
       \returns 0, if done normally<br>
                1, if record wasd left
       */
      int putDataFormat(TaskCommon * me,
             IODataEntry * dataEntry, size_t index,
             size_t loopOffset,
             IOFormatEntry * format);

      /**
       treat one input job entry, which must be a data format element

       \param me pointer to the calling task
       \param dataEntry  pointer to the current entry
       \param index current index for array elements of simple types
       \param loopOffset current offset in loops for structs
       \param format pointer to the format entry to be used
       \returns 0, if done normally<br>
                1, if record wasd left
       */
      int getDataFormat(TaskCommon * me,
             IODataEntry * dataEntry, size_t index,
             size_t loopOffset,
             IOFormatEntry * format);

   };
   /** @} */
}
#endif
