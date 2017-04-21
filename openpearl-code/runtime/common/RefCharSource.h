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

#ifndef REFCHARSOURCE_H_INCLUDED
#define REFCHARSOURCE_H_INCLUDED

#include <stddef.h>

#include "Source.h"
#include "RefChar.h"

namespace pearlrt {
   /**
   \file

   \brief internal type for data source which operates on a ref char variable
   */

   /**
   \addtogroup io_common
   @{
   */

   /**
   \brief a Source, which operates upon a RefCharacter variable

   This class defines a source which provides sequential access to characters
   in a in a ref char variable.
   This class is used as data source in CONVERT .. FROM statements.
   */
   class RefCharSource: public Source {
   private:
      /** the data object which contains the characters */
      RefCharacter * sourceObj;

//      size_t readPtr; //< index of next char to return
   public:
      /**
       Constructor

      \param s refchar variable  to be used as input buffer
      */
      RefCharSource(RefCharacter & s);

      /**
       get one character to the string.

      \throws NoMoreCharactersSignal, if no more characters are available
      \returns the next character from the source<br>
      */
      char realGetChar(void);

      /**
       rewind

       reset read point to start location of the RefChar

      */
      void rewind();

      /** set position 

      useful method for the implementation of the CONVERT statement
  
      \param pos  new  read position
      */
      void pos(size_t pos);

      /** get position 

      useful method for CVONVERT statement
  
      \returns current read position
      */
      size_t sop();

   };
   /** @} */
}
#endif

