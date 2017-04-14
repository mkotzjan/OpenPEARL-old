/*
 [A "BSD license"]
 Copyright (c) 2014-2017 Rainer Mueller
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
\brief implementation of  I/O-Formats for PG-userdation and CONVERT

*/
#include "IOFormats.h"
#include "Sink.h"
#include "Source.h"
#include "Fixed.h"
#include "PutHelper.h"
#include "PutClock.h"
#include "GetClock.h"
#include "PutDuration.h"
#include "GetDuration.h"

namespace pearlrt {

   void IOFormats::setupIOFormats(Sink * _sink, Source * _source) {
     sink = _sink;
     source = _source;
   }

   void IOFormats::toT(const Clock f,
                      const Fixed<31> w,
                      const Fixed<31> d) {
      checkCapacity(w);
      PutClock::toT(f, w, d, *sink);
   }

   void IOFormats::fromT(Clock & f,
                        const Fixed<31> w,
                        const Fixed<31> d) {
      checkCapacity(w);
      GetClock::fromT(f, w, d, *source);
   }

   void IOFormats::toD(const Duration f,
                      const Fixed<31> w,
                      const Fixed<31> d) {
      checkCapacity(w);
      PutDuration::toD(f, w, d, *sink);
   }

   void IOFormats::fromD(Duration & f,
                        const Fixed<31> w,
                        const Fixed<31> d) {
      checkCapacity(w);
      GetDuration::fromD(f, w, d, *source);
   }

}
