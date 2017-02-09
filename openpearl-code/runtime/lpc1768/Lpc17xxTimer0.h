/*
 * Lpc17xxTimer0.h
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
#ifndef LPC17XXTIMER0_INCLUDED_H_
#define LPC17XXTIMER0_INCLUDED_H_
#include <time.h>
#include <inttypes.h>

namespace pearlrt {

   /**
      Wrapper class for LPC1768 Timer0 functions

      On system start, the RTC gives the current time.
      The TIMER0 is configured to trigger an interrupt each second
      as well as at the end of a requested time period.

      The time betwenn two ticks is calculated from the systick register
      values.
   */
   class Lpc17xxTimer0 {
   private:
      static uint64_t nsec;
      static uint32_t ticksPerSecond;
      static uint32_t newTicksPerSecond;

      static void setTimeOut(uint64_t nsec);

      static unsigned int ticks2nsec(unsigned int ticks);
      static unsigned int nsec2ticks(unsigned int nsecs);

   public:
      /**
       start the timer

      Configure the timer to produce an interrupt each second.
      */
      static void start();

      /**
      receive the drift to time reference

      change the time base to meet th time reference better
      \param drift nsec difference to reference clock in a 1 seconds period
      */
      static void drift(int drift);

      /**
      register the Timer0 based time as system time base

      */
      static void registerTimeBase();

      /**
      read time in nsecs

      \param nsec pointer to the storage which shoul receive the current time
                     in units of ns
      */
      static void gettime(uint64_t * nsec);

      /**
      write current time in nsecs

      \param nsec pointer to the storage which contains the current time
                     in units of ns
      */
      static void settime(const uint64_t * nsec);

      /**
      To be called from vApplicationTickHook()

      ... does nothing useful ...
      */
      static void tick(void);
   };
}
#endif
