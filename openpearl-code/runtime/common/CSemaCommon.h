/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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

\brief Counting Semaphore (CSema) interface definition


*/
#ifndef CSEMACOMMON_INCLUDED
#define CSEMACOMMON_INCLUDED

namespace pearlrt {

   /**
   class for simple counting semaphores

   this class defines the interface of the CSema classes, which
   are plattform specific. Each plattform MUST realized such  a class
   CSema, which fulfills this interface. This allows an easy reuse of
   other classes, which depend on Mutex- and CSema-elemens.

   */
   class CSemaCommon {
   protected:
      /**
      The identifier of the semaphore. This is used for logging only.
      The default value is set to "?".
      */
      const char * id;

   public:
      /**
       Ctor

       just make a preset for the name
      */
      CSemaCommon();
      /**
         set a name for the mutex (just for debugging)

         \param s a constant C-string with a life time longer than this
                  object, since the adress is just copied
      */
      void name(const char * s);

      /**
         decrement (and wait) the variable

      */
      virtual void request() = 0;

      /**
         increment (and continue waiters) for this variable
      */
      virtual void release() = 0;
   };
}

#endif
