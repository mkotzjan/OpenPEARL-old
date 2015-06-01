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
#include <inttypes.h>
#include "chip.h"
#include "systeminit.h"
#include "FreeRTOSConfig.h"

extern int (*clock_gettime_cb)(clockid_t clock_id, struct timespec *tp);
extern void (*nsec_clock_gettime)(uint64_t*);
extern int (*timerarm)(int64_t);

static volatile time_t unixtime;
static volatile uint64_t unixtime_nsec_offset=0;
static volatile uint32_t tickspersecond = 1e8;

static unsigned int ticks2nsec(unsigned int ticks){
	return (unsigned int)(
			((uint64_t)ticks*((uint64_t)1e9))
			/((uint64_t)tickspersecond));
}

static unsigned int nsec2ticks(unsigned int nsecs){
	return (unsigned int)(
			((uint64_t)nsecs*(uint64_t)tickspersecond)
			/((uint64_t)1e9));
}

/*timerarm prepares the HW to generate an Interrupt
 * no later than the specified time, but as late as
 * possible. No need to check wether alarmstamp is
 * in the past, ClackerTask already does*/
static int timerarm_cb_systime(int64_t alarmstamp){
	LPC_TIMER0->IR = (1<<6);
	uint32_t mystamp = nsec2ticks(alarmstamp);
	mystamp += LPC_TIMER0->TC;
	mystamp %= tickspersecond;
	LPC_TIMER0->MR[2] = mystamp;
	if(LPC_TIMER0->TC >= tickspersecond){
		return -1;
	}
	LPC_TIMER0->MCR |= (1<<6);
	return 0;
};

static void nsec_clock_gettime_timer0(uint64_t *nsectime){
	uint64_t timestamp = 0;
	while(timestamp != unixtime_nsec_offset){
		timestamp = unixtime_nsec_offset;
		*nsectime = timestamp + (uint64_t)ticks2nsec(LPC_TIMER0->TC);
	}
}

static int clock_gettime_timer0(clockid_t clock_id, struct timespec *tp){
	while(tp->tv_sec!=unixtime){
		tp->tv_sec=unixtime;
		tp->tv_nsec=ticks2nsec(LPC_TIMER0->TC);
	}
	return 0;
}

void systeminit_timer0(){
	//TODO: sync to beginning of RTC second
	struct timespec ts;
	clock_gettime(0,&ts);
	unixtime = ts.tv_sec;
	unixtime_nsec_offset = unixtime * (uint64_t)1e9;
	tickspersecond = Chip_Clock_GetSystemClockRate();
	LPC_SYSCON->PCONP|=(1<<1);//powerup Timer 0
	LPC_SYSCON->PCLKSEL[0]|=(1<<2);//bits 3:2 = 01 to set PCLK_TIMER0 to run at full speed. reset=00
	LPC_TIMER0->MR[1] = tickspersecond; //1s
	LPC_TIMER0->MR[2] = ~0;
	LPC_TIMER0->MCR = (0x3<<3)|(0x01<<6);//reset+interrupt on mr1, interrupt on mr2
	LPC_TIMER0->CTCR=0;//Timer0 is running in timer mode.
	LPC_TIMER0->PR=0;//Prescaler=0;
	timerarm = &timerarm_cb_systime;
	LPC_TIMER0->IR = (~0);//clear interrupts
	LPC_TIMER0->TCR=1;//enable
	NVIC_SetPriority(TIMER0_IRQn,configMAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(TIMER0_IRQn);
	timerarm = &timerarm_cb_systime;
	clock_gettime_cb = &clock_gettime_timer0;
	nsec_clock_gettime = &nsec_clock_gettime_timer0;
}

static void nsec_clock_gettime_debug(uint64_t *nsectime){
	uint32_t timestamp = 0;
	while(timestamp != LPC_TIMER1->TC){
		timestamp = LPC_TIMER1->TC;
		*nsectime = ((uint64_t)timestamp * (uint64_t)1e9) + ((uint64_t)(LPC_TIMER0->TC)*(uint64_t)10);
	}
}

static int clock_gettime_debug(clockid_t clock_id, struct timespec *tp){
	while(tp->tv_sec!=LPC_TIMER1->TC){
		tp->tv_sec=LPC_TIMER1->TC;
		tp->tv_nsec=ticks2nsec(LPC_TIMER0->TC);
	}
	return 0;
}

void systeminit_debug_settime(){
	struct timespec ts;
	systeminit_timer0();
	LPC_SYSCON->PCONP|=(1<<1);//powerup Timer 1
	LPC_SYSCON->PCLKSEL[0]|=(1<<4);//set PCLK_TIMER1 to run at full speed. reset=00
	LPC_IOCON->PINSEL[3] |= (3<<4)|(3<<18)|(3<<24);//p1.18 is cap1.0 p1.25 is mat1.1 output,p1.28 is mat0.0
	LPC_TIMER0->EMR |= (3<<4);//toggle externel match thingie
	LPC_TIMER1->CTCR=0x3;//tick from both edges of cap1.0
	LPC_TIMER1->TCR=1;
	clock_gettime(0,&ts);
	LPC_TIMER0->TC=nsec2ticks(ts.tv_nsec);
	LPC_TIMER1->TC=ts.tv_sec;
	clock_gettime_cb = &clock_gettime_debug;
	nsec_clock_gettime = &nsec_clock_gettime_debug;
}

void TIMER0_IRQHandler(){
	void TimerResumeClackerFromISR();
	//decide between second and alarm interrupt
	if(LPC_TIMER0->IR&(1<<1)){
		unixtime++;
		unixtime_nsec_offset += (uint64_t)1e9;
		LPC_TIMER0->IR = (1<<1);
	}
	if(LPC_TIMER0->IR&(1<<2)){
		TimerResumeClackerFromISR();
		LPC_TIMER0->MCR &= ~(1<<6);
		LPC_TIMER0->IR = (1<<2);
	}
}
