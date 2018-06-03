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

\brief access to posix counting semaphores

\author R. Mueller
*/
#ifndef CSEMA_INCLUDED
#define CSEMA_INCLUDED

#include "CSemaCommon.h"
#include "FakeTypes.h"

namespace pearlrt {

   /**
   \brief access to posix counting semaphores

   class for counting semaphore

   this class is based of the pthread sema

   */
   class CSema : public CSemaCommon {
   private:
      static const FakeUBaseType_t max = 32767;

      FakeSemaphoreHandle_t sem;
      const char * id;

   public:
      /**
         initialize the semaphore variable
         \param preset initial value - must be >= 0
      */
      CSema(int preset = 0);

      /**
         set a name for the semaphore (just for debugging)

         \param s C-string with max 39 characters
      */
      void name(const char * s);

      /**
          remove the mutex
      */
      ~CSema();

      /**
         request the semaphore (Djikstra's P-operation)
      */
      void request();

      /**
         release the semaphore variable  (Djikstra'S V-operation)
      */
      void release();
   };
}

#endif
