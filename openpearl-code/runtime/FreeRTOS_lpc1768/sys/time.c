/*
 [The "BSD license"]
 Copyright (c) 2015 Jonas Meyer
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

#include <time.h>
#include <sys/time.h>
#include <sys/errno.h>
#include "chip.h"
#include "systeminit.h"

static int rtc_clock_gettime_cb(clockid_t clock_id, struct timespec *tp);
int (*clock_gettime_cb)(clockid_t clock_id, struct timespec *tp) = &rtc_clock_gettime_cb;

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

static void rtc_setunixtime(time_t time){
	RTC_TIME_T rtc_time;
	struct tm tm;
	gmtime_r(&time, &tm);
	structtm_to_rtc(&tm,&rtc_time);
	Chip_RTC_SetFullTime(LPC_RTC,&rtc_time);
}

void systeminit_rtc_settime(unsigned int fallbackstamp){
	RTC_TIME_T time;
	Chip_RTC_GetFullTime(LPC_RTC, &time);
	if(check_rtc(&time, fallbackstamp))
		rtc_setunixtime(fallbackstamp);
}

static time_t rtc_getunixtime(){
	RTC_TIME_T time;
	struct tm tm;
	Chip_RTC_GetFullTime(LPC_RTC, &time);
	rtc_to_structtm(&time,&tm);
	return mktime(&tm);
}

struct tm *localtime_r(const time_t *_time, struct tm * tm){
	//always assume localtime was UTC
	gmtime_r(_time, tm);
	return tm;
}

struct tm *localtime(const time_t *_time){
	//always assume localtime was UTC
	static struct tm tm;//needs not be thread safe, says susv3
	return localtime_r(_time, &tm);
}

static int rtc_clock_gettime_cb(clockid_t clock_id, struct timespec *ts){
	ts->tv_nsec=0;
	ts->tv_sec=rtc_getunixtime();
	return 0;
}

extern int clock_gettime(clockid_t clock_id, struct timespec *ts){
	return (*clock_gettime_cb)(clock_id, ts);
}

int gettimeofday(struct timeval *__p, void *__tz){
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	if(__tz)
		return ENOSYS;
	__p->tv_usec=ts.tv_nsec/1000;
	__p->tv_sec=ts.tv_sec;
	return 0;
}

