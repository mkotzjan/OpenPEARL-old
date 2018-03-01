/*
 * Lpc17xxRTC.h
 *
 *  Created on: 04.12.2015
 *      Author: r. mueller
 */
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
#ifndef LPC17XXRTC_INCLUDED_H_
#define LPC17XXRTC_INCLUDED_H_
#include <time.h>
#include <inttypes.h>

/*
extern "C" {
void vApplicationTickHook(void);
}
*/

namespace pearlrt {

   /**
      Wrapper class for LPC1768 RTC functions

      The RTC behaves a little strange. After inserting the battery
      it takes several attempts to start the rtc. After the first successful
      start in runs without problems.

      On system start, the RTC gives the current time. As long as FreeRTOS
      is running, the internal clock value is incremented with the
      system tick hook.
      The time betwenn two ticks is calculated from the systick register
      values.
   */
   class Lpc17xxRTC {
   private:
      static int ticks;    //< number of system ticks for timer trigger

      static int noTicksYet; //< flag to detect whether FreeRTOS is running
      static uint64_t tickBasedTime;

      static void setTimeOut(uint64_t nsec);

   public:
      /**
      get current time from RTC in nsec

      This method is puclic in RTC, since Post needs direct access to the RTC

      \param nsec pointer to the uint64_t value for the nanoseconds value
       */

      static void gettime(uint64_t * nsec);
      /**
       start the RTC

      */
      static void start();


      /**
       test for reasonable RTC value

       \returns true if value seems to be ok
       \returns false else
      */
      static bool valueOk(void);

      /**
       test whether RTC is running

       Read the rtc values for a maximum of 5 seconds
       and monitor the seconds. A the first change of the
       seconds value in the RTC, the RTC seems to be running

       \returns true if the RTC is running
       \returns false else
      */
      static bool isRunning(void);

      /**
      set the RTC to the given time

      \param tm  new value for the RTC
      */
      static void set(const struct tm * tm);

      /**
      register the RTC based time as system time base

      */
      static void registerTimeBase();

      /**
      increment internal time by 1ms

      To be called from vApplicationTickHook()
      */
      static void tick(void);

      /**
       enable 1s interrupt
      */
      static void enableInterrupt();
   };
}
#endif
