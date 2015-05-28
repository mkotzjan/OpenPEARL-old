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
#include "chip.h"
#include "systeminit.h"

extern int (*clock_gettime_cb)(clockid_t clock_id, struct timespec *tp);
static void default_nsec_clock_gettime_cb(uint64_t*);
static void (*nsec_clock_gettime_cb)(uint64_t*) = &default_nsec_clock_gettime_cb;

static volatile unsigned int unixtime;
static volatile uint64_t unixtime_nsec_offset=0;
static volatile uint32_t tickspersecond = 100000000;
//used by the RTC self calibrating thing
static volatile unsigned int goodcounter = 0;

unsigned int timer_ready(){
	return goodcounter;
}

//Todo: Calculate the maximum change in the next second and make a second short by that amount.
//this ensures perfect monotony and gives a decent value for the precision

static unsigned int ticks2nsec(unsigned int ticks){
	return (unsigned int)(
			((uint64_t)ticks*(uint64_t)1e9)
			/((uint64_t)tickspersecond));
}

unsigned int nsec2ticks(unsigned int nsecs){
	return (unsigned int)(
			((uint64_t)nsecs*(uint64_t)tickspersecond)
			/(uint64_t)(1e9));
}

/*timerarm prepares the HW to generate an Interrupt
 * no later than the specified time, but as late as
 * possible. */
void timerarm(int64_t alarmstamp){
	uint32_t mystamp = nsec2ticks(alarmstamp);
	mystamp += LPC_TIMER0->TC;
	mystamp %= tickspersecond;
	LPC_TIMER0->MR[2] = mystamp;
	LPC_TIMER0->MCR |= (1<<6);
};

void nsec_clock_gettime(uint64_t *nsectime){
	nsec_clock_gettime_cb(nsectime);
}

void default_nsec_clock_gettime_cb(uint64_t *nsectime){
	uint64_t timestamp = 0;
	while(timestamp != unixtime_nsec_offset){
		timestamp = unixtime_nsec_offset;
		*nsectime = unixtime_nsec_offset + ticks2nsec(LPC_TIMER0->TC);
	}
}

void debug_nsec_clock_gettime_cb(uint64_t *nsectime){
	uint32_t timestamp = 0;
	while(timestamp != LPC_TIMER1->TC){
		timestamp = LPC_TIMER1->TC;
		*nsectime = ((uint64_t)timestamp * 1000000000) + ((LPC_TIMER0->TC)*10);
	}
}

static int systime_clock_gettime_cb(clockid_t clock_id, struct timespec *tp){
	while(tp->tv_sec!=unixtime){
		tp->tv_sec=unixtime;
		tp->tv_nsec=ticks2nsec(LPC_TIMER0->TC);
	}
	return 0;
}

static int debugtime_clock_gettime_cb(clockid_t clock_id, struct timespec *tp){
	while(tp->tv_sec!=LPC_TIMER1->TC){
		tp->tv_sec=LPC_TIMER1->TC;
		tp->tv_nsec=ticks2nsec(LPC_TIMER0->TC);
	}
	return 0;
}

void systeminit_debug_settime(){
	struct timespec ts;
	clock_gettime(0,&ts);
	tickspersecond=Chip_Clock_GetSystemClockRate();
	LPC_TIMER0->TC=nsec2ticks(ts.tv_nsec);
	LPC_TIMER1->TC=ts.tv_sec;
	clock_gettime_cb = &debugtime_clock_gettime_cb;
	nsec_clock_gettime_cb = &debug_nsec_clock_gettime_cb;
}

void RTC_IRQHandler() {
	//using non-standard gcc extension for nested functions
	void InitTimerFromRTC_IRQHandler(){
		/* This switches the clock over from RTC to the this
		 * Realtime clock, as soon as it is available.
		 * */
		struct timespec ts;
		clock_gettime(0,&ts);
		unixtime = ts.tv_sec;
		unixtime_nsec_offset = unixtime * 1000000000;
		clock_gettime_cb=&systime_clock_gettime_cb;
	}
	void calibraterRTtimerFromRTC_IRQHandler(volatile int delta){
		unsigned int tickstore = tickspersecond;

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
		void TimerResumeClackerFromISR();
		TimerResumeClackerFromISR();
	}
	static enum{
		tick,
		tock,
		init,
		noinit
	}tickstate=noinit;
	int delta = LPC_TIMER0->TC - tickspersecond;
	LPC_TIMER0->TCR=3;LPC_TIMER0->TCR=1;//Reset the Timer0

	switch(tickstate){
	case tick:
		tickstate = tock;
		unixtime++;
		unixtime_nsec_offset += 1e9;
		break;
	case tock:
		tickstate = tick;
		unixtime++;
		unixtime_nsec_offset += 1e9;
		calibraterRTtimerFromRTC_IRQHandler(delta);
		break;
	case init:
		InitTimerFromRTC_IRQHandler();
		tickstate = tock;
		tickspersecond = delta;//Initial value
		break;
	case noinit:
		//wait to init at beginning of second
		//Todo:load worsened, saved values from battery backed ram.
		//this would improve bootup speed significantly.
		tickstate = init;
		break;
	}
	LPC_RTC->ILR = 3;
}

void TIMER0_IRQHandler(){
	void TimerResumeClackerFromISR();
	//decide between second and alarm interrupt
	TimerResumeClackerFromISR();
	LPC_TIMER0->MCR &= ~(1<<6);
	LPC_TIMER0->IR = 1<<2;
}

