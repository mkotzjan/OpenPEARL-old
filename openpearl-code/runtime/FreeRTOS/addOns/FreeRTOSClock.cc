/*
 [The "BSD license"]
 Copyright (c) 2015 Rainer Mueller
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
#include <time.h>
#include <inttypes.h>
#include <stdio.h>
#include "chip.h"
#include "time_addons.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOSClock.h"

/**
\file

\brief implementation of the time base on FreeRTOS ticks
*/

namespace pearlrt {

   int FreeRTOSClock::ticks = 0;

   void FreeRTOSClock::set(const struct tm * time) {
      struct tm t = *time; // mktime expects non const value
      tickBasedTime = mktime(&t);
      tickBasedTime  *= 1000000000;
   }

   void FreeRTOSClock::registerTimeBase() {
      register_timer_source(setTimeOut, gettime);
   }

   // local time, after FreeRTOS was started
   uint64_t FreeRTOSClock::tickBasedTime;

   void FreeRTOSClock::setTimeOut(uint64_t nsec) {
      static const uint64_t nsPerSec = 1000000000;
      uint64_t t;

      // for clearance let's assume:
      //         nsec = 1234567890 = approx 1.2sec
      //         configTICK_RATE_HZ = 500  (2ms/TICK)
      // if we would calculate in float:
      //    number of ticks = Duration_in_seconds * configTICK_RATE_HZ
      //                    = nsec / nsPerSec * configTICK_RATE_HZ
      // and perform rounding to next tick
      //                    = nsec / nsPerSec * configTICK_RATE_HZ + 0.999999
      //  ticks = 617.28... must be rounded to 618 ticks
      // but we perform the calculation with integers
      //   --> multiply first and divide at last
      // numbers me become very large: uint64_t run up to 1.8E19
      // --> 317 years; that's enough!
      t = (nsec * configTICK_RATE_HZ + nsPerSec - 1) / nsPerSec;

      ticks = t;
//printf("FreeRTOSClock: armed after %d ticks\n", ticks);
   }

   void FreeRTOSClock::gettime(uint64_t *nsec) {
      *nsec = tickBasedTime;
   }

   void FreeRTOSClock::tick(void) {
      // increment the tick based time on each FreeRTOS-Tick
      tickBasedTime += 1000000L;  // one milli second

      if (ticks) {
         ticks --;

         if (! ticks) {
            resume_timer_task_from_ISR();
         }
      }
   }

}
extern "C" {
   /**
      select the FreeRTOSClock as clock source

      This function is used in timer.c to perform the default
      timer configuration.
   */
   void selectFreeRTOSClock() {
      pearlrt::FreeRTOSClock::registerTimeBase();
   }
}
