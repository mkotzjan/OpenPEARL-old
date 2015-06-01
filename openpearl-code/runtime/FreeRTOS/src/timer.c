/*
 * timer.c
 *
 *  Created on: 27.05.2015
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
#include <errno.h> //not actually useful, at least not threadsafe,yet. It could however be added.
#include <inttypes.h>

#include "FreeRTOS.h"
#include "task.h"

#define MAXTIMER 12
#define ENTERCRITICAL taskDISABLE_INTERRUPTS();DMB();
#define LEAVECRITICAL DMB();taskENABLE_INTERRUPTS()

static inline void DMB(void) { asm volatile ("dmb" ::: "memory"); }

static TaskHandle_t xClackerTaskHandle;
static TaskHandle_t xTimerSortTaskHandle;
static void TaskClacker(void *);
static void TaskTimerSort(void *);

static void nsec_clock_gettime_wait(uint64_t*);
static int timerarm_wait(int64_t alarm);

void (* nsec_clock_gettime)(uint64_t*) = &nsec_clock_gettime_wait;
int (*timerarm)(int64_t alarm) = &timerarm_wait;

struct internaltimerspec{
	uint64_t nsec_value;
	uint64_t nsec_interval;
};

struct tableentry{
	timer_t				next;
	timer_t				prev;
	struct sigevent *__restrict evp;	//actually a pointer callback and its argument.
	struct internaltimerspec	value;
};

//list entry points
static volatile timer_t firstfree=0;
static volatile timer_t firstactive=0;
static volatile timer_t firstunsorted=0;
static volatile timer_t lastunsorted=0;//fifo, to not starve low frequency jobs

static struct{
	unsigned full:1;
	unsigned active:1;
	unsigned unsorted:1;
	unsigned tableinitialized:1;
	unsigned sorttaskreset:1;
}tablestate = {0};

//effectively 3 disjunct lists in the same table. others are dangling.
//due to delete being applicable to any list all of them need to be bidirected
//lists are terminated at both ends by next == index or prev == index
//that way it is possible to put a timer between a single timer.
static struct tableentry table[(timer_t)MAXTIMER];//endof range is special and marks the end of things

static void nsec_clock_gettime_wait(uint64_t *alarm){
	while(*nsec_clock_gettime == &nsec_clock_gettime_wait)
		DMB();
	(*nsec_clock_gettime)(alarm);
}

static int timerarm_wait(int64_t alarm){
	while(*timerarm == &timerarm_wait)
		DMB();
	return (*timerarm)(alarm);
}

void TimerResumeClackerFromISR(){
	if(xClackerTaskHandle){
		xTaskResumeFromISR(xClackerTaskHandle);
		portYIELD_FROM_ISR(xClackerTaskHandle);
	}
}

static void timer_init(){
	int i=0;
	for(i=0;i<MAXTIMER-1;i++)
		table[i].next=i+1;
	table[MAXTIMER-1].next=MAXTIMER-1;
	xTaskCreate(TaskTimerSort,"SortTask",50,NULL,configMAX_PRIORITIES-2,&xTimerSortTaskHandle);
	xTaskCreate(TaskClacker,"ClackerTask",200,NULL,configMAX_PRIORITIES-1,&xClackerTaskHandle);//stack 50 war nicht nur schlecht
	tablestate.tableinitialized=1;
}

static void itimerspectointernaltimerspec(const struct itimerspec			*const a,
												struct internaltimerspec	*const b){
	b->nsec_value    = (uint64_t)a->it_value.tv_sec*1e9    + a->it_value.tv_nsec;
	b->nsec_interval = (uint64_t)a->it_interval.tv_sec*1e9 + a->it_interval.tv_nsec;
}

static void internaltimerspectoitimerspec(const struct internaltimerspec	*const a,
												struct itimerspec			*const b){
	b->it_value.tv_sec     = a->nsec_value/1e9;
	b->it_value.tv_nsec    = a->nsec_value - b->it_value.tv_sec;
	b->it_interval.tv_sec  = a->nsec_interval/1e9;
	b->it_interval.tv_nsec = a->nsec_interval - b->it_value.tv_sec;
}

static int checktimerspec(const struct timespec * ts){
	if(ts->tv_sec<0)
		return -1;
	if(ts->tv_nsec<0)
		return -1;
	if(ts->tv_nsec>=1000000000)
		return -1;
	return 0;
}

static int checkitimerspec(const struct itimerspec * its){
	if(checktimerspec(&its->it_value)==-1)
		return -1;
	if(checktimerspec(&its->it_interval)==-1)
		return -1;
	return 0;
}

typedef void critical;//just a reminder, not an actual type
static critical dangle(const timer_t thatindex){
	struct tableentry * const that = &table[thatindex];
	//the free list is handled by the timer_create/delete pair
	if(thatindex == lastunsorted)
		lastunsorted = that->prev;
	if(thatindex == firstunsorted)
		firstunsorted = that->next;
	if(thatindex == firstactive)
		firstactive = that->next;

	if(that->next == that->prev){
		if(thatindex == firstactive)
			tablestate.active = 0;
		if(thatindex == firstunsorted)
			tablestate.unsorted = 0;
	}
	that->next = that->prev = thatindex;
	tablestate.sorttaskreset=1;
}

/*the timer must be dangling already*/
static critical putfirstunsorted(const timer_t thatindex){
	struct tableentry *const that = &table[thatindex];
	if(!tablestate.unsorted)
		firstunsorted = lastunsorted = thatindex;
	that->next = firstunsorted;
	table[that->next].prev = thatindex;
	firstunsorted = thatindex;
	tablestate.unsorted=1;
	tablestate.sorttaskreset=1;
}

int timer_create(clockid_t clock_id, struct sigevent *__restrict evp, timer_t *__restrict timerid){
	/*[EINVAL] The specified clock ID is not defined.*/
	if(!tablestate.tableinitialized)
		timer_init();

	ENTERCRITICAL;
	if(tablestate.full){
		LEAVECRITICAL;
		errno = EAGAIN;
		return -1;
	}
	*timerid = firstfree;
	struct tableentry *const that = &table[firstfree];
	if(firstfree == that->next)
		tablestate.full = 1;
	else
		firstfree = that->next;
	table[*timerid].evp=evp;
	//dangle the timer, this is the first timer in a list
	table[that->next].prev = that->next;
	that->next=*timerid;
	that->prev=*timerid;
	LEAVECRITICAL;
	return 0;
}

int timer_settime(const timer_t timerid, const int flags,
       const struct itimerspec *const value,
       struct itimerspec *const ovalue){
	struct tableentry *const that = &table[timerid];
	uint64_t nsectimestamp;
	(*nsec_clock_gettime)(&nsectimestamp);

	if(timerid>MAXTIMER){
		errno = EINVAL; return -1;}
	if(checkitimerspec(value)==-1){
		errno = EINVAL; return -1;}

	struct internaltimerspec loadvalue,ovalprep;
	itimerspectointernaltimerspec(value, &loadvalue);

	if(!loadvalue.nsec_value)
		loadvalue.nsec_value = loadvalue.nsec_interval;
	if(flags!=TIMER_ABSTIME&&loadvalue.nsec_value)
		loadvalue.nsec_value+=nsectimestamp;
	if(loadvalue.nsec_value){
		ENTERCRITICAL;
		ovalprep = that->value;
		that->value = loadvalue;
		dangle(timerid);
		putfirstunsorted(timerid);
		tablestate.sorttaskreset = 1;
		LEAVECRITICAL;
	}
	else{
		ENTERCRITICAL;
		ovalprep = that->value;
		that->value = loadvalue;
		dangle(timerid);
		tablestate.sorttaskreset = 1;
		LEAVECRITICAL;
	}

	if(ovalue){
		ovalprep.nsec_value -=nsectimestamp;
		internaltimerspectoitimerspec(&ovalprep,ovalue);
	}
	if(xTimerSortTaskHandle)
		vTaskResume(xTimerSortTaskHandle);
	return 0;
}

int timer_delete(timer_t timerid){
	if(timerid>=MAXTIMER){
		errno = EINVAL;
		return -1;
	}
	ENTERCRITICAL;
	struct tableentry *const that = &table[timerid];
	dangle(timerid);
	//fixup free list: put the freed timer as the first free of the chain of free timers
	if(!tablestate.full)
		that->next = firstfree;
	firstfree = timerid;
	table[that->next].prev = timerid;
	tablestate.full=0;
	that->value.nsec_value = that->value.nsec_interval =0;
	LEAVECRITICAL;
	return 0;
}

int timer_gettime(timer_t timerid, struct itimerspec *value){
	if(timerid>MAXTIMER){
		errno = EINVAL;
		return -1;
	}
	uint64_t nsectimestamp;
	(*nsec_clock_gettime)(&nsectimestamp);

	struct internaltimerspec valprep;

	ENTERCRITICAL;
	valprep = table[timerid].value;
	LEAVECRITICAL;

	valprep.nsec_value -=nsectimestamp;
	internaltimerspectoitimerspec(&valprep,value);
	return 0;
}

int timer_getoverrun(timer_t timerid){
	errno = ENOTSUP;
	return -1;
}

static void TaskTimerSort(void *pcParameters){
	timer_t timindex;
	timindex = firstactive;
	/*Nested functions are a non-standard GCC extension
	 * just cut and paste if it is an issue */
	void insertfirstactive(){
		ENTERCRITICAL;
		struct tableentry* const that = &table[lastunsorted];
		const timer_t thatindex = lastunsorted;
		if(!(tablestate.unsorted&&(!tablestate.active))){
			LEAVECRITICAL;return;}
		if(that->prev==thatindex)
			tablestate.unsorted=0;
		lastunsorted = that->prev;
		table[that->prev].next = that->prev; //terminate predecessor
		that->prev = that->next = firstactive = thatindex;
		tablestate.active = 1;
		LEAVECRITICAL;
		vTaskResume(xClackerTaskHandle);
	}
	void timer_put(	const timer_t previndex,
							const timer_t nextindex){
		struct tableentry *const prev = &table[previndex];
		struct tableentry *const next = &table[nextindex];

		//preventry == nextentry means either first or last
		ENTERCRITICAL;
		struct tableentry *const that = &table[lastunsorted];
		const unsigned int thatindex = lastunsorted;
		//check
		if(tablestate.sorttaskreset==1){
			LEAVECRITICAL;return;}
		if(tablestate.active==0||tablestate.unsorted==0){
			LEAVECRITICAL;return;}
		if(!(prev->value.nsec_value <= that->value.nsec_value)){
			LEAVECRITICAL;return;}
		if(!(next->value.nsec_value >= that->value.nsec_value)){
			LEAVECRITICAL;return;}
		if(prev==next){
			LEAVECRITICAL;return;}
		//check was okay. so actually insert:
		if(nextindex == firstactive)
			firstactive = lastunsorted;
		if(that->next == that->prev)//lastunsorted == firstunsorted
			tablestate.unsorted = 0;
		lastunsorted = that->prev;

		table[that->prev].next = that->prev;
		table[previndex].next = thatindex;
		table[nextindex].prev = thatindex;
		that->prev=previndex;
		that->next=nextindex;

		LEAVECRITICAL;
		vTaskResume(xClackerTaskHandle);
	}
	for(;;){
		while(tablestate.unsorted){
			if(tablestate.sorttaskreset){
				ENTERCRITICAL;
				tablestate.sorttaskreset=0;
				timindex = firstactive;
				LEAVECRITICAL;
			}
			if(!tablestate.active){
				insertfirstactive();
			}
			else if(table[lastunsorted].value.nsec_value < table[firstactive].value.nsec_value){
				timer_put(lastunsorted, firstactive);
				timindex=firstactive;
			}
			else if(table[lastunsorted].value.nsec_value < table[timindex].value.nsec_value){
				timer_put(table[timindex].prev,timindex);//new 'normal' entry or first entry
				timindex=firstactive;
			}
			else if(table[timindex].next == timindex){
				timer_put(timindex,lastunsorted);//new last entry
				timindex=firstactive;
			}
			else{
				timindex = table[timindex].next;
			}
		}
		vTaskSuspend(NULL);
	}
}

static void TaskClacker(void *pcParameters){
	static uint64_t timestamp=0;
	void activateEntry(){
		ENTERCRITICAL;
		struct tableentry * const that = &table[firstactive];
		struct { void (*cb)(void *arg_ptr);
				 void *th;
				}*callback = (void *)(that->evp);
		const unsigned int thatindex = firstactive;

		if(!(that->value.nsec_value < timestamp)){
			LEAVECRITICAL;return;}
		if(that->next == thatindex)
			tablestate.active = 0;
		firstactive = that->next;
		//dangle
		that->prev = that->next = thatindex;
		//timer reload
		if(that->value.nsec_interval){
			that->value.nsec_value += that->value.nsec_interval;
			if(that->value.nsec_value<timestamp)
				that->value.nsec_value = that->value.nsec_interval + timestamp;
			putfirstunsorted(thatindex);
		}
		else
			that->value.nsec_value = 0;
		tablestate.sorttaskreset = 1;
		LEAVECRITICAL;

		/*callback->cb(callback->th); can be used instead of a FreeRTOS Task,
		 *  if locking the clacker is acceptable
		 *  Careful with nested Tasks! containing function must not exit!*/
		void StarterTask(void *pcParameters){
			struct {
						  void (*cb)(void *arg_ptr);
						  void *th;
				}*callback = pcParameters;
			callback->cb(callback->th);
			vTaskDelete(NULL);
		}
		xTaskCreate(StarterTask,"StarterTask",200,callback,configMAX_PRIORITIES-3,NULL);
		if(tablestate.sorttaskreset)
			vTaskResume(xTimerSortTaskHandle);
	}

	for(;;){
		if(tablestate.active)
			do{//the double while reduces the gettime calls
				while((table[firstactive].value.nsec_value < timestamp) && tablestate.active)
					activateEntry();
				(*nsec_clock_gettime)(&timestamp);
			}while((table[firstactive].value.nsec_value < timestamp) && tablestate.active);
		if(tablestate.active){//no harm in being called early
			int fail=0;
			do{
				ENTERCRITICAL;
				fail = (*timerarm)(table[firstactive].value.nsec_value-timestamp);
				LEAVECRITICAL;
				(*nsec_clock_gettime)(&timestamp);
			}while(fail<0);
			if(table[firstactive].value.nsec_value > timestamp)
				vTaskSuspend(NULL);
		}
		else
			vTaskSuspend(NULL);
	}
}
