/*
 * clock.c
 *
 *  Created on: 28.05.2015
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
#include <sys/errno.h>

extern int (*clock_gettime_cb)(clockid_t clock_id, struct timespec *tp);

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

extern int clock_gettime(clockid_t clock_id, struct timespec *ts){
	return (*clock_gettime_cb)(clock_id, ts);
}

#include <sys/time.h>
int gettimeofday(struct timeval *__p, void *__tz){
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	if(__tz)
		return ENOSYS;
	__p->tv_usec=ts.tv_nsec/1000;
	__p->tv_sec=ts.tv_sec;
	return 0;
}
