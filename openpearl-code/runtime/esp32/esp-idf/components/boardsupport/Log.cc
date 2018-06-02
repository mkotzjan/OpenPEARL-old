/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
 Copyright (c) 2018 Michael Kotzjan
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

#include "Log.h"
#include "Mutex.h"
#include "Signals.h"
//#include "lpc17_interruptState.h"

namespace pearlrt {
   static Mutex mutex;

   Log::Log() {
      ctorIsActive = true;
      provider = NULL;          // no provider --> use printf
      ctorIsActive = false;
   }

   void Log::doit(const Character<7>& type,
                  const char * format,
                  va_list args) {
      Character<128> line;
      RefCharacter rc(line);
      bool usePrintf = false;

      try {
         doFormat(type, rc, format, args);

         if (!provider) {
            usePrintf = true;
         }
         /*
         if (lpc17_isInterrupt()) {
            usePrintf = true;
         }
         */
         if (!usePrintf) {
            mutex.lock();
            provider->dationWrite(rc.getCstring(), rc.getCurrent());
            mutex.unlock();
         } else {
            printf(rc.getCstring());
         }
      } catch (CharacterTooLongSignal s) {
         if (provider) {
            mutex.lock();
            provider->dationWrite(line.get(), (size_t)(line.upb().x));
            provider->dationWrite((void*)ERRORMESSAGE, strlen(ERRORMESSAGE));
            mutex.unlock();
         } else {
            printf(line.get());
         }
      }

   }


}
