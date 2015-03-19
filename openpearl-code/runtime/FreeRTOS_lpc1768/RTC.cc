/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
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

/*
 * Date.cc
 *
 *  Created on: 03.05.2014
 *      Author: florian
 * migration to RTC-modul doen by r. mueller
 */


#include "RTC.h"
#include "Log.h"
#include "chip.h"    // LpcOpen

namespace pearlrt {

   int64_t RTC::getUnixTimeStamp(int year, int month, int day,
                                 int hour, int minute, int second) {
      int64_t unixTime = 0;
      const int16_t daysSinceNewYear[12] = { 0, 31, 59, 90, 120, 151, 181,
                                             212, 243, 273, 304, 334
                                           };
      int64_t years = year - 1970;
      int64_t leapYears = ((year - 1) - 1968) / 4 -
                          ((year - 1) - 1900) / 100 +
                          ((year - 1) - 1600) / 400;
      unixTime = second +
                 60 * minute +
                 60 * 60 * hour +
                 (daysSinceNewYear[month - 1] + day - 1) * 60 * 60 * 24 +
                 (years * 365 + leapYears) * 60 * 60 * 24;

      if ((month > 2) &&
            (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
         unixTime += 60 * 60 * 24;
      }

      return unixTime;
   }


   void RTC::getTimeOfDay(timeval* tv) {
      uint64_t unixTime = 0;
      RTC_TIME_T tm = {0};
      Chip_RTC_GetFullTime(LPC_RTC, &tm);
      unixTime = getUnixTimeStamp(tm.time[RTC_TIMETYPE_YEAR],
                                  tm.time[RTC_TIMETYPE_MONTH],
                                  tm.time[RTC_TIMETYPE_DAYOFMONTH],
                                  tm.time[RTC_TIMETYPE_HOUR],
                                  tm.time[RTC_TIMETYPE_MINUTE],
                                  tm.time[RTC_TIMETYPE_SECOND]);
      // time_t ~ long int
      // suseconds_t ~ long int
      tv->tv_sec = unixTime;
      tv->tv_usec = 0;
   }


   void RTC::init() {
      RTC_TIME_T time;
      Chip_RTC_Init(LPC_RTC);
      Chip_RTC_GetFullTime(LPC_RTC, &time);

      if (time.time[RTC_TIMETYPE_YEAR] < 2014 ||
            time.time[RTC_TIMETYPE_YEAR] > 2030) {
         time.time[RTC_TIMETYPE_SECOND] 	= 0;
         time.time[RTC_TIMETYPE_MINUTE] 	= 0;
         time.time[RTC_TIMETYPE_HOUR]	= 0;
         time.time[RTC_TIMETYPE_DAYOFMONTH]	= 1;
         time.time[RTC_TIMETYPE_DAYOFWEEK]	= 1;
         time.time[RTC_TIMETYPE_DAYOFYEAR]	= 1;
         time.time[RTC_TIMETYPE_MONTH]     = 1;
         time.time[RTC_TIMETYPE_YEAR]	= 2014;
         Chip_RTC_SetFullTime(LPC_RTC, &time);
         pearlrt::Log::info("RTC seems to be never initialized "
                            "-> set to 1.1.2014");
      } else {
         pearlrt::Log::info("getRTCTime: %d.%d.%d %d:%d:%d",
                            (int)time.time[RTC_TIMETYPE_DAYOFMONTH],
                            (int)time.time[RTC_TIMETYPE_MONTH],
                            (int)time.time[RTC_TIMETYPE_YEAR],
                            (int)time.time[RTC_TIMETYPE_HOUR],
                            (int)time.time[RTC_TIMETYPE_MINUTE],
                            (int)time.time[RTC_TIMETYPE_SECOND]);

      }

      NVIC_DisableIRQ(RTC_IRQn);
      NVIC_SetPriority(RTC_IRQn, ((0x01 << 3) | 0x01));
      Chip_RTC_Enable(LPC_RTC, ENABLE);
      NVIC_EnableIRQ(RTC_IRQn);
   }

}
