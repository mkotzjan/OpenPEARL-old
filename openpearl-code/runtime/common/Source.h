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

#ifndef SOURCE_H_INCLUDED
#define SOURCE_H_INCLUDED


namespace pearlrt {
   /**
   \file

   \brief internal type for data source (used by formatting routines)

   The formatting routines need a interface to receive the input
   streaming characters.
   This stream may be something as a file
   or a string. This class defines the interface for all possible
   input sources.

   */

   /**
   This class defines an interface for input characters to be used in the
   formatting routines for the individual data types.
   */
   class Source {
   private:
      char unGetCharacter;
      bool unGetCharacterAvailable;
   public:
      /**
      constructor

      initialize private data
      */
      Source();

      /**
       take one character from the input.

       Deliver an unget()-character is available, else return the next
       character from the real source object.

       \throws NoMoreCharactersSignal in case of problems reading new data

      \return the character to be treated <br>
               the encoding of the character is defined by the system
      */
      char getChar(void);

      /**
       take one character from the real source object.

       The concrete implementation may decide to throw exceptions
       in case of trouble.

      \return the character to be treated<br>
               the encoding of the character is defined by the system
      */
      virtual char realGetChar(void) = 0;

      /**
       unget one character from the input.

       This routine is similar to the unget()-Function in stdio.h

       Only 1 character may be ungeted in sequence.

      \param c the character to be ungetted
      */
      void unGetChar(char c);

      /**
      forget an ungot character

      discard the character from the unget buffer, if there is one.
      Else no nothing.


      This operation is needed, while working with ALPHIC operations
      on a DIRECT DATION. The seek-operation must discard this character.
      If this would not happen, the last unGetCharacter would be returned
      after any positioning.
      */
      void forgetUnGetChar(void);

   };
}
#endif

