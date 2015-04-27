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

#define _POSIX_TIMERS

#include <time.h>
#include <stdio.h>
#include "chip.h"

static volatile unsigned int unixtime;
static volatile unsigned int tickspersecond;

void TIMER0_IRQHandler() __attribute__ ((used));
void RTC_IRQHandler() __attribute__ ((used));

//Todo: Calculate the maximum change in the next second and make a second short by that amount.
//this ensures perfect monotony and gives a decent value for the precision

static unsigned int ticks2nsec(unsigned int nsecs){
	return (unsigned int)(
			((uint64_t)nsecs*1000000000)
			/((uint64_t)tickspersecond));
}

static unsigned int nsec2ticks(unsigned int ticks){
	return (unsigned int)(
			((uint64_t)ticks*(uint64_t)tickspersecond)
			/(uint64_t)(1000000000));
}

//static?
int systime_clock_gettime_cb(clockid_t clock_id, struct timespec *tp){
	tp->tv_nsec=ticks2nsec(LPC_TIMER0->TC);
	tp->tv_sec=unixtime;
	return 0;
}

static inline void calibraterRTtimerFromRTC_IRQHandler(volatile int delta){
	static unsigned int goodcounter;
	unsigned int tickstore = tickspersecond;

	printf("calibration: %i.%i Mhz\n",tickspersecond/1000000,tickspersecond-((tickspersecond/1000000)*1000000));
	printf("error: %i ticks, this second\n",delta);


	if(delta==0){
		goodcounter++;
		if(goodcounter>1800){//1h
			goodcounter = 0;
			tickspersecond = (unsigned int)(
				((uint64_t)tickspersecond*((uint64_t)(LPC_TIMER0->PR+1))
						/(uint64_t)(LPC_TIMER0->PR))
							);
			LPC_TIMER0->PR-=1;
		}
	}
	else if(delta==1)
		tickspersecond++;
	else if(delta==-1)
		tickspersecond--;
	else{
		tickspersecond=tickspersecond+(delta/2);
		if(tickstore==tickspersecond)
				tickspersecond++;
		LPC_TIMER0->PR+=1;//increase the timer0 prescaler
		tickspersecond = (unsigned int)(
				((uint64_t)tickspersecond*((uint64_t)(LPC_TIMER0->PR))
						/(uint64_t)(LPC_TIMER0->PR+1))
						);
		goodcounter=0;
	}
}

extern int (*clock_gettime_cb)(clockid_t clock_id, struct timespec *tp);
static void InitTimerFromRTC_IRQHandler(){
	/* This switches the clock over from RTC to the this
	 * Realtime clock, as soon as it is available.
	 * */
	struct timespec ts;
	clock_gettime(0,&ts);
	unixtime = ts.tv_sec;
	clock_gettime_cb=&systime_clock_gettime_cb;
}

__attribute__ ((used)) void RTC_IRQHandler() {
	static enum{
		tick,
		tock,
		init,
		noinit
	}tickstate=noinit;
	int delta = LPC_TIMER0->TC - tickspersecond;
	LPC_TIMER0->TCR=3;LPC_TIMER0->TCR=1;//Reset the Timer0
	unixtime++;

	switch(tickstate){
	case tick:
		tickstate = tock;
		break;
	case tock:
		tickstate = tick;
		calibraterRTtimerFromRTC_IRQHandler(delta);
		break;
	case init:
		InitTimerFromRTC_IRQHandler();
		tickstate = tock;
		tickspersecond = delta;//Initial value
		break;
	case noinit:
		//wait to init at beginning of second
		//load worsened, saved values from battery backed ram.
		tickstate = init;
		break;
	}
	LPC_RTC->ILR = 3;
}

