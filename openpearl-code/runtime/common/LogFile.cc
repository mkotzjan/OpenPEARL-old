/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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
\brief Implementation of the LogFile configuration element

*/

#include "LogFile.h"
#include "Dation.h"
#include "Signals.h"
#include "Log.h"

namespace pearlrt {

   LogFile::LogFile(SystemDationNB * _provider, const char * fileName) :
      SystemDationNB() {

      if (_provider == NULL) {
         Log::error("LogFile: No provider given");
         throw theIllegalParamSignal;
      }

      if (fileName == NULL) {
         Log::error("LogFile: No file name given");
         throw theIllegalParamSignal;
      }

      provider = _provider;
      logFileName = fileName;
   }

   int LogFile::capabilities() {
      return 0;  // dummy implementation, never called
   }

   LogFile* LogFile::dationOpen(const char * idf, int openParams) {
printf("logfile op=%d  provider=%p\n", openParams, provider);
      if (openParams) {
         Log::warn("LogFile: openParams ignored");
      }

printf(" delegate open \n");
      provider = provider->dationOpen(logFileName,
                                      Dation::OUT | Dation::IDF | Dation::ANY);
printf("logfile .... provider=%p\n",provider);

      return this;
   }

   void LogFile::dationClose(int closeParams) {

      provider->dationClose(Dation::PRM);

   }

   void LogFile::dationRead(void * destination, size_t size) {
      Log::error("LogFile::dationRead is not supported");
      throw theInternalDationSignal;
   }


   void LogFile::dationWrite(void * source, size_t size) {
printf("LogFile: delegate write to %p\n", provider); 
      provider->dationWrite(source, size);
   }

   void LogFile::dationUnGetChar(const char x) {
      Log::error("LogFile::dationUnGetChar is not supported");
      throw theInternalDationSignal;
   }

}
