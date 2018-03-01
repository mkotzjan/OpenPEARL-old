#ifndef GETCHARACTER_H_INCLUDED
#define GETCHARACTER_H_INCLUDED
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

/**
\file

\brief input formating  for the data type CHARACTER


\author R. Mueller

*/
#include <stdio.h>

#include "Character.h"
#include "Source.h"
#include "Signals.h"
#include "GetHelper.h"


namespace pearlrt {
   /**
   \addtogroup io_common_helper
   @{
   */

   /**
      \brief Input processing of character values.
      \tparam S the length of the Character variable
   */
   template <size_t S>
   class GetCharacter {
   public:
      /**
      read a CHAR value from the given data source.

      The input field is delimited by the parameter width and 
      additionally by EndOfLine.

      if w < S, the variable is filled with spaces at the right side

      if w > S. the input is truncated to S characters; descarding the 
             additional input

      if  w == 0; no data is taken from the input, the variable is filled
             with spaces.

      The EndOfLine is never added into the variable

      \param c the variable which gets the read value
      \param w the input field width
      \param source the input stream

      \throws  CharacterFormatSignal if w is <0 

      */
      static void fromA(Character<S> &c,
                        const Fixed<31> w,
                        Source & source) {

          GetHelper helper(w, &source);
          helper.setDelimiters(GetHelper::EndOfLine);

          RefCharacter rc;
          rc.setWork(c);

          // do all actions inside a helper method to reduce code size
          helper.readCharacterByA(&rc);
      }
   };
   /* @} */
}
#endif

