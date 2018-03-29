/*
 [A "BSD license"]
 Copyright (c) 2012-2017 Rainer Mueller
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

\brief Data type CHARACTER

This class provides the operations of the data type character.

*/
#ifndef CHARACTER_INCLUDED
#define CHARACTER_INCLUDED

#include <string.h>
#include "Signals.h"
#include "Fixed.h"
//#include "VarCharacter.h"

namespace pearlrt {
   /**
   perform the string compare according the PEARL rules

   This method is used by various API methods for Character and CharSlice
   comparisons

   \param s1  pointer to the data of the first string
   \param l1  length of the first string
   \param s2  pointer to the data of the second string
   \param l2  length of the second string
   \return <0 , if the first string is smaller than the second string
        <br>=0 , if the first string is equal with the second string
        <br> >0 , if the first string is greater than the second string

   */
   int characterCompare(const char * s1, int l1, const char* s2, int l2);

   /**
   fill the data area with spaces

   This method is used by various API methods for Character and CharSlice
   operations.

   If the length is less or equal 0 - no operation occurs

   \param dest  pointer to the data area to fill
   \param len the length of the data area
   */
   void characterFillSpaces(char * dest, int len);

   /**
   copy the data from source to destination

   This method is aware about overlapping regions from source and
   destination and performs the copy in the correct direction.

   If the length is less or equal 0 - no operation occurs

   \param dest pointer to the destination area
   \param source pointer to the source area
   \param len the number of bytes in the source area
   */
   void characterSafeCopy(char*dest, const char* source, int len) ;

   /**
     \brief data type character

     implementation of data type character

   DIN66253-2 indicates in Tab 7.4 that the length of character strings
   is limited to 15 Bit.
   RTOS-UH indicates no limit

   DIN66253-2 indicates that characters use the ASCII character set.
   Control characters (TOFIXED() < 32) are quoted with '\';
   eg 'ABC\ 0d 0a \\ DE'
   would contain a string with ar CR and LF inside. The spaces between the hex
   encoded values are not precisely spezified. The encoding must be treated by
   the compiler.

   \note Proposal: The internal 8 bit characater set is used.
         Control characters are treated without any numerical
         restrictions.
         The length of character strings is limited to the range
         of FIXED(15) as written in the language reference.

   The 'LENGTH' operation, which defines the default length of strings,
   should be handled inside the compiler.
   This class implementation requires dedicated length information.

   The first valid index is '1' (according to the PEARL standard)


   The language reference indicates that assignment between character
   strings are only possible, if they have the same length.
   There is no explicit statement for this behavior.

   The slice operations are treated by the class CharSlice.

   A too long initializer or length value may lead the
   CharacterTooLong-signal.

   Indexed access to the elements may lead to CharterIndexOutOfRange-signal.

   \tparam length the length of the character string

   \see toChar
   \see catChar
   \see compareChar
   \see getSliceChar
   \see setSliceChar

   <b>Sample PEARL-Code:</b>
   <pre>
    DCL hw CHARACTER(12);
    hw := 'Hello ' CAT 'World.';
    </pre>

    <b>Possible compiler output:</b>
    <pre>
    Character<12> hw;

    {
        Character<6> hello(6,(char*)"Hello ");
        Character<6> world(6,(char*)"World.");
         hw = hello.catChar(world);
    }
    </pre>

   */
   template<size_t length>
   class Character {

   public:
      /**
      \returns the character string elements
      */
      char data[length];

   public:
      /**
      create a character string with the given length.
      The string will be initialized with 'space'.

      \throws CharacterTooLongSignal
      */
      Character<length>() {
         size_t i;

         if (length > 32767 || length < 1) {
            printf("Character: illegal length (%d)\n", (int)length);
            throw theCharacterTooLongSignal;
         }

         for (i = 0; i < length; i++) {
            data[i] = ' ';
         }
      }

      /**
      create a character initialized with the given C-string.
      The resolving of quotation should have been treated by the compiler.

      \param string byte array with the preset values
             (may include 0-bytes)

      \throws CharacterTooLongSignal
      */
      Character(const char * string) {
         unsigned int i;
         unsigned int l = strlen(string);

         if (length > 32767 || l > length) {
            printf("Character: illegal length (%d)\n", (int)length);
            throw theCharacterTooLongSignal;
         }

         for (i = 0; i < l; i++) {
            data[i] = string[i];
         }

         for (i = l; i < length; i++) {
            data[i] = ' ';
         }
      }


      /**
      create a character initialized with the given C-string.
      The resolving of quotation should have been treated by the compiler.

      \param len number of bytes for the string
      \param string byte array with the preset values
             (may include 0-bytes)

      \throws CharacterTooLongSignal
      */
      Character(int len, const char * string) {
         unsigned int i;
         unsigned int l = len;

         if (length > 32767 || l > length) {
            //printf("Character: illegal length (%d)", length);
            throw theCharacterTooLongSignal;
         }

         for (i = 0; i < l; i++) {
            data[i] = string[i];
         }

         for (i = l; i < length; i++) {
            data[i] = ' ';
         }
      }

      /**
      create a character initialized with another Char-value

      \param string the other character value
             (may include 0-bytes)

      \throws CharacterTooLongSignal
      */
      template<size_t T> Character(Character<T> &string) {
         //unsigned int i;

         if (length < T) {
            //printf("Character: illegal length (%d)", length);
            throw theCharacterTooLongSignal;
         }
         characterSafeCopy(data, string.data, T);
         characterFillSpaces(data + T, length - T);

      }

      /**
      upper bound

      return the upper limit of the index

      \returns length of the string
      */
      Fixed<15> upb() const {
         return Fixed<15>(length);
      }



      /**
      getCharAt(p)   .CHAR(p) in PEARL

      Return the character at position p of the character string.

      \param p position of the requested character
      \return Char at position p
      \throws CharacterIndexOutOfRangeSignal
      */
      Character<1>  getCharAt(Fixed<15> p) {
         if ((size_t)p.x > length || p.x < 1) {
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
      void setCharAt(Fixed<15> p, Character<1> c) {
         if ((size_t)p.x > length || p.x < 1) {
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
      Fixed<8> toFixed() {
         if (length > 1) {
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
      char *  get() {
         return data;
      }

      /**
       assign string

       the shorter string is expanded with spaces

        \tparam LRHS length of right hand side parameter
        \param rhs right hand side parameter

        \returns reference to the assigned string

      */
      template<size_t LRHS>
      Character<length>&  operator=(const Character<LRHS> & rhs) {
         size_t i;

         if (length < LRHS) {
            printf("internal error: assign to smaller string\n");
            throw theInternalSignal;
         }

         // compare the object to check if they are the same
         // due the the template characteristics we compare the void*
         if ((void*)this != (void*)&rhs) {
            characterSafeCopy(data, rhs.data, LRHS);
            characterFillSpaces(data + LRHS, length - LRHS);
         }

         return *this;
      }

      /**
      \brief concatenation (CAT or ><)

       concatenates two character strings

       \param rhs the string to be added to the current string
       \tparam rhsLength the length of the string to be added
       \return the concatinated string
      */
      template<size_t LRHS>
      Character < length + LRHS > catChar(Character<LRHS> & rhs) {
         Character < length + LRHS > result;

         characterSafeCopy(result.data, data, length);
         characterSafeCopy(result.data + length, rhs.data, LRHS);

         return result;
      }


   };   // end of class  Character

   /* ********************************************************* */
   /* companion functions                                        */
   /* ********************************************************* */

   /**
   toChar(x)

   \param x ascii value of a character
   \return Char with the given value
   */
   Character<1>  toChar(int x);

#if 0
   /**
    compare two strings

    the shorter string is expanded with spaces

     \param lhs left hand side parameter
     \param rhs right hand side parameter
     \returns -1, if lhs<rhs <br>0, if lhs==rhs<br>+1, if lhs>rhs

   */
   template<size_t LLHS, size_t LRHS>
   int compareChar(Character<LLHS>& lhs, Character<RLHS> & rhs) {
      return BitString<1> (characterCompare(lhs.data, LLHS, rhs.data, LRHS));

      size_t i;
      size_t len = (size_t)(lhs.upb().x);

      if (len > (size_t)(rhs.upb().x)) {
         len = (size_t)(rhs.upb().x);
      }

      for (i = 0; i < len && (lhs.data[i] == rhs.data[i]); i++) {
         ;
      }

      if (i < len) {
         return lhs.data[i] - rhs.data[i];
      }

      if (len < (size_t)(lhs.upb().x)) {
         for (i = len; i < (size_t)(lhs.upb().x) && lhs.data[i] == ' '; i++) {
            ;
         }

         if (i == (size_t)(lhs.upb().x)) {
            return (0);
         } else {
            return lhs.data[i] - ' ';
         }
      }

      if (len < (size_t)(rhs.upb().x)) {
         for (i = len; i < (size_t)(rhs.upb().x) && rhs.data[i] == ' '; i++) {
            ;
         }

         if (i == (size_t)(rhs.upb().x)) {
            return (0);
         } else {
            return ' ' - rhs.data[i];
         }
      }

      return 0;
   }
#endif
}

#endif
