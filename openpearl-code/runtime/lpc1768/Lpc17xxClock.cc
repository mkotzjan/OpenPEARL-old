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
#include "Lpc17xxTimer0.h"

static void (*tickHook)(void) = pearlrt::FreeRTOSClock::tick;
static bool synchronize2RTC = false;

extern "C" {
   /**
   vApplicationTickHook

   This function is called by FrrRTOS in each timer tick.
   This implementation performs the timeout operation for the
   tick based timers in the LPC1768 port of OpenPEARL

   The concrete operations are set when the concrete time base is
   selected. This is done in the ctor of the class Lpc17xxClock.
   */
   void vApplicationTickHook(void) {
      if (tickHook) {
         // invoke the current tick hook function if one is set
         (*tickHook)();
      }
   }

   /**
   TIMER0_IRQHandler

    This function is the interruot service routine of the timer0
    of the LPC1768.
    This timer is used for clock sources with a resolution of 10 usec.
    This function is called, if timer0 is enabled, which is selected
    in the ctor of the class Lpc17xxClock.
   */
   void TIMER0_IRQHandler() {
      pearlrt::Lpc17xxTimer0::tick();
   }

   /**
   RTC_IRQHandler

   This function is the interrupt service routine of the real time clock (RTC)
   of the LPC1768.
   The RTC is used with interrupt, when the timer0 should run synchronized
   with the RTC. This is necessary in some cases to avoid large differences
   between these two clock sources.
   */
   void RTC_IRQHandler() {
      static uint64_t last_time;
      static int starting = 2;  // it takes 2 RTC interrupts to stabilize
      // the time
      static bool firstInterrupt = true;
      uint64_t now;
      int delta;

      if (firstInterrupt) {
         printf("RTC: first tick\n");
         // set the time base of Timer0 to the current time of the RTC
         // to enable the date-functions
         // now may be used for setting the current time in timer0

         pearlrt::Lpc17xxRTC::gettime(&now);
         pearlrt::Lpc17xxTimer0::settime(&now);

         pearlrt::Lpc17xxTimer0::start();
         pearlrt::Lpc17xxTimer0::gettime(&now);
      } else {
         pearlrt::Lpc17xxTimer0::gettime(&now);

         if (starting == 0 && synchronize2RTC) {
            delta = 1000000000 - (now - last_time);
            printf("RTC: tick diff to last time %d\n", delta);
            pearlrt::Lpc17xxTimer0::drift(delta);
         } else {
            starting --;
         }
      }

      firstInterrupt = false;
      last_time = now;
      LPC_RTC->ILR = 3;
   }
}

namespace pearlrt {

   bool Lpc17xxClock::clockSelected = false;

   bool Lpc17xxClock::isClockSelected() {
      return clockSelected;
   }

   Lpc17xxClock::Lpc17xxClock(const int typeOfClock) {
      uint64_t now;
      static const struct tm  defaultDate = {0, 0, 0,	// sec, min, hour,
                1, 0, 2016 - 1900,			// mday, mon, year
                5, 0, 0						// wday, yday, isdst
      }; // Fr 1.1.2016 0:00:00

      if (clockSelected) {
         Log::error("Lpc17xxClock: already a clock source selected");
         throw theInternalConfigurationSignal;
      }

      switch (typeOfClock) {
      default:
         Log::error("Lpc17xxClock: Illegal selector %d", typeOfClock);
         throw theInternalConfigurationSignal;

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
         CortexMClock::set(&defaultDate);

         // tell time.c to take the current time from this function
         // and set the tickHook function pointer to the according method
         tickHook = CortexMClock::tick;
         CortexMClock::registerTimeBase();
         break;

      case 2: // take current time from RTC
         if (Lpc17xxRTC::valueOk()) {
            printf("date/time is reasonable\n");
         } else {
            printf("ridiculous date/time\n");
            Lpc17xxRTC::set(&defaultDate);

            if (Lpc17xxRTC::valueOk()) {
               printf("date/time is reasonable now\n");
            } else {
               printf("still ridiculous date/time --> RTC dead?\n");
               exit(1);  // exit ok??
            }
         }

         if (!Lpc17xxRTC::isRunning()) {
            printf("RTC is not running --> start it\n");
            Lpc17xxRTC::start();

            if (!Lpc17xxRTC::isRunning()) {
               printf("RTC start was not successful --> RTC dead??\n");
               exit(1);  // exit ok??
            }
         }

         // tell time.c to take the current time from this function
         // and set the tickHook function pointer to the according method
         tickHook = Lpc17xxRTC::tick;
         Lpc17xxRTC::registerTimeBase();
         break;

      case 3: // Timer 0 with start at RTC seconds tick
         synchronize2RTC = false;
         tickHook = NULL;   // nothing to do Lpc17xxTimer0::tick;
         Lpc17xxTimer0::registerTimeBase();
         Lpc17xxRTC::enableInterrupt();
         break;

      case 4: // Timer 0 with start at RTC seconds tick and synchronisation
         // to RTC 1 second interrupt
         tickHook = NULL;   // nothing to do Lpc17xxTimer0::tick;
         synchronize2RTC = true;
         // set the time base of Timer0 to the current time of the RTC
         // to enable the date-functions
         // now may be used for setting the current time in timer0

         pearlrt::Lpc17xxRTC::gettime(&now);
         pearlrt::Lpc17xxTimer0::settime(&now);

         Lpc17xxTimer0::registerTimeBase();
         Lpc17xxRTC::enableInterrupt();
         break;
      }

      clockSelected = true;
   }
}

