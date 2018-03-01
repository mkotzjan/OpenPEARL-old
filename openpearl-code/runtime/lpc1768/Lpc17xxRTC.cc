/*
 * lpc17_rtc.c
 *
 *  Created on: 28.04.2015
 *      Author: quitte
 *
 * translated to C++ and restructured  by r. mueller
 */
/*
 [The "BSD license"]
 Copyright (c) 2015 Jonas Meyer
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
#include <stdio.h>
#include <inttypes.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "chip.h"
#include "time_addons.h"
#include "Lpc17xxRTC.h"

/* access the systick registers directly */
#define portNVIC_SYSTICK_LOAD_REG               \
                        ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG      \
                        ( * ( ( volatile uint32_t * ) 0xe000e018 ) )

namespace pearlrt {

   static void rtc2struct_tm(const RTC_TIME_T * rtc_tm, struct tm * tm);
   static void struct_tm2rtc(const struct tm * tm, RTC_TIME_T * rtc_tm);

   void Lpc17xxRTC::start(void) {
      Chip_RTC_Init(LPC_RTC);
      Chip_RTC_Enable(LPC_RTC, ENABLE);
   }

   bool Lpc17xxRTC::isRunning(void) {
      RTC_TIME_T t;
      unsigned int startSecond;
      int i;
      bool isRunning = 0;

      // test if it really runs
      //  read current time and loop for approximatelly of 5 seconds and
      //  monitor the change of the seconds value
      //  report the number of loop cycles for 1 second
      Chip_RTC_GetFullTime(LPC_RTC, &t);
      startSecond = t.time[RTC_TIMETYPE_SECOND];

      for (i = 0; i < 183000 * 5 && !isRunning ; i++) {
         Chip_RTC_GetFullTime(LPC_RTC, &t);

         if (startSecond != t.time[RTC_TIMETYPE_SECOND]) {
            //printf("Lpx17xxRTC: seconds tick at i=%d\n", i);
            startSecond = t.time[RTC_TIMETYPE_SECOND];
            isRunning = 1; // return ok
         }
      }

      return isRunning;
   }

   bool Lpc17xxRTC::valueOk(void) {
      RTC_TIME_T rtc_tm;
      unsigned int testval;

      Chip_RTC_GetFullTime(LPC_RTC, &rtc_tm);
      /*
      printf("RTC TIME: %lu.%lu.%lu %lu:%lu:%lu dow =%lu doy=%lu\n",
             rtc_tm.time[RTC_TIMETYPE_DAYOFMONTH],
             rtc_tm.time[RTC_TIMETYPE_MONTH],
             rtc_tm.time[RTC_TIMETYPE_YEAR],
             rtc_tm.time[RTC_TIMETYPE_HOUR],
             rtc_tm.time[RTC_TIMETYPE_MINUTE],
             rtc_tm.time[RTC_TIMETYPE_SECOND],
             rtc_tm.time[RTC_TIMETYPE_DAYOFWEEK],
             rtc_tm.time[RTC_TIMETYPE_DAYOFYEAR]);
       */

      testval = rtc_tm.time[RTC_TIMETYPE_SECOND];

      if (testval > 59) {
         return false;
      }

      testval = rtc_tm.time[RTC_TIMETYPE_MINUTE];

      if (testval > 59) {
         return false;
      }

      testval = rtc_tm.time[RTC_TIMETYPE_HOUR];

      if (testval > 23) {
         return false;
      }

      testval = rtc_tm.time[RTC_TIMETYPE_DAYOFMONTH];

      if (testval > 31) {
         return false;
      }

      testval = rtc_tm.time[RTC_TIMETYPE_MONTH];

      if (testval > 11) {
         return false;
      }

      testval = rtc_tm.time[RTC_TIMETYPE_YEAR];

      if (testval < 2014  || testval > 2050) {
         return false;
      }

      testval = rtc_tm.time[RTC_TIMETYPE_DAYOFWEEK];

      if (testval > 6) {
         return false;
      }

      testval = rtc_tm.time[RTC_TIMETYPE_DAYOFYEAR];

      if (testval > 366) {
         return false;
      }

      // no bad value detected
      return true;
   }

   void Lpc17xxRTC::set(const struct tm * time) {
      RTC_TIME_T rtc_time;
      struct_tm2rtc(time, &rtc_time);
      Chip_RTC_SetFullTime(LPC_RTC, &rtc_time);
   }

   void Lpc17xxRTC::registerTimeBase() {
      register_timer_source(setTimeOut, gettime);
   }

   /**
     convert unix struct tm into rtc registers and
     adjust values as required
    */
   static void struct_tm2rtc(const struct tm * tm, RTC_TIME_T * rtc_tm) {
      rtc_tm->time[RTC_TIMETYPE_SECOND]	= tm->tm_sec;
      rtc_tm->time[RTC_TIMETYPE_MINUTE]	= tm->tm_min;
      rtc_tm->time[RTC_TIMETYPE_HOUR]	= tm->tm_hour;
      rtc_tm->time[RTC_TIMETYPE_DAYOFMONTH] = tm->tm_mday;
      rtc_tm->time[RTC_TIMETYPE_MONTH]	= tm->tm_mon + 1;
      rtc_tm->time[RTC_TIMETYPE_YEAR]	= tm->tm_year + 1900;
      rtc_tm->time[RTC_TIMETYPE_DAYOFWEEK] = tm->tm_wday;
      rtc_tm->time[RTC_TIMETYPE_DAYOFYEAR] = tm->tm_yday + 1;
   }

   /**
     convert rtc registers into unix struct tm and
     adjust values as required
   */
   static void rtc2struct_tm(const RTC_TIME_T * rtc_tm, struct tm * tm) {
      tm->tm_sec   = rtc_tm->time[RTC_TIMETYPE_SECOND];
      tm->tm_min   = rtc_tm->time[RTC_TIMETYPE_MINUTE];
      tm->tm_hour  = rtc_tm->time[RTC_TIMETYPE_HOUR];
      tm->tm_mday  = rtc_tm->time[RTC_TIMETYPE_DAYOFMONTH];
      tm->tm_mon   = rtc_tm->time[RTC_TIMETYPE_MONTH] - 1;
      tm->tm_year  = rtc_tm->time[RTC_TIMETYPE_YEAR] - 1900;
      tm->tm_wday  = rtc_tm->time[RTC_TIMETYPE_DAYOFWEEK];
      tm->tm_yday  = rtc_tm->time[RTC_TIMETYPE_DAYOFYEAR] - 1;
   }

   int Lpc17xxRTC::noTicksYet = 1;
   int Lpc17xxRTC::ticks = 0;

   ///  local time, after FreeRTOS was started
   uint64_t Lpc17xxRTC::tickBasedTime;


   void Lpc17xxRTC::setTimeOut(uint64_t nsec) {
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
//printf("Lpc17xxRTC: armed after %d ticks\n", ticks);
   }

   void Lpc17xxRTC::gettime(uint64_t * ns) {
      RTC_TIME_T time;
      struct tm tm;
      static const uint64_t  nsPerSec = 1000000000L;

      if (noTicksYet) {
         Chip_RTC_GetFullTime(LPC_RTC, &time);
         rtc2struct_tm(&time, &tm);
         *ns = mktime(&tm) * nsPerSec;
      } else {
         *ns = tickBasedTime;

         // fetch sub milli seconds from the systick timer
         if (portNVIC_SYSTICK_LOAD_REG != 0) {
            double tickRatio;
            tickRatio =  portNVIC_SYSTICK_CURRENT_VALUE_REG;
            tickRatio /= portNVIC_SYSTICK_LOAD_REG;
            *ns += (1 - tickRatio) * 1000000; // 1 ms = 1000000 ns
         }
      }

//printf("RTC::gettime returns %"PRIu64" \n", *ns);

      /*
      printf("Lpc17ccRTC::gettime returns: %ld:%ld\n",
             ts->tv_sec, ts->tv_nsec);
      */
   }

   void Lpc17xxRTC::enableInterrupt() {
      LPC_RTC->CIIR = 1; //interrupt on second increment
      //TODO: double check
      LPC_RTC->RTC_AUXEN = 0; //disable the oscillator fail flag
      LPC_RTC->ILR = 3; //clear interrupt flags
      //   NVIC->ISER[0] = (1<<RTC_IRQn);
      NVIC_SetPriority(RTC_IRQn, 5);  // best prio to get best time base
      NVIC_EnableIRQ(RTC_IRQn);
   }

   void Lpc17xxRTC::tick(void) {
      if (noTicksYet) {
         // read current time from RTC
         gettime(&tickBasedTime);  // read RTC as time base

         // set flag to use the software increment from now on
         noTicksYet = 0;
      } else {
         // increment the tick based time on each FreeRTOS-Tick
         tickBasedTime += 1000000L;  // one milli second
      }

      if (ticks) {
         ticks --;

         if (!ticks) {
            resume_timer_task_from_ISR();
         }
      }

   }

}
