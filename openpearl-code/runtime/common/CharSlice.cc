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

This class provides the the variable character string, which
is needed if the length of a CHAR(start:end) is not computable by the
compiler.

*/

#include "Signals.h"
#include "Fixed.h"
#include "CharSlice.h"
#include "Character.h"

namespace pearlrt {


   /**
   upper bound

   return the upper limit of the index

   \returns length of the string
   */
   Fixed<15> CharSlice::upb() const {
      return length;
   }



   /**
   getCharAt(p)   .CHAR(p) in PEARL

   Return the character at position p of the character string.

   \param p position of the requested character
   \return Char at position p
   \throws CharacterIndexOutOfRangeSignal
   */
   Character<1>  CharSlice::getCharAt(Fixed<15> p) {
      if (p.x > length.x || p.x < 1) {
         //printf("Character: index out of range (%d)", p.x);
         throw theCharacterIndexOutOfRangeSignal;
      }

      Character<1> c;
      c.data[0] = data[p.x - 1];
      return c;
   }

   /**
   setCharAt(p,c)   .CHAR(p) in PEARL on left hand side

   Replace the character at position p of the character string with
   the given parameter.

   \param p position of the requested character
   \param c the new value of the character to be replaced
   \throws CharacterIndexOutOfRangeSignal
   */
   void CharSlice::setCharAt(Fixed<15> p, Character<1> c) {
      if (p.x > length.x || p.x < 1) {
         //printf("Character: index out of range (%d)", p);
         throw theCharacterIndexOutOfRangeSignal;
      }

      data[p.x - 1] = c.data[0];
   }


   /**
   TOFIXED

   deliver the ASCII-value of the given character

   \throws CharacterTooLongSignal
   \returns ASCII value of the Character(1) value

   */
   Fixed<8> CharSlice::toFixed() {
      if (length.x > 1) {
         //printf("Character::toFixed: illegal length (%d)", length);
         throw theCharacterTooLongSignal;
      }

      return (Fixed<8>)data[0];
   }

   /**
    get the data

    \returns a C-pointer to the data. It is not enshured that the
        string is null-terminated.

   */

   char *  CharSlice::get() {
      return data;
   }

   CharSlice& CharSlice::getSlice(Fixed<15>start, Fixed<15> end) {
      const Fixed<15> one(1);

      if (start.x < 1 || length.x < end.x) {
         printf("illegal slice(%d:%d) length: %d\n",
                start.x, end.x, length.x);
         throw theCharacterIndexOutOfRangeSignal;
      }

      length = end - start + one;
      data += start.x - 1;
      return (*this);
   }

   void CharSlice::setSlice(const CharSlice& rhs) {

      if (length.x < rhs.length.x) {
         printf("length does not match %d <-> %d\n",
                length.x, rhs.length.x);
         throw theCharacterTooLongSignal;
      }

      if (this != &rhs) {
         characterSafeCopy(data, rhs.data, rhs.length.x);
         characterFillSpaces(data + rhs.length.x, length.x  - rhs.length.x);
      }
   }

}
