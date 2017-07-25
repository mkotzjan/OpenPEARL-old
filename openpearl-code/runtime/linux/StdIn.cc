/*
 [A "BSD license"]
 Copyright (c) 2013-2014 Holger Koelle
 Copyright (c) 2014-2016 Rainer Mueller
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
\brief Implementation of generic non-Basic Linux Systemdation

*/

#include "StdIn.h"
#include "Character.h"
#include "RefChar.h"
#include "Dation.h"
#include "Log.h"
#include "Signals.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace pearlrt {
   bool StdIn::_isDefined = false;

   StdIn::StdIn() : SystemDationNB() {
      /* ctor is called before multitasking starts --> no mutex required */
      mutex.name("StdIn");
      inUse = false;
      cap = FORWARD;
      cap |= PRM;
      cap |= ANY;
      cap |= IN;
      fp = stdin;

      _isDefined = true;
   }

   int StdIn::capabilities() {
      return cap;
   }

   StdIn* StdIn::dationOpen(const char * idf, int openParams) {
      if (openParams & (Dation::IDF | Dation::CAN)) {
         Log::error("StdIn: does not support IDF and CAN");
         throw theDationParamSignal;
      }

      mutex.lock();
      inUse = true;
      mutex.unlock();
      return this;
   }

   void StdIn::dationClose(int closeParams) {
      int ret;

      //
      mutex.lock();
      inUse = true;
      ret = fflush(fp);

      if (ret != 0) {
         Log::error("StdIn: error at close (%s)", strerror(ferror(fp)));
         mutex.unlock();
         throw theCloseFailedSignal;
      }

      if (closeParams & Dation::CAN) {
         Log::error("StdIn: CAN not supported");
         mutex.unlock();
         throw theDationParamSignal;
      }

      mutex.unlock();
   }

   void StdIn::dationRead(void * destination, size_t size) {
      int ret;
      mutex.lock();
      clearerr(fp);
      errno = 0;
      ret = fread(destination, size, 1, fp);

      if (ret < 1) {
         if (feof(fp)) {
            Log::error("StdIn: error across EOF");
            mutex.unlock();
            throw theDationEOFSignal;
         }

         Log::error("StdIn: error at read (%s)", strerror(errno));
         mutex.unlock();
         throw theReadingFailedSignal;
      }

      mutex.unlock();
   }


   void StdIn::dationWrite(void * source, size_t size) {
         Log::error("StdIn: write is not supported");
         throw theDationNotSupportedSignal;
   }

   void StdIn::dationUnGetChar(const char x) {
      mutex.lock();
      ungetc(x, fp);
      mutex.unlock();
   }



   bool StdIn::isDefined() {
      return (_isDefined);
   }


   void StdIn::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }

}
