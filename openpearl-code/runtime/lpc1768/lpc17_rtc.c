/*
 * lpc17_rtc.c
 *
 *  Created on: 28.04.2015
 *      Author: quitte
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
#include "chip.h"
#include "time_addons.h"

/* access the systick registers directly */
#define portNVIC_SYSTICK_LOAD_REG               \
                        ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG      \
                        ( * ( ( volatile uint32_t * ) 0xe000e018 ) )


static int noTicksYet = 1;  // flag to detect if FreeRTOS is already activated

static struct timespec tickBasedTime;

static int rtc_clock_gettime_cb(clockid_t clock_id, struct timespec *tp);

static void rtc_to_structtm(RTC_TIME_T * rtc_tm, struct tm * tm){
	tm->tm_sec	= rtc_tm->time[RTC_TIMETYPE_SECOND];
	tm->tm_min	= rtc_tm->time[RTC_TIMETYPE_MINUTE];
	tm->tm_hour	= rtc_tm->time[RTC_TIMETYPE_HOUR];
	tm->tm_mday	= rtc_tm->time[RTC_TIMETYPE_DAYOFMONTH];
	tm->tm_mon	= rtc_tm->time[RTC_TIMETYPE_MONTH];
	tm->tm_year	= rtc_tm->time[RTC_TIMETYPE_YEAR]-1900;
	tm->tm_wday	= rtc_tm->time[RTC_TIMETYPE_DAYOFWEEK];
	tm->tm_yday	= rtc_tm->time[RTC_TIMETYPE_DAYOFYEAR];
}

static time_t rtc_getunixtime(){
	RTC_TIME_T time;
	struct tm tm;
	Chip_RTC_GetFullTime(LPC_RTC, &time);
	rtc_to_structtm(&time,&tm);
	return mktime(&tm);
}

static int rtc_clock_gettime_cb(clockid_t clock_id, struct timespec *ts){
   if (noTicksYet) {
	ts->tv_nsec=0;
	ts->tv_sec=rtc_getunixtime();
   } else {
       *ts = tickBasedTime;
       // fetch sub milli seconds from the systick timer 
       if (portNVIC_SYSTICK_LOAD_REG != 0) {
         double tickRatio;
         tickRatio =  portNVIC_SYSTICK_CURRENT_VALUE_REG;
         tickRatio /= portNVIC_SYSTICK_LOAD_REG;
         ts->tv_nsec += (1 - tickRatio) * 1000000; // 1 ms = 1000000 ns
      }
   }
   return 0;
}

static void structtm_to_rtc(struct tm * tm, RTC_TIME_T * rtc_tm){
	rtc_tm->time[RTC_TIMETYPE_SECOND]	= tm->tm_sec;
	rtc_tm->time[RTC_TIMETYPE_MINUTE]	= tm->tm_min;
	rtc_tm->time[RTC_TIMETYPE_HOUR]		= tm->tm_hour;
	rtc_tm->time[RTC_TIMETYPE_DAYOFMONTH]	= tm->tm_mday;
	rtc_tm->time[RTC_TIMETYPE_MONTH]	= tm->tm_mon;
	rtc_tm->time[RTC_TIMETYPE_YEAR]		= tm->tm_year+1900;
	rtc_tm->time[RTC_TIMETYPE_DAYOFWEEK]=tm->tm_wday;
	rtc_tm->time[RTC_TIMETYPE_DAYOFYEAR]=tm->tm_yday;
}

static void rtc_setunixtime(time_t time){
	RTC_TIME_T rtc_time;
	struct tm tm;
	gmtime_r(&time, &tm);
	structtm_to_rtc(&tm,&rtc_time);
	Chip_RTC_SetFullTime(LPC_RTC,&rtc_time);
}

void rtc_settime(struct tm * tm) {
	RTC_TIME_T rtc_time;
	structtm_to_rtc(tm,&rtc_time);
	Chip_RTC_SetFullTime(LPC_RTC,&rtc_time);
}


static int check_rtc(RTC_TIME_T * rtctime, unsigned int fallbackstamp){
	struct tm tm={0};
	struct tm tmgood={0};
	time_t timestamp=0;
	rtc_to_structtm(rtctime,&tm);
	timestamp = mktime(&tm);
	gmtime_r(&timestamp,&tmgood);
	if(!(tm.tm_mday==tmgood.tm_mday))
			return 1;
	if(!(tm.tm_wday==tmgood.tm_wday))
		return 1;
	if(!(tm.tm_yday==tmgood.tm_yday))
			return 1;
	if(timestamp<fallbackstamp){
		return 1;
	}
	return 0;
}

void lpc17_systeminit_rtc_settime(unsigned int fallbackstamp){
	RTC_TIME_T time;
	Chip_RTC_GetFullTime(LPC_RTC, &time);
	if(check_rtc(&time, fallbackstamp)) {
		rtc_setunixtime(fallbackstamp);
        }
	set_clock_gettime_cb(rtc_clock_gettime_cb);
}

void vApplicationTickHook(void) {
   if (noTicksYet) {
      // read current time from RTC
      rtc_clock_gettime_cb(0, &tickBasedTime);  // read RTC as time base
      // and tell rtc_clock_gettime_cb to use the software based timer 
      // from now on
      noTicksYet = 0;
   } else {
      // increment the tick based time on each FreeRTOS-Tick
      tickBasedTime.tv_nsec += 1000000L;  // one milli second 
      if (tickBasedTime.tv_nsec > 1000000000L) { // one full second
         tickBasedTime.tv_sec ++;
         tickBasedTime.tv_nsec -= 1000000000L;
      }
   }
}
