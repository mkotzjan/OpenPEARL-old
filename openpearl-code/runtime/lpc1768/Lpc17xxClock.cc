/*
 [The "BSD license"]
 Copyright (c) 2015 Rainer Mueller
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
\brief Implementation of clock source for the LPC1768 Landtiger board

*/

#include "Log.h"
#include <time.h>
//#include <cstdarg>
#include "chip.h"
#include "time_addons.h"
#include "Lpc17xxClock.h"
#include "Lpc17xxRTC.h"
#include "FreeRTOSClock.h"
#include "CortexMClock.h"

static void (*tickHook)(void) = pearlrt::FreeRTOSClock::tick;

extern "C" {
   void vApplicationTickHook(void) {
      if (tickHook) {
         // invoke the current tick hook function if one is set
         (*tickHook)();
      }
   }
}

namespace pearlrt {

   Lpc17xxClock::Lpc17xxClock(const int typeOfClock) {
      Lpc17xxRTC rtc;
      CortexMClock cortexMClock;
      static const struct tm  defaultDate = {0, 0, 0,
                1, 0, 2016 - 1900,
                5, 0, 0
      }; // 1.1.2016 0:00:00

      switch (typeOfClock) {
      default:
         Log::error("Lpc17xxClock: Illegale selector %d", typeOfClock);
         throw theIllegalParamSignal;

      case 0: // only systick
              // no absolute time from RTC desired
         
         // lets assume to be midnight of 1.1.2016 right now
         FreeRTOSClock::set(&defaultDate);

         // tell time.c to take the current time from this function
         // and set the tickHook function pointer to the according method
         tickHook = FreeRTOSClock::tick;
         FreeRTOSClock::registerTimeBase();
         break;

      case 1: // only systick and subsystick time
              // no absolute time from RTC desired
         
         // lets assume to be midnight of 1.1.2016 right now
         cortexMClock.set(&defaultDate);

         // tell time.c to take the current time from this function
         // and set the tickHook function pointer to the according method
         tickHook = CortexMClock::tick;
         cortexMClock.registerTimeBase();
         break;

      case 2: // take current time from RTC
         if (rtc.valueOk()) {
            printf("date/time is reasonable\n");
         } else {
            printf("ridiculous date/time\n");
            rtc.set(&defaultDate);

            if (rtc.valueOk()) {
               printf("date/time is reasonable now\n");
            } else {
               printf("still ridiculous date/time --> RTC dead?\n");
               exit(1);  // exit ok??
            }
         }

         if (!rtc.isRunning()) {
            printf("RTC is not running --> start it\n");
            rtc.start();

            if (!rtc.isRunning()) {
               printf("RTC start was not successful --> RTC dead??\n");
               exit(1);  // exit ok??
            }
         }

         // tell time.c to take the current time from this function
         // and set the tickHook function pointer to the according method
         tickHook = Lpc17xxRTC::tick;
         rtc.registerTimeBase();
         break;
      }
   }
}

