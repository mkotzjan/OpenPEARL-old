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
     This class provides RST format behavior for UserDation and CONVERT


     All exceptions in the try catch block will be handed towards
     the updateRst()-method. If the rst-formatting element was set,
     the exception is treated, else it is rethrown.

   */
   class Rst {
   protected:
      Fixed<15> * rstValue;

    public:
      /**
        set the rst variable

        \param rstVar the variable, which should be set is case of
                      exception after this call
      */
      void rst(Fixed<15> & rstVar);

      /**
         update the RST variable if set
         \param s pointer to the sigbal which was caught
         \returns true, if RST-variable was defined
         \returns false, if RST-variable was NOT defined
      */
      bool updateRst(Signal * s);
   };
}
#endif
