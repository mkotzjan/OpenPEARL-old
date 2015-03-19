/*
 [The "BSD license"]
 Copyright (c) 2015 Rainer Mueller
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

/**

\brief timer facility for tasking

\author R. Mueller

*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Signals.h"
#include "TaskTimer.h"
#include "Log.h"
#include "Task.h"

namespace pearlrt {

   void TaskTimer::update() {
      Log::info("%s: TaskTimer: update", task->getName());

      if (isInStartPeriod) {
         isInStartPeriod = false;

         if (cyclicPeriod > 0) {
            if (xTimerChangePeriod(timer, cyclicPeriod, 10) != pdPASS) {
               Log::error("could not set cycle period");
               throw theInternalTaskSignal;
            }
         }
      }

      //check for endcondition for during
      if (counts < 0) {
         // do it eternally
         if (xTimerStart(timer, 0) == pdFALSE) {
            Log::error("could not start timer");
         }
      } else {
         counts --;

         if (counts == 0) {
            // became zero --> stop timer
            if (stop()) {
               // error during cancellation
               // ?????????????????????????
            }
         } else {
            if (xTimerStart(timer, 0) == pdFALSE) {
               Log::error("could not start timer");
            }
         }
      }

      callback(task);
      return;
   }

   static void freeRtosTimerCallback(TimerHandle_t th) {
      TaskTimer *t = (TaskTimer*) pvTimerGetTimerID(th);
      t->update();
   }


   TaskTimer::TaskTimer() {
      counts = 0;
      countsBackup = 0;
      timer = 0;
      startPeriod = 0;
      cyclicPeriod = 0;

   }
   void TaskTimer::create(TaskCommon * task,
                          TimerCallback cb) {
      this->task = task;
      this->callback = cb;
      // the period is set to 1 tick to enshure timer creation in freertos
      // the period is updated befor the timer is started
      timer = xTimerCreate(NULL,   // const char * const pcTimerName,
                           1,      //const TickType_t xTimerPeriod,
                           pdFALSE, // const UBaseType_t uxAutoReload,
                           this,    // void * const pvTimerID,
                           freeRtosTimerCallback);

      if (timer == 0) {
         Log::error("%s: could not create timer", task->getName());
      }
   }

   void TaskTimer::set(int condition,
                       Clock at, Duration after, Duration all,
                       Clock until, Duration during) {
      int counts;
      static Duration nullDelay(0);
      static Duration oneDay(24 * 60 * 60);
      Clock now = Clock::now();

      isInStartPeriod = false;

      // calculate start delay
      if (condition & TaskCommon::AFTER) {
         if ((after <= nullDelay).getBoolean()) {
            Log::error("Task %s: negative value at AFTER", task->getName());
            throw theIllegalSchedulingSignal;
         }

         counts = 1;
      } else {
         after = Duration(0.0);  // start immedially
         counts = 0;
      }

      if (condition & TaskCommon::AT) {
         after = at - now;

         if ((after < nullDelay).getBoolean()) {
            Log::warn("Task %s: negative value at AFTER --> next day",
                      task->getName());

            do {
               after += oneDay;
            } while ((after < 0).getBoolean());
         }

         counts = 1;
      }

      if (counts != 0) {
         // initial delay sepcified
         startPeriod = (after.get().get() / 1000L * configTICK_RATE_HZ) /
                       1000L ;

         if (startPeriod == 0) {
            Log::warn("%s: TaskTimer initial delay less than 1 tick"
                      " -> set to 1 tick", task->getName());
            startPeriod = 1;
         }
      }


      // calculate repetition counts
      if (condition & TaskCommon::ALL) {
         if ((all <= nullDelay).getBoolean()) {
            Log::error("Task %s: negative value at ALL", task->getName());
            throw theIllegalSchedulingSignal;
         }

         cyclicPeriod = (all.get().get() / 1000L * configTICK_RATE_HZ) / 1000L;

         if (cyclicPeriod == 0) {
            Log::warn("%s: TaskTimer cyclic delay less than 1 tick"
                      " -> set to 1 tick", task->getName());
            cyclicPeriod = 1;
         }

         if (condition & TaskCommon::UNTIL) {
            // transform absolute end time into relative duration
            during = until - now;

            // the AT-value is transformed into an AFTER some lines
            // above --> treat both
            if (condition & (TaskCommon::AT | TaskCommon::AFTER)) {
               during = during - after;
            }

            if ((during <= nullDelay).getBoolean()) {
               do {
                  during += oneDay;
               } while ((during < 0).getBoolean());
            }

            counts = (during / all).x + 1;
         } else if (condition & TaskCommon::DURING) {
            // the AT-value is transformed into an AFTER some lines
            // above --> treat both
            if (condition & (TaskCommon::AT | TaskCommon::AFTER)) {
               during = during - after;
            }

            if ((during <= nullDelay).getBoolean()) {
               Log::error("Task %s: negative (effective) value at DURING",
                          task->getName());
               throw theIllegalSchedulingSignal;
            }

            counts = (during / all).x + 1;
         } else {
            counts = -1;
         }
      }

      countsBackup = counts;
      Log::info("%s: TaskTimer is set; counts=%d start =%d cycle=%d",
                task->getName(),  counts, startPeriod,
                cyclicPeriod);
   }

   int TaskTimer::start() {
      counts = countsBackup;   // restore number for triggeredActivate

      if (startPeriod > 0) {
         if (xTimerChangePeriod(timer, startPeriod, 10) != pdPASS) {
            Log::error("could not set start period");
            return (-1);
         }

         isInStartPeriod = true;
      } else if (cyclicPeriod > 0) {
         if (xTimerChangePeriod(timer, cyclicPeriod, 10) != pdPASS) {
            Log::error("could not set cyclic period");
            return (-1);
         }
      }

      // retrigger the timer if running - else just start the timer
      if (xTimerReset(timer, 10) == pdFALSE) {
         Log::error("could not start timer");
         return (-1);
      }

      Log::info("%s: TaskTimer::started", task->getName());

      return 0;
   }

   bool TaskTimer::isActive() {
      return counts != 0;
   }

   bool TaskTimer::isSet() {
      return countsBackup != 0;
   }

   int TaskTimer::stop() {
      // need local variable to keep time settings of the time
      // unchanged for new start of the timer
      counts = 0;

      //kill the timer
      if (xTimerStop(timer, 0) == pdFALSE) {
         Log::error("could not stop timer");
         return (-1);
      }

      return 0;
   }



   int TaskTimer::cancel() {
      countsBackup = 0;
      return stop();
   }



   /*---------------------------------------------------------------*/
   /* ---------------- linux specific extensions below -------------*/

   void TaskTimer::init(int p) {


   }



   void TaskTimer::detailedStatus(char *id, char * line) {
      float rept;

      rept = ((float)cyclicPeriod) / configTICK_RATE_HZ;

      if (counts > 0) {
         sprintf(line,
                 "%s : all %.1f sec : %d times remaining",
                 id, rept, counts);
      } else {
         sprintf(line,
                 "%s : all %.1f sec : eternally",
                 id, rept);
      }

   }

}
