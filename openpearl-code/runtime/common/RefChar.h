/*
 [A "BSD license"]
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

#ifndef REFCHAR_INCLUDED
#define REFCHAR_INCLUDED

/**
\file

\brief Data type REF CHARACTER

This class provides the operations of the data type ref char.

*/
#include <string.h>
#include "Signals.h"
#include "Character.h"

namespace pearlrt {

   /**
   \addtogroup datatypes
   @{
   */
   
   /**
   Type REF CHAR
   \brief REF CHAR Type for variable string length

   REF CHARs use a normal CHAR<x> variable as container for the
   stored characters. The RefChar provides additional information about
   current length and maximum length.

   It is easy to concatinate character variables and nice for flexible
   function calls.

   The functionality is located in template functions
   \see  storeRefChar

   <b>Sample PEARL code</b>
   <pre>
   DCL wrk CHARACTER(100);
   DCL result CHARACTER(50);
   DCL rc REF CHAR();
   rc := wrk;

   CONT rc := 'Hallo';
   CONT rc := CONT rc CAT ' Welt.';
   result := CONT rc;
   </pre>

   <b>possible compiler output</b>
   <pre>
   Character<100> wrk;
   Character<50> result;
   RefChar rc;

   setRefChar(rc,wrk);
   {
      rc.clear();
      Character<5> hello((char*)"Hallo");
      rc.add(hello);
   }
   {
      Character<6> welt((char*)" Welt.");
      rc.add(welt);
   }
   rc.store(result);
   </pre>

   */
   class RefCharacter {
   public:
      size_t max;     ///< the maximum size (derived from the container)
      size_t current; ///< the current content length
      char * data;    ///< pointer to the data area in the container
 
      /**
         constructor
      */
      RefCharacter();

      /**
         constructor and set work space character variable

         \tparam S the size of the Character value which is used as
                   initial storage of the RefCharacter

         \param rhs the container storage for the RefCharacter
      */
      template<size_t S>
      RefCharacter(Character<S> & rhs) {
         max = rhs.upb().x;
         current = 0; //max;
         data = &rhs.data[0];
      }

      /**
       setWork(rhs)

       set the working storage for the ref character variable.
       This sets the attribute max and content to the length of the
       container variable.

       \param rhs the string which should be used as working compound
       \tparam Tcharr the type  the string which should be used as working
              compound
               <br> This parameter is expected to be of type Character<x>
      */
      template<size_t S>
      void setWork(Character<S> & rhs) {
         max = rhs.upb().x;
         current = 0; //max;
         data = &rhs.data[0];
      }

      void setWork(void * s, size_t len);

      /**
        getCharAt

        retrieve the data element at the given index

        \param index the index of the desited character (starting at 0)
        \returns the selected character of the data string
        \throws CharacterIndexOutOfRangeSignal, if the index i out of range
      */
      char getCharAt(size_t index) ;

      /**
       getNextChar

       retrieve the next data element at the current index
 
       \returns the next character
       \throws CharacterIndexOutOfRangeSignal, if no more characters
                are available
      */
      char getNextChar();

      /**
        getCstring

        return the data as nil-terminated C-string.
        a NIL is appended to the current data (without
        incrementing the content-counter) to enshure
        a proper c-string behavior.

        \returns pointer to the first character of the data string
        \throws CharacterTooLongSignal, if the NIL does not fit into the
                work area
      */
      char * getCstring() ;

      /**
         set current content to 'empty'
      */
      void clear();

      /**
         fill the remaining data in the container to SPACE

         \note This filling does not affect the current content pointer!
      */
      void fill();

      /**
         add another ref char

         \param rhs the ref char to be added
         \throws CharacterTooLongSignal
                if the result would not fit in the container
      */
      void add(const RefCharacter & rhs);

      /**
         add another C string

         \param rhs the C string to be added
         \throws CharacterTooLongSignal
                if the result would not fit in the container
      */
      void add(const char * rhs);

      /**
         get size of the container
         \returns maximum size of the content
      */
      size_t getMax();

      /**
         get current content size
         \returns current size of the content
      */
      size_t getCurrent();

      /**
         set current content size
 
         this feature is needed for positioning in convert

         \param newpos set the read/write pointer to this offset.
                The positions starts counting with 0.

         \throws CharacterTooLongSignal, if new position would be
                                         outside of the string
      */
      void setCurrent(size_t newpos);

      /**
          add one character

          \param rhs the char to be added
          \throws CharacterTooLongSignal
                  if the result would not fit in the container
       */
      void add(const char rhs);
      /**
       add Character<S>

       add the string rhs to the end of the REF CHAR

       \param rhs the string which should be appended
       \tparam S the length of the Character variable which should be added
       \throws CharacterTooLongSignal
               if the result would not fit in the container
      */
      template<size_t S>
      void add(const Character<S> & rhs) {
         if ((size_t)(rhs.upb().x) > (max - current)) {
            throw theCharacterTooLongSignal;
         }

         for (size_t i = 0; i < (size_t)(rhs.upb().x); i++) {
            data[current + i] = rhs.data[i];
         }

         current += rhs.upb().x;
      }

      /**
       store to Character<S>

       store the  ref char into the fixed size string

       \param dest the target string of type Character<S>
       \tparam S the length of the target string
       \throws CharacterTooLongSignal
               if the content does not fit into the target
      */
      template<size_t S>
      void store(Character<S> & dest) {
         size_t len;
         len = current;

         if (len > (size_t)(dest.upb().x)) {
            throw theCharacterTooLongSignal;
         }

         for (size_t i = 0; i < len; i++) {
            dest.data[i] = data[i];
         }

         if (len < (size_t)(dest.upb().x)) {
            for (size_t i = len; i < (size_t)(dest.upb().x); i++) {
               dest.data[i] = ' ';
            }
         }
      }
   };
   /** @} */
}

#endif


