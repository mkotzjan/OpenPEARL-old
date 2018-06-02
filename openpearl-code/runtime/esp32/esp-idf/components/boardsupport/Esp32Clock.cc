/*
 [The "BSD license"]
 Copyright (c) 2015 Rainer Mueller
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

/**
\file
\brief Implementation of clock source for the ESP32 board

*/

#include "Log.h"
#include <time.h>
//#include <cstdarg>
#include "FreeRTOS.h"
#include "timers.h"
#include "time_addons.h"
#include "Esp32Clock.h"
//#include "Esp32RTC.h"
#include "FreeRTOSClock.h"
//#include "CortexMClock.h"
//#include "Esp32Timer0.h"

static void (*tickHook)(void) = pearlrt::FreeRTOSClock::tick;
//static bool synchronize2RTC = false;

extern "C" {
   /**
   vApplicationTickHook

   This function is called by FreeRTOS in each timer tick.
   This implementation performs the timeout operation for the
   tick based timers in the ESP32 port of OpenPEARL

   The concrete operations are set when the concrete time base is
   selected. This is done in the ctor of the class Esp32Clock.
   */
   void vApplicationTickHook(void *) {
      if (tickHook) {
         // invoke the current tick hook function if one is set
         (*tickHook)();
      }
   }

   /**
   TIMER0_IRQHandler

    This function is the interrupt service routine of the timer0
    of the ESP32.
    This timer is used for clock sources with a resolution of 10 usec.
    This function is called, if timer0 is enabled, which is selected
    in the ctor of the class Esp32Clock.
   */
   /*
   void TIMER0_IRQHandler() {
      pearlrt::Esp32Timer0::tick();
   }
   */

   /**
   RTC_IRQHandler

   This function is the interrupt service routine of the real time clock (RTC)
   of the ESP32.
   The RTC is used with interrupt, when the timer0 should run synchronized
   with the RTC. This is necessary in some cases to avoid large differences
   between these two clock sources.
   */
   /*
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

         pearlrt::Esp32RTC::gettime(&now);
         pearlrt::Esp32Timer0::settime(&now);

         pearlrt::Esp32Timer0::start();
         pearlrt::Esp32Timer0::gettime(&now);
      } else {
         pearlrt::Esp32Timer0::gettime(&now);

         if (starting == 0 && synchronize2RTC) {
            delta = 1000000000 - (now - last_time);
            printf("RTC: tick diff to last time %d\n", delta);
            pearlrt::Esp32Timer0::drift(delta);
         } else {
            starting --;
         }
      }

      firstInterrupt = false;
      last_time = now;
      //LPC_RTC->ILR = 3;
   }
   */
}

namespace pearlrt {

   bool Esp32Clock::clockSelected = false;

   bool Esp32Clock::isClockSelected() {
      return clockSelected;
   }

   Esp32Clock::Esp32Clock(const int typeOfClock) {
      //uint64_t now;
      static const struct tm  defaultDate = {0, 0, 0,	// sec, min, hour,
                1, 0, 2016 - 1900,			// mday, mon, year
                5, 0, 0						// wday, yday, isdst
      }; // Fr 1.1.2016 0:00:00

      if (clockSelected) {
         Log::error("Esp32Clock: already a clock source selected");
         throw 0;
         //throw theInternalConfigurationSignal;
      }

      switch (typeOfClock) {
      default:
         Log::error("Esp32Clock: Illegal selector %d", typeOfClock);
         throw 0;
         //throw theInternalConfigurationSignal;

      case 0: // only systick
         // no absolute time from RTC desired

         // lets assume to be midnight of 1.1.2016 right now
         FreeRTOSClock::set(&defaultDate);

         // tell time.c to take the current time from this function
         // and set the tickHook function pointer to the according method
         tickHook = FreeRTOSClock::tick;
         FreeRTOSClock::registerTimeBase();
         //xTimerCreate("OpenPEARLTimer", 1, pdTRUE, (void *) 0, vApplicationTickHook);
         break;
/*
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
         if (Esp32RTC::valueOk()) {
            printf("date/time is reasonable\n");
         } else {
            printf("ridiculous date/time\n");
            Esp32RTC::set(&defaultDate);

            if (Esp32RTC::valueOk()) {
               printf("date/time is reasonable now\n");
            } else {
               printf("still ridiculous date/time --> RTC dead?\n");
               exit(1);  // exit ok??
            }
         }

         if (!Esp32RTC::isRunning()) {
            printf("RTC is not running --> start it\n");
            Esp32RTC::start();

            if (!Esp32RTC::isRunning()) {
               printf("RTC start was not successful --> RTC dead??\n");
               exit(1);  // exit ok??
            }
         }

         // tell time.c to take the current time from this function
         // and set the tickHook function pointer to the according method
         tickHook = Esp32RTC::tick;
         Esp32RTC::registerTimeBase();
         break;

      case 3: // Timer 0 with start at RTC seconds tick
         synchronize2RTC = false;
         tickHook = NULL;   // nothing to do Esp32Timer0::tick;
         Esp32Timer0::registerTimeBase();
         Esp32RTC::enableInterrupt();
         break;

      case 4: // Timer 0 with start at RTC seconds tick and synchronisation
         // to RTC 1 second interrupt
         tickHook = NULL;   // nothing to do Esp32Timer0::tick;
         synchronize2RTC = true;
         // set the time base of Timer0 to the current time of the RTC
         // to enable the date-functions
         // now may be used for setting the current time in timer0

         pearlrt::Esp32RTC::gettime(&now);
         pearlrt::Esp32Timer0::settime(&now);

         Esp32Timer0::registerTimeBase();
         Esp32RTC::enableInterrupt();
         break;
*/
      }

      clockSelected = true;
   }
}

