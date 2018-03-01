#ifndef PUTHELPER_H_INCLUDED
#define PUTHELPER_H_INCLUDED
/*
 [A "BSD license"]
 Copyright (c) 2012-2016 Rainer Mueller
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
#include <stdio.h>

#include "Sink.h"
#include "RefChar.h"
#include "Float.h"

/**
\file

\brief output formating  helper object


\author R. Mueller

*/

namespace pearlrt {
   /**
     \addtogroup io_common_helper
     @{
  */

  /**
     \brief Helper class providing utility functions for output format
     processing.

     This class provides several helper routines for PUT-processing.
     This should reduce  the program size for the templated operations
     with Character types.
     This helps primarily to reduce the code footprint.

   */
   class PutHelper {

   public:

      /**
         perform the A-format

         All action is placed in this routine to reduce memory
         footprint.
         Only the wrapper towards RefChar is templated

         \param length length of the output field
         \param rc RefChar element wrapping the Character variable to be
                   sent to the output
         \param s target object to receive the output data
         \throws signal in case of problems
      */
      static void doPutChar(int length, RefCharacter* rc, Sink * s);

      /**
      the values of the exponents with only one bit set
      */
      const static Float<53> binExpValues[];

      /**
      the length of the array binExpValues
      */
      const static int nbrBinExpValues;

   };

   /** @} */
}
#endif

