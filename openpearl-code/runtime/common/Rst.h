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

#ifndef RST_INCLUDED
#define RST_INCLUDED

/**
\file

\brief class to support RST-format

*/

#include "Signals.h"
#include "Fixed.h"

namespace pearlrt {
   /**
   \addtogroup io_common
   @{
   */

   /**
   \brief This class provides RST format behavior for UserDation and CONVERT


     All exceptions in the try catch block of an i/o or CONVERT statement
     will be handed towards the updateRst()-method.
     If the rst-formatting element was set,
     the exception is treated, else it is rethrown.

   */
   class Rst {
   protected:
      /**
      pointer to error variable.

      If this pointer is NULL, no rst variable is set.
      This attribute is preset with NULL in the start of the i/o
      operation.
      */
      Fixed<15> * rstValue;

      /**
      pointer to the error variable with the IOJob interface

      If this pointer is NULL, no rst variable is set.
      This attribute is preset with NULL in the start of the i/o
      operation.
      */
      void * rstVoidPointer;

      /**
      the length of the rst variable.
      To avoid assignement problems of a too large error number,
      it is recommended that the error variable is at least of
      the type FIXED(15). 
      */
      size_t rstLength;

   public:
      /**
        set the rst variable

        \param rstVar the variable, which should be set is case of
                      exception after this call
      */
      void rst(Fixed<15> & rstVar);

      /**
        set the rst variable via an IOJob

        \param rstVoidPointer pointer the variable,
                      which should be set is case of
                      exception after this call
        \param len the length of the FIXED-variable
      */
      void rst(void * rstVoidPointer, size_t len);

      /**
         update the RST variable if set

         This method is intended to be called in the exception handler
         of an i/o statement. If an rst-variable is active,
         its value is set by the number of the given signal

         \param s pointer to the signal which was caught
         \returns true, if RST-variable was defined
         \returns false, if RST-variable was NOT defined
      */
      bool updateRst(Signal * s);
   };

   /** @} */

}
#endif
