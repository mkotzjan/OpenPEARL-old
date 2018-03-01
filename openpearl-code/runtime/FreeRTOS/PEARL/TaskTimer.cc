/*
 [The "BSD license"]
 Copyright (c) 2015 Rainer Mueller, Jonas Meyer
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
\author Jonas Meyer

*/

#include <string.h>
#include <errno.h>
#include <unistd.h>

#define _POSIX_TIMERS
#include <time.h>
#include <errno.h>

#include "Signals.h"
#include "TaskTimer.h"
#include "Log.h"
#include "Task.h"

// with gcc 7.2.1 this causes an error 
//struct sigevent {}; //shoehorn for casting

namespace pearlrt {
   void TaskTimer::update() {
      //check for endcondition for during
      if (counts < 0) {
         // do it eternally
      } else {
         counts --;

         if (counts == 0) {
            // became zero --> stop timer
            if (stop()) {
               // error during cancellation
               // ?????????????????????????
            }
         }
      }

      callback(task);
      return;
   }

   TaskTimer::TaskTimer() {
      counts = 0;
      countsBackup = 0;
   }

   static void freeRtosTimerCallback(void* tt_ptr) {
      TaskTimer* tt = (TaskTimer*)tt_ptr;
      tt->update();
   }

   void TaskTimer::create(TaskCommon * task, int signalNumber,
                          TimerCallback cb) {

      this->timer_callback.cb = (void *)&freeRtosTimerCallback;
      this->timer_callback.th = (void *)this;
      this->signalNumber = signalNumber;
      this->callback = cb;
      this->task = task;

      if (timer_create(CLOCK_REALTIME,
                       (sigevent *)&timer_callback,
                       &timer) == -1) {
         Log::error("task %s: could not create timer for signal %d",
                    task->getName(), signalNumber);
         throw theInternalTaskSignal;
      }
   }

   void TaskTimer::set(int condition,
                       Clock at, Duration after, Duration all,
                       Clock until, Duration during) {
      int counts;
      static Duration nullDelay(0);
      static Duration oneDay(24 * 60 * 60);
      Clock now = Clock::now();

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

      its.it_value.tv_sec = after.getSec();
      its.it_value.tv_nsec = after.getUsec() * 1000;
      // calculate repetition counter for the schedule
      its.it_interval.tv_sec = 0;
      its.it_interval.tv_nsec = 0;

      // calculate repetition counts
      if (condition & TaskCommon::ALL) {
         if ((all <= nullDelay).getBoolean()) {
            Log::error("Task %s: negative value at ALL", task->getName());
            throw theIllegalSchedulingSignal;
         }

         its.it_interval.tv_sec = all.getSec();;
         its.it_interval.tv_nsec = all.getUsec() * 1000;

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
/* ignore delayed start for the interpretation of DURING -->
    theses lines must vanish
            if (condition & (TaskCommon::AT | TaskCommon::AFTER)) {
               during = during - after;
            }
*/
            if ((during <= nullDelay).getBoolean()) {
               Log::error("Task %s: negative (effective) value at DURING",
                          task->getName());
               throw theIllegalSchedulingSignal;
            }

            counts = (during / all).x + 1;
            Log::debug(
               "task %s: scheduled after=%.3f s all %.3f s %d times",
               task->getName(),
               its.it_value.tv_sec + its.it_value.tv_nsec / 1e9 ,
               its.it_interval.tv_sec + its.it_interval.tv_nsec / 1e9,
               counts);
         } else {
            counts = -1;
            Log::debug(
               "task %s: scheduled  after=%.3f s all %.3f s eternally",
               task->getName(),
               its.it_value.tv_sec + its.it_value.tv_nsec / 1e9 ,
               its.it_interval.tv_sec + its.it_interval.tv_nsec / 1e9);
         }
      }

      if ((condition & (TaskCommon:: AT | TaskCommon::ALL | TaskCommon::AFTER |
                        TaskCommon::UNTIL | TaskCommon::DURING)) != 0) {
         // timed activate/continue, set initial delay to repetion delay
         // if no initial delay is specified
         if (its.it_value.tv_sec == 0 && its.it_value.tv_nsec == 0) {
            its.it_value.tv_sec = its.it_interval.tv_sec;
            its.it_value.tv_nsec = its.it_interval.tv_nsec;
         }
      }

      countsBackup = counts;
   }

   int TaskTimer::start() {
      counts = countsBackup;   // restore number for triggeredActivate
      Log::info("%s: TaskTimer::start", task->getName());

      if (its.it_value.tv_sec != 0 || its.it_value.tv_nsec != 0) {
         if (timer_settime(timer, 0, &its, NULL) == -1) {
            Log::error("task %s: error setting schedule timer (%s)",
                       task->getName(), strerror(errno));
            return (-1);
         }
      }

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
      struct itimerspec its;

      its.it_value.tv_sec = 0;
      its.it_value.tv_nsec = 0;
      its.it_interval.tv_sec = 0;
      its.it_interval.tv_nsec = 0;
      counts = 0;

      //kill the timer
      if (timer_settime(timer, 0, &its, NULL) == -1) {
         Log::error(
            "task %s: error cancelling timer (%s)",
            task->getName(), strerror(errno));
         return (-1);
      }

      return 0;
   }



   int TaskTimer::cancel() {
      countsBackup = 0;
      return stop();
   }
}
