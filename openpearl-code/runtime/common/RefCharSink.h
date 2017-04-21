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

#ifndef REFCHARSINK_H_INCLUDED
#define REFCHARSINK_H_INCLUDED


#include "Sink.h"
#include "RefChar.h"

namespace pearlrt {
   /**
   \file

   \brief internal type for data sink which operates on a ref char variable
   */

   /**
   \addtogroup io_common
   @{
   */

   /**

   \brief a Sink which operates upon a RefCharacter variable

   This class defines a sink which puts all characters in a ref char variable.
   The output of a CONVERT ... TO statement is collected by this class.
   
   */
   class RefCharSink: public Sink {
   private:
      RefCharacter* sinkObj;  //< the data object which gathers the characters

   public:
      /**
       Constructor

      \param s refchar variable  to be used as output buffer
      */

      RefCharSink(RefCharacter & s);
      /**
       send one character to the output.

      \param c the character which shall be captured by the sink

      \throws CharacterTooLongSignal if the ref char is full
      */
      void putChar(char c);

      /**
      set the current write position
     
      \param newColumn  the new position to continue writing 
      */
      void pos(size_t newColumn);
   
      /**
      get the current write position
     
      \returns the current position 
      */
      size_t sop();
   };
   /** @} */
}
#endif

