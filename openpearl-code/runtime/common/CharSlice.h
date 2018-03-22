/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

\brief Data type CharSlice

This class provides the character slices, which
is needed if the length of a CHAR(start:end) is not computable by the
compiler.

The class is also useful for all slice operations.

*/
#ifndef CHARSLICE_INCLUDED
#define CHARSLICE_INCLUDED

#include "Signals.h"
#include "Fixed.h"
//#include "BitString.h"
#include "Character.h"

namespace pearlrt {
   /**
     \brief Data type CharSlice

     All slice operations line .CHAR(s:e) on left habd side and
     right hand side are done with this helper call.

     PEARL:

     \code{.unparsed}
     SPC proc (a CHAR(4), b CHAR(5) IDENT);

     DCL x CHAR(10);
     DCL y CHAR(3);

     y := x.CHAR(3:5);  ! select digits# 345 from  x
     y.CHAR(1:5).CHAR(2:3) := x.CHAR(1:2);

     ! pass shorter string to the proc
     CALL proc(x.CHAR(1:3), y.CHAR(1:5));

     \endcode

     C++:
     \code{.unparsed}
     pearlrt::Character<10> x;
     pearlrt::Character<3> y;
     const pearlrt::Fixed<15> one(1), two(2), three(3), four(4), five(5);

     // y := x.CHAR(3:5);  ! select digits# 345 from  x
     y = *(CharSlice(x)
            .getSlice(three, five)
            .mkCharPtr((pearlrt::Character<3>*)0));

    // y.CHAR(1:5).CHAR(2:3) := x.CHAR(1:2);
    CharSlice(y)
       .getSlice(one,five)
       .getSlice(two, three)
       .setSlice(CharSlice(x).getSlice(one,two));

    {
       Character<4> tmp;   // create temporary object, if the lengths differ
       tmp = *(CharSlice(x)
   	.getSlice(one, four)
                .mkCharPtr((Character<4>*)0)
              );  // the remaining space is padded with space
       proc(
           tmp,
           CharSlice(y).getSlice(one, five).mkCharPtr((Character<5>*)0)
       );
    }
     \endcode

   */
   class CharSlice {
   public:
      /** pointer to the data region of the character slice

      This region is part of a normal Character<S> object
      */
      char *data;

      /**
      The length of the character slice
      */
      Fixed<15> length;

      /**
      create a character slice upon a complete Character variable

      The character slice operates on the original data

      \param source is the character variable, which holds the data of the
                CharSlice
      \tparam S the length of the source Character string

      */
      template<size_t S>CharSlice(Character<S>& source) {
         length = Fixed<15>(S);
         data   = source.get();
      }

      /**
      create a pointer to a Character variable upon the data of the
      CharSlice object.

      The length of the return value must be identical or shorter
      as the current CharSlice length

      \param dummy a dummy parameter to transport the template parameter.
           It is ok that dummy is NULL
      \return pointer to a Character<S> variable
      \tparam S the length of the source Character string
      */
      template<size_t S>
      Character<S>* mkCharPtr(Character<S>* dummy) {
         if ((int)S > length.x) {
            printf("mkCharacter: illegal length %d <-> %d", (int)S, length.x);
            throw theCharacterTooLongSignal;
         }

         return (Character<S>*) data;
      }

      /**
      obtain a slice of the CharSlice

      \param start the staring character index (begins counting at 1)
      \param end the last character index (begins counting at 1)

      \throws CharacterIndexOutOfRangeSignal if start or end
            is outside of the limits of the current object
      \return reference to the same object
      */
      CharSlice& getSlice(Fixed<15> start, Fixed<15> end);

      /**
      write the content of rhs to the current object

      The object rhs may be smaller or equal in length.
      If rhs is smaller, the assigned substring is padded
       with spaces at the end

      \throws CharacterTooLongSignal if the rhs value is too large
      \param rhs the new value of the substring, which is adressed
                 by the current object
      */
      void setSlice(const CharSlice& rhs);

      /**
      upper bound

      return the upper limit of the index

      \returns length of the string
      */
      Fixed<15> upb() const;


      /**
      getCharAt(p)   .CHAR(p) in PEARL

      Return the character at position p of the character string.

      \param p position of the requested character
      \return Char at position p
      \throws CharacterIndexOutOfRangeSignal if p is out of range
      */
      Character<1>  getCharAt(Fixed<15> p);

      /**
      setCharAt(p,c)   .CHAR(p) in PEARL on left hand side

      Replace the character at position p of the character string with
      the given parameter.

      \param p position of the requested character
      \param c the new value of the character to be replaced
      \throws CharacterIndexOutOfRangeSignal if p is out of range
      */
      void setCharAt(Fixed<15> p, Character<1> c);


      /**
      TOFIXED

      deliver the ASCII-value of the given character

      \throws CharacterTooLongSignal
      \returns ASCII value of the Character(1) value

      */
      Fixed<8> toFixed();

      /**
       get the data

       \returns a C-pointer to the data. It is not enshured that the
           string is null-terminated.

      */
      char *  get();

   };   // end of class  CharSlice

};

#endif
