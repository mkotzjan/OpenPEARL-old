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

/**
\file

\brief class which provides the RST-Format support

*/
#include "Rst.h"
#include "Fixed.h"

namespace pearlrt {

   void Rst::rst(Fixed<15> & rst) {
      rstValue = & rst;
      rst = (Fixed<15>)0;
      rstVoidPointer = NULL;
   }

   void Rst::rst(void * rstPointer, size_t len) {
      rstVoidPointer = rstPointer;
      rstLength = len;
      /* is in Fixed.h */
      assignIntToFixedViaVoidPointer(rstVoidPointer, rstLength, 0);
      rstValue = NULL;
   }

   bool Rst::updateRst(Signal * s) {
      if (rstValue != NULL) {
         * rstValue = s->whichRST();
         return true;
      }

      if (rstVoidPointer != NULL) {
         /* is in Fixed.h */
         assignIntToFixedViaVoidPointer(rstVoidPointer, rstLength,
                                        s->whichRST());
         return true;
      }

      return false;
   }

}
