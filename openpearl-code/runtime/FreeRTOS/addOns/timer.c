/*
 * timer.c
 *
 *  Created on: 27.05.2015
 *      Author: jonas meyer, rainer mueller
 */
/*
 [The "BSD license"]
 Copyright (c) 2015 Jonas Meyer
               2015 Rainer Mueller
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


/* ------------------------
   Modifications
   dec 2015: r. mueller : only 1 timer thread which waits for continuation
                          from the isr
                          there is a linked list of active timers
                          on timer_settime - the timer is removed from the list
                                             if the timespec is 0
                                           - the timer is removed and inserted
                                             to the list, else
                          the list of timers is sorted according the
                          next timeout
*/


#include <time.h>

//not actually useful, at least not threadsafe,yet. It could however be added.
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "time_addons.h"
#include "FreeRTOSClock.h"

#define TIMER_STACK_SIZE 1000
#define MAXTIMER configTIMER_QUEUE_LENGTH
#define ENTERCRITICAL taskDISABLE_INTERRUPTS();DMB();
#define LEAVECRITICAL DMB();taskENABLE_INTERRUPTS()

static TaskHandle_t xClackerTaskHandle;
static void clackerTask(void *);
static void retrigger_timer();


struct internalTimerSpec {
   uint64_t nsec_value;
   uint64_t nsec_interval;
};

struct tableEntry {
   timer_t next;			// used in free and active list
   timer_t prev;                        // used only in active list
   struct sigevent *__restrict evp;	//actually a pointer the callback struct
   // which consists of
   //   the pointer to the function
   //    and its argument.
   struct internalTimerSpec value;      // initial delay and interval in nsec
};

static struct tableEntry timerTable[MAXTIMER];
static timer_t unused = -1;            // points to no timer
static timer_t expires_next = -1;       // points to no timer

static QueueHandle_t timer_queue;

// function pointer to define next timeout
static void (*timer_set)(uint64_t ns) = NULL;

/* various flags for internal control of the itimer subsystem */
static struct {
   unsigned tableinitialized: 1;
} tablestate = {0};

static void itimerspec2internaltimerspec(
   const struct itimerspec *const a,
   struct internalTimerSpec	*const b) {
   b->nsec_value    = (uint64_t)(a->it_value.tv_sec) * 1e9    +
                      a->it_value.tv_nsec;
   b->nsec_interval = (uint64_t)(a->it_interval.tv_sec) * 1e9 +
                      a->it_interval.tv_nsec;
}

static void dump_timers() {
   timer_t i;

   printf("dump_timers: expires_next=%d\n", (int)expires_next);
   i = expires_next;

   while (i != -1) {
      printf("#%d: at %"PRIu64"  all %"PRIu64" \n",
             (int)i, timerTable[i].value.nsec_value,
             timerTable[i].value.nsec_interval);
      i = timerTable[i].next;
   }
}
/*
static void internaltimerspec2itimerspec(
   const struct internalTimerSpec *const a,
   struct itimerspec *const b) {
   b->it_value.tv_sec     = a->nsec_value / 1e9;
   b->it_value.tv_nsec    = a->nsec_value - b->it_value.tv_sec;
   b->it_interval.tv_sec  = a->nsec_interval / 1e9;
   b->it_interval.tv_nsec = a->nsec_interval - b->it_value.tv_sec;
}
*/

/**
retrigger the timer for the next relevant value

\returns 0, if timer was retriggered
\returns 1, if the timer reached its timeout in the meantime
*/
static void retrigger_timer() {
   uint64_t now;
   uint64_t delta;

   if (timer_set) {  // is there a real timer registered ?
      if (expires_next != -1) {
         clock_gettime_nsec(&now);

         if (timerTable[expires_next].value.nsec_value > now) {
            delta = timerTable[expires_next].value.nsec_value - now;
//          printf("retrigger timer: %d  delta: %"PRIu64" \n",
//                 (int)expires_next, delta);
            (*timer_set)(delta);
            return;
         }

//         printf("retrigger was too slow --> resumetimertask\n");
         resume_timer_task_from_ISR();
         return;
      }
   } else {
      printf("ERROR no real timer defined\n");
   }
}

static void timer_init_if_necessary() {
   int i = 0;
   StructParameters_t createParameters;
   static StackType_t timerStack[TIMER_STACK_SIZE];
   static TCB_t timerTcb;

   createParameters.pvParameter = NULL;
   createParameters.tcb = NULL;
   createParameters.stack = timerStack;
   createParameters.tcb = &timerTcb;

   if (! tablestate.tableinitialized) {
      if (timer_set == NULL) {
         // no timer defined by the user as static object
         // enforce the FreeRTOS tick based clock
         selectFreeRTOSClock();
      }

      unused = 0;
      expires_next = -1;

      timerTable[0].next = 1;
      timerTable[0].prev = -1;

      for (i = 1; i < MAXTIMER - 1; i++) {
         timerTable[i].next = i + 1;
         timerTable[i].prev = i - 1;
      }

      timerTable[MAXTIMER - 1].next = -1;
      timerTable[MAXTIMER - 1].prev = MAXTIMER - 2;

      timer_queue = xQueueCreate(10, 1);

      xTaskCreate(clackerTask,
                  "ClackerTask", TIMER_STACK_SIZE, &createParameters,
                  configMAX_PRIORITIES - 1,
                  &xClackerTaskHandle); //stack 50 war nicht nur schlecht

      tablestate.tableinitialized = 1;
   }
}

static timer_t find_free_timer_and_remove_from_free_list() {
   timer_t i = unused;

   if (i != -1) {
      // free timer available
      unused = timerTable[i].next;
   }

   return i;
}

static void insert_timer_into_free_list(timer_t t) {
   timerTable[t].next = unused;
   unused = t;
}

static void remove_timer_from_active_list(timer_t t) {
   // pedandic check if timer is really in active list
   int i;
   int timer_found = 0;

   i = expires_next;

   while (i != -1 && timer_found == 0) {
      if (i == t) {
         timer_found = 1;

         if (i == expires_next) {
            // remove first element from list
            expires_next =  timerTable[i].next;
         }
      }

      i = timerTable[i].next;
   }

   if (timer_found == 0) {
      // ok: delete non active timer -- nothing more to do
      return;
   }

   timerTable[timerTable[t].next].prev = timerTable[t].prev;
   timerTable[timerTable[t].prev].next = timerTable[t].next;
}


static void insert_timer_into_active_list(timer_t t) {
   int i = expires_next;
   int last_i;

   if (expires_next == -1) {
      // first element in list
      expires_next = t;
      timerTable[t].next = -1; // no next element
      timerTable[t].prev = -1; // no previous element
   } else {
      last_i = i;

      while (i >= 0 &&
             timerTable[t].value.nsec_value >=
             timerTable[i].value.nsec_value) {
         last_i = i;
         i = timerTable[i].next;
      }

      if (i == -1) {
         // append to the end
         timerTable[last_i].next = t;
         timerTable[t].prev = last_i;
         timerTable[t].next = -1;
      } else if (last_i == i) {
         // insert before first element
         timerTable[t].next = i;
         timerTable[t].prev = -1;
         timerTable[expires_next].prev = t;
         expires_next = t;
      } else {
         // insert between elements last_i and i
         timerTable[t].next = i;
         timerTable[t].prev = last_i;
         timerTable[last_i].next = t;
         timerTable[i].prev = t;
      }
   }
}

static inline void DMB(void) {
   asm volatile("dmb" ::: "memory");
}


/*
checks the value of the given timespec

\returns -1: if ts is illegal (negative values)
          0: if ts is zero
          1: if ts is positive
*/
static int timerspecOk(const struct timespec * ts) {
   // test for illegal value
   if (ts->tv_sec < 0) {
      return -1;
   }

   if (ts->tv_nsec < 0) {
      return -1;
   }

   if (ts->tv_nsec >= 1000000000) {
      return -1;
   }

   // test for zero
   if (ts->tv_sec == 0 && ts->tv_nsec == 0) {
      return 0;
   }

   return 1;
}

/*
checks the value of the given itimespec

\returns -1: if ts is illegal (negative values)
          0: if ts is zero
          1: if ts is positive
*/
static int itimerspecOk(const struct itimerspec * its) {
   int testValue1;
   int testValue = timerspecOk(&(its->it_value));

   if (testValue == -1) {
      return -1;
   }

   testValue1 = timerspecOk(&(its->it_interval));

   if (testValue1 == -1) {
      return -1;
   }

   // both values are ok
   // the result must be 0, if both are zero
   //                    1 else
   // --> just add both and trim into 0/1 by double negation
   return !!(testValue + testValue1);
}

void register_timer_source(void (*set)(uint64_t ns),
                           void (*get)(uint64_t  * ns)) {
   timer_set = set;
   set_clock_gettime_cb(get);
}

int timer_create(clockid_t clock_id,
                 struct sigevent *__restrict evp,
                 timer_t *__restrict timerid) {
   /*[EINVAL] The specified clock ID is not defined.*/

   timer_t firstfree;

   timer_init_if_necessary();


   ENTERCRITICAL;

   firstfree = find_free_timer_and_remove_from_free_list();

   if (firstfree < 0) {
      LEAVECRITICAL;
      errno = EAGAIN;
      return -1;
   }

   *timerid = firstfree;
   timerTable[firstfree].evp = evp;
   LEAVECRITICAL;

   return 0;
}

int timer_delete(timer_t thatindex) {

   timer_init_if_necessary();

   if (thatindex >= MAXTIMER) {
      errno = EINVAL;
      return -1;
   }

   ENTERCRITICAL;

   remove_timer_from_active_list(thatindex);
   insert_timer_into_free_list(thatindex);

   retrigger_timer();

   LEAVECRITICAL;
   return 0;
}

int timer_settime(const timer_t timerid, const int flags,
                  const struct itimerspec *const value,
                  struct itimerspec *const ovalue) {
   uint64_t now;

   timer_init_if_necessary();

   /*
   printf("timer_settime(tid=%d val=%ld:%ld rept=%ld:%ld\n", (int)timerid,
                  (long int)value->it_value.tv_sec,
                  (long int)value->it_value.tv_nsec,
                  (long int)value->it_interval.tv_sec,
                  (long int)value->it_interval.tv_nsec);
   */

   if (timerid > MAXTIMER) {
      errno = EINVAL;
      return -1;
   }

   switch (itimerspecOk(value)) {
   case -1: // illegal itimerspec value
      return -1;

   case 0:  // set with time 0 --> kill timer
//         printf("timer_settime: kill timer %d\n", (int)timerid);


      ENTERCRITICAL;
//printf("settime: remove timer: %d, next = %d\n", timerid, expires_next);
      remove_timer_from_active_list(timerid);
//printf("settime1: remove timer: %d, next = %d\n", timerid, expires_next);
      retrigger_timer();
      LEAVECRITICAL;
      break;

   case 1:  // set with time > 0 --> add/modify timer
      clock_gettime_nsec(&now);
//         printf("timer_settime: add/modify timer %d\n", (int)timerid);
      ENTERCRITICAL;
      itimerspec2internaltimerspec(value, &timerTable[timerid].value);
      timerTable[timerid].value.nsec_value += now;
//    printf("internalTimeSpec value %"PRIu64"\n",
//            timerTable[timerid].value.nsec_value);
      insert_timer_into_active_list(timerid);
//dump_timers();
      retrigger_timer();
      LEAVECRITICAL;
      break;
   }

   return 0; // success
}

void resume_timer_task_from_ISR() {
   char dummy = 1;

//printf("clack\n");
   if (xClackerTaskHandle) {
      if (!xQueueSendFromISR(timer_queue, &dummy, NULL)) {
         printf("error at xQuereSendFromISR\n");
      }

      //xTaskResumeFromISR(xClackerTaskHandle);
      //The yield is _very_ important for performance
      portYIELD_FROM_ISR(xClackerTaskHandle);
   }
}

static void clackerTask(void *pcParameters) {
   uint64_t now, timeout;
   int timerid;
   char dummy;

   struct Callback {
      void (*cb)(void *arg_ptr);
      void *th;
   }*callback;

   while (1) {
      //vTaskSuspend(NULL);
      if (!xQueueReceive(timer_queue, &dummy, portMAX_DELAY)) {
         printf("error at xQueueReceive\n");
      }


//printf("clackerTask: clack\n");
//dump_timers();
      // check, which timers reached their timeout

      clock_gettime_nsec(&now);

      ENTERCRITICAL;

      do {
         timerid = expires_next;

         if (timerid >= 0) {
            timeout = timerTable[timerid].value.nsec_value;

//printf("clack: timerid=%d  timeout=%"PRIu64" now= %"PRIu64" \n",
//          timerid, timeout, now);
            if (timeout < now) {
               // timeout reached
               // --> remove timer from list and
               // --> reinsert if timer is set periodic
//printf("timeout reached\n");
               remove_timer_from_active_list(timerid);

               if (timerTable[timerid].value.nsec_interval) {
                  timerTable[timerid].value.nsec_value +=
                     timerTable[timerid].value.nsec_interval;
//printf("reinserted after interval \n");
                  insert_timer_into_active_list(timerid);
//dump_timers();
               }

               // invoke timer callback method
               callback = (struct Callback*) timerTable[timerid].evp;
//            printf("call of callback %p(%p) \n",
//                     callback->cb, callback->th);
               callback->cb(callback->th);
//            printf("callback done \n");
            }
         }
      } while (timerid >= 0 && timeout < now);

      retrigger_timer();
      LEAVECRITICAL;
   }
}

#ifdef TOBEDELETED
static void nsec_clock_gettime_wait(uint64_t*);
static int timerarm_wait(int64_t alarm);

void (*nsec_clock_gettime)(uint64_t*) = nsec_clock_gettime_wait;
int (*timerarm)(int64_t alarm) = timerarm_wait;

struct internaltimerspec {
   uint64_t nsec_value;
   uint64_t nsec_interval;
};

struct tableentry {
   timer_t next;
   timer_t prev;
   struct sigevent *__restrict evp;	//actually a pointer callback
   // and its argument.
   struct internaltimerspec value;
};

//list entry points
static volatile timer_t firstfree = 0;
static volatile timer_t firstactive = 0;
static volatile timer_t firstunsorted = 0;
static volatile timer_t lastunsorted = 0; //fifo, to not starve
// low frequency jobs


// effectively 3 disjunct lists in the same table. others are dangling.
// due to delete being applicable to any list all of them need to be bidirected
// lists are terminated at both ends by next == index or prev == index
// that way it is possible to put a timer between a single timer.

//endof range is special and marks the end of things
static struct tableentry table[(timer_t)MAXTIMER];

static void nsec_clock_gettime_wait(uint64_t *alarm) {
   while (*nsec_clock_gettime == &nsec_clock_gettime_wait) {
      DMB();
   }

   nsec_clock_gettime(alarm);
}

static int timerarm_wait(int64_t alarm) {
   while (*timerarm == &timerarm_wait) {
      DMB();
   }

   return (*timerarm)(alarm);
}

void TimerResumeClackerFromISR() {
   if (xClackerTaskHandle) {
      xTaskResumeFromISR(xClackerTaskHandle);
      //The yield is _very_ important for performance
      portYIELD_FROM_ISR(xClackerTaskHandle);
   }
}

static void timer_init() {
   int i = 0;
   table[0].prev = 0;
   table[0].next = 1;

   for (i = 1; i < MAXTIMER; i++) {
      table[i].next = i + 1;
      table[i].prev = i - 1;
   }

   table[MAXTIMER - 1].next = MAXTIMER - 1;
   xTaskCreate(TaskTimerSort,
               "SortTask", 50, NULL,
               configMAX_PRIORITIES - 2,
               &xTimerSortTaskHandle);
   xTaskCreate(TaskClacker,
               "ClackerTask", 200, NULL,
               configMAX_PRIORITIES - 1,
               &xClackerTaskHandle); //stack 50 war nicht nur schlecht
   tablestate.tableinitialized = 1;
}


typedef void critical;//just a reminder, not an actual type
static critical dangle(const timer_t thatindex) {
   struct tableentry * const that = &table[thatindex];

   //the free list is handled by the timer_create/delete pair
   if (thatindex == lastunsorted) {
      lastunsorted = that->prev;
   }

   if (thatindex == firstunsorted) {
      firstunsorted = that->next;
   }

   if (thatindex == firstactive) {
      firstactive = that->next;
   }

   if (that->next == that->prev) {
      if (thatindex == firstactive) {
         tablestate.active = 0;
      }

      if (thatindex == firstunsorted) {
         tablestate.unsorted = 0;
      }
   }

   that->next = that->prev = thatindex;
   tablestate.sorttaskreset = 1;
}

/*the timer must be dangling already*/
static critical putfirstunsorted(const timer_t thatindex) {
   struct tableentry *const that = &table[thatindex];

   if (!tablestate.unsorted) {
      firstunsorted = lastunsorted = thatindex;
   }

   that->next = firstunsorted;
   table[that->next].prev = thatindex;
   firstunsorted = thatindex;
   tablestate.unsorted = 1;
}

int timer_create(clockid_t clock_id,
                 struct sigevent *__restrict evp,
                 timer_t *__restrict timerid) {
   /*[EINVAL] The specified clock ID is not defined.*/
   if (!tablestate.tableinitialized) {
      timer_init();
   }

   ENTERCRITICAL;

   if (tablestate.full) {
      LEAVECRITICAL;
      errno = EAGAIN;
      return -1;
   }

   *timerid = firstfree;
   struct tableentry *const that = &table[firstfree];

   if (that->next == firstfree) {
      tablestate.full = 1;
   }

   firstfree = that->next;
   that->evp = evp;
   //dangle the timer, this is the first timer in a list
   table[that->next].prev = that->next;
   that->next = *timerid;
   that->prev = *timerid;
   LEAVECRITICAL;
   return 0;
}

int timer_settime(const timer_t timerid, const int flags,
                  const struct itimerspec *const value,
                  struct itimerspec *const ovalue) {
   struct tableentry *const that = &table[timerid];
   uint64_t nsectimestamp;

   nsec_clock_gettime(&nsectimestamp);
//printf("timer_settime(tid=%d val=%ld:%ld rept=%ld:%ld\n", timerid,
   value->it_value.tv_sec, value->it_value.tv_nsec,
         value->it_interval.tv_sec, value->it_interval.tv_nsec);

   if (timerid > MAXTIMER) {
   errno = EINVAL;
   return -1;
}

if (checkitimerspec(value) == -1) {
      errno = EINVAL;
      return -1;
   }

   struct internaltimerspec loadvalue, ovalprep;

   itimerspectointernaltimerspec(value, &loadvalue);

   if (!loadvalue.nsec_value) {
   loadvalue.nsec_value = loadvalue.nsec_interval;
}

if (flags != TIMER_ABSTIME && loadvalue.nsec_value) {
   loadvalue.nsec_value += nsectimestamp;
}

ENTERCRITICAL;
ovalprep = that->value;
that->value = loadvalue;
dangle(timerid);

if (loadvalue.nsec_value) {
   putfirstunsorted(timerid);
   }

   LEAVECRITICAL;

   if (ovalue) {
   ovalprep.nsec_value -= nsectimestamp;
   internaltimerspectoitimerspec(&ovalprep, ovalue);
   }

   if (xTimerSortTaskHandle) {
   vTaskResume(xTimerSortTaskHandle);
   }

   return 0;
}

int timer_delete(timer_t thatindex) {
   if (thatindex >= MAXTIMER) {
      errno = EINVAL;
      return -1;
   }

   ENTERCRITICAL;
   struct tableentry *const that = &table[thatindex];
   dangle(thatindex);

   //fixup free list:
   //  put the freed timer as the first free of the chain of free timers
   if (!tablestate.full) {
      that->next = firstfree;
   }

   firstfree = thatindex;
   table[that->next].prev = thatindex;
   tablestate.full = 0;
   that->value.nsec_value = that->value.nsec_interval = 0;
   LEAVECRITICAL;
   return 0;
}

int timer_gettime(timer_t timerid, struct itimerspec *value) {
   if (timerid > MAXTIMER) {
      errno = EINVAL;
      return -1;
   }

   uint64_t nsectimestamp;
   (*nsec_clock_gettime)(&nsectimestamp);

   struct internaltimerspec valprep;

   ENTERCRITICAL;
   valprep = table[timerid].value;
   LEAVECRITICAL;

   valprep.nsec_value -= nsectimestamp;
   internaltimerspectoitimerspec(&valprep, value);
   return 0;
}

int timer_getoverrun(timer_t timerid) {
   errno = ENOTSUP;
   return -1;
}


/*
*/

#endif
