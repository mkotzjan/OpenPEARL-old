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

namespace pearlrt {
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
        Character<6> hello((char*)"Hello ");
        Character<6> world((char*)"World.");
        catChar(hello, world, hw);
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
         unsigned int i;

         if (length < T) {
            //printf("Character: illegal length (%d)", length);
            throw theCharacterTooLongSignal;
         }


         for (i = 0; i < T; i++) {
            data[i] = string.data[i];
         }

         for (i = T; i < length; i++) {
            data[i] = ' ';
         }
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
      Character<1>  getCharAt(size_t p) {
         if (p > length || p < 1) {
            //printf("Character: index out of range (%d)", p);
            throw theCharacterIndexOutOfRangeSignal;
         }

         Character<1> c;
         c.data[0] = data[p - 1];
         return c;
      }

      /**
      setCharAt(p,c)   .CHAR(p) in PEARL

      Replace the character at position p of the character string with
      the given parameter.

      \param p position of the requested character
      \param c the new value of the character to be replaced
      \throws CharacterIndexOutOfRangeSignal
      */
      void setCharAt(size_t p, Character<1> c) {
         if (p > length || p < 1) {
            //printf("Character: index out of range (%d)", p);
            throw theCharacterIndexOutOfRangeSignal;
         }

         data[p - 1] = c.data[0];
      }


      /**
      TOFIXED

      deliver the ASCII-value of the given character

      \throws CharacterTooLongSignal
      \returns ASCII value of the Character(1) value

      */
      Fixed<8> toFixed() {
         if (length > 1) {
            //printf("Character::toFixed: illegale length (%d)", length);
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
         for (i=0; i< LRHS; i++) {
            data[i] = rhs.data[i]; 
         }
    
         if (length > LRHS) {
            for( /* start with last i */; i<length; i++) {
               data[i] = ' ';
            }
         }
      }
      return *this; 
   }        

   };   // end of class  Character

   /* ********************************************************* */
   /* Template functions                                        */
   /* ********************************************************* */

   /**
   toChar(x)

   \param x ascii value of a character
   \return Char with the given value
   */
   Character<1>  toChar(int x);


   /**
   \brief concatenation (CAT or ><)

    concatenates two character strings

    \param left starting part of the result string
    \param right ending part of the result string
    \param res the resulting string
    \tparam Tres the result type
    \tparam Tleft the type of the left hand parameeter
    \tparam Tright the type of the left hand parameeter
   */
   template<class Tres, class Tleft, class Tright>
   void catChar(Tleft& left, Tright& right, Tres& res) {
      int j = 0;

      if ((left.upb() + right.upb() != res.upb()).getBoolean()) {
         //printf("Character::catChar: result type too small");
         throw theCharacterTooLongSignal;
      }

      for (unsigned int i = 0; i < (size_t)(left.upb().x); i++) {
         res.data[j++] = left.data[i];
      }

      for (unsigned int i = 0; i < (size_t)(right.upb().x); i++) {
         res.data[j++] = right.data[i];
      }
   }

   /**
     getSliceChar(start,end) in PEARL: .CHAR(start,end)

     get a slice of a string

     In cases where the compiler can calculate the length of
     the resulting character string this method may be used

     If the target char-type is larger than the requested slice, it
     will be filled with spaces

     \param c the base character string
     \param start starting position of the requested substring
     \param end the last position of the requested substring
     \param res the resulting substring
     \throws CharacterTooLongSignal;
     \throws CharacterIndexOutOfRangeSignal;

     \tparam Tres type of the resulting substring
     \tparam Tcharx type of the base character string
   */
   template<class Tres, class Tcharx>
   void getSliceChar(Tcharx& c, size_t start, size_t end, Tres & res) {
      size_t len = end - start + 1;

      if (len > (size_t)(res.upb().x)) {
         //printf("Character::getSliceChar: result type too small");
         throw theCharacterTooLongSignal;
      }

      if (start < 1 || end > (size_t)(c.upb().x)) {
         //printf("Character::getSliceChar: illegal slice(%d:%d)",
         //  start, end);
         throw theCharacterIndexOutOfRangeSignal;
      }

      for (size_t i = 0; i < len; i++) {
         res.data[i] = c.data[start + i - 1];
      }
   }

   /**
     setSliceChar(start,end) in PEARL: .CHAR(start,end)

     set a slice of a string

     In cases where the compiler can calculate the length of
     the resulting character string this method may be used

     \param res the string, where the slice should be replaced
     \param start starting position of the requested substring
     \param end the last position of the requested substring
     \param c the new slice data
     \throws CharacterTooLongSignal;
     \throws CharacterIndexOutOfRangeSignal;

     \tparam Tres type of the resulting string
     \tparam Tcharx type of the new slice
   */
   template<class Tres, class Tcharx>
   void setSliceChar(Tres & res, size_t start, size_t end, Tcharx & c) {
      size_t len = end - start + 1;

      if (len < (size_t)(c.upb().x)) {
         //printf("Character::setSliceChar: result type too small");
         throw theCharacterTooLongSignal;
      }

      if (start < 1 || end > (size_t)(res.upb().x)) {
         //printf("Character::setSliceChar: illegal slice(%d:%d)",
         //  start, end);
         throw theCharacterIndexOutOfRangeSignal;
      }

      for (size_t i = 0; i < len; i++) {
         res.data[start - 1 + i] = c.data[i];
      }

      for (size_t i = len; i < (size_t)(c.upb().x); i++) {
         res.data[start - 1 + i] = ' ';
      }
   }

   /**
    compare two strings

    the shorter string is expanded with spaces

     \param lhs left hand side parameter
     \param rhs right hand side parameter
     \returns -1, if lhs<rhs <br>0, if lhs==rhs<br>+1, if lhs>rhs

   */
   template<class Tcharl, class Tcharr>
   int compareChar(Tcharl& lhs, Tcharr & rhs) {
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

}

#endif
