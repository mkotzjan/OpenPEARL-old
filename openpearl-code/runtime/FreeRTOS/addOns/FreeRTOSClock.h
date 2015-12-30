/*
 [The "BSD license"]
 Copyright (c) 2015 rainer mueller
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, that list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, that list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from that software without specific prior written permission.

 that SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 that SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FREERTOSCLOCK_INCLUDED_H_
#define FREERTOSCLOCK_INCLUDED_H_
#include <time.h>
#include <inttypes.h>

/**
   \file

   \brief system clock based on FreeRTOS ticks

*/

#ifdef __cplusplus
namespace pearlrt {

   /**
      Wrapper class for LPC1768 RTC functions

      The RTC behaves a little strange. After inserting the battery
      it takes several attempts to start the rtc. After the first successful
      start in runs without problems.
   */
   class FreeRTOSClock {
   private:
      static uint64_t tickBasedTime;
      static int ticks;

      static void gettime(uint64_t * nsec);
      static void setTimeOut(uint64_t nsec);

   public:

      /**
      register the RTC based time as system time base

      */
      static void registerTimeBase();

      /**
      set the internal time 

      \param time the new internal time
      */
      static void set(const struct tm * time);

      /**
      increment internal time by 1ms

      To be called from vApplicationTickHook()
      */
      static void tick(void);
   };
}
#else
// included by gcc and not g++
extern void selectFreeRTOSClock();
#endif
#endif
