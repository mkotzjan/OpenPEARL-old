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

/**
\file

\brief put-formatting of types FIXED(x)

\author R. Mueller

This module contains the output formatting of FIXED F(w,d,s)-format
*/

#ifndef PUTCHARACTER_H_INCLUDED
#define PUTCHARACTER_H_INCLUDED


#include <stdint.h>
#include <stdio.h>
#include "Fixed.h"
#include "Character.h"
#include "Sink.h"
#include "PutHelper.h"

using namespace std;
namespace pearlrt {

   /**
   \addtogroup io_common_helper
   @{
   */

   /**
   \brief Class containing the output formatting of character variables


   \tparam S the size of the Character variable
   */
   template<size_t S>
   class PutCharacter {
   private:
      /**
       no constructor needed, since the only static methods
       are provided. They need no objects.
      */
      PutCharacter() {}

   public:

      /**
      convert the character value into the requested output format

      width denoting the total width of the output field<br>

      The value will be set left adjusted into the output field,
      if w > S.

      \param charValue the data to be formatted
      \param w the width of the output field.
      \param sink the destination for the generated character sequence
      \throws CharFormatSignal if format
                parameters are illegal (w<0)
      */
      static void toA(
         Character<S> &charValue,
         const Fixed<31> w,
         Sink & sink) {
         RefCharacter rc;
         rc.setWork(charValue);
         // performs checks and output
         PutHelper::doPutChar(w.x, &rc, &sink);
      }


      /**
      convert the character value into the requested output format

      width is derived from the character variable

      \param charValue the data to be formatted
      \param sink the destination for the generated character sequence
      \return rst value of  CharFormatSignal if format
               parameters are illegal (w<0)
      */
      static void toA(
         Character<S> &charValue,
         Sink & sink) {
         toA(charValue, S, sink);
      }

   };
   /** @} */
}
#endif

