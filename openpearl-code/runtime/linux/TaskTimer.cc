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
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <bits/local_lim.h>  // PTHREAD_STACK_MIN
#include <time.h>
#include <errno.h>
#include <sys/signalfd.h>  // signalfd

#include "Signals.h"
#include "UnixSignal.h"
#include "TaskTimer.h"
#include "Log.h"
#include "Task.h"

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

   void TaskTimer::create(TaskCommon * task, int signalNumber,
                          TimerCallback cb) {
      struct sigevent sev;

      this->signalNumber = signalNumber;
      this->callback = cb;
      this->task = task;
      sev.sigev_notify = SIGEV_SIGNAL;
      sev.sigev_signo = signalNumber;
      sev.sigev_value.sival_ptr = this;

      if (timer_create(CLOCK_REALTIME, &sev, &timer) == -1) {
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
            if (condition & (TaskCommon::AT | TaskCommon::AFTER)) {
               during = during - after;
            }

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



   /*---------------------------------------------------------------*/
   /* ---------------- linux specific extensions below -------------*/

   static void* signalThreadCode(void*);
   void TaskTimer::init(int p) {


      if (SIGRTMAX >= SIGRTMIN + 4) {
         Log::info("System reports: SIGRTMIN SIGRTMAX range is [%d,%d]: ok",
                   SIGRTMIN, SIGRTMAX);
      } else {
         Log::info("System reports: SIGRTMIN SIGRTMAX range is [%d,%d]: "
                   ": not enough RT-Signals free",
                   SIGRTMIN, SIGRTMAX);
         fprintf(stderr, "not enough RT-Signal free");
         exit(1);
      }

      // block signals, which are treated by signalThread
      sigset_t set;
      sigemptyset(&set);
      sigaddset(&set, SIGRTMIN + 1);  // activate
      sigaddset(&set, SIGRTMIN + 2);  // resume
      sigaddset(&set, SIGRTMIN + 3);  // continue
      sigaddset(&set, SIGRTMIN);      // suspend

      if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) {
         Log::error("timerTask: error blocking signals");
         throw theInternalTaskSignal;
      }

      pthread_attr_t signalThreadAttrib;
      pthread_t signalThreadPid;
      sched_param signalThreadParam;
      int ret;

      if (pthread_attr_init(&signalThreadAttrib) != 0) {
         Log::error("timerTask: error initializing pthread_attr");
         throw theInternalTaskSignal;
      }

      ret = pthread_attr_setstacksize(&signalThreadAttrib, PTHREAD_STACK_MIN);

      if (ret < 0) {
         Log::error("timerTask: error setting pthread_attr_stacksize");
         throw theInternalTaskSignal;
      }

      //no inheritance of the main thread priority
      ret = pthread_attr_setinheritsched(&signalThreadAttrib,
                                         PTHREAD_EXPLICIT_SCHED);

      if (ret != 0) {
         Log::error("timerTask: error setting pthread inheritance attributes");
         throw theInternalTaskSignal;
      }

      if (p != -1) {
         ret = pthread_attr_setschedpolicy(&signalThreadAttrib, SCHED_RR);

         if (ret != 0) {
            Log::error("timerTask: error setting SCHED_RR scheduler");
            throw theInternalTaskSignal;
         }

         signalThreadParam.sched_priority = p;
         ret = pthread_attr_setschedparam(&signalThreadAttrib,
                                          &signalThreadParam);

         if (ret != 0) {
            Log::error(
               "timerTask: error on setting priority");
            throw theInternalTaskSignal;
         }
      }

      //create the thread
      if (pthread_create(&signalThreadPid, &signalThreadAttrib,
                         signalThreadCode,
                         NULL) != 0) {
         Log::error("signalThread: could not create thread (%s))",
                    strerror(errno));
         throw theInternalTaskSignal;
      }

   }


   static void* signalThreadCode(void* dummy) {
      int fds;
      int ret;
      ret = 0;
      ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

      if (ret != 0) {
         Log::error("taskTimer: error on setting cancellation type");
         throw theInternalTaskSignal;
      }

      sigset_t set;
      sigemptyset(&set);
      sigaddset(&set, SIGRTMIN + 1);
      sigaddset(&set, SIGRTMIN + 2);
      sigaddset(&set, SIGRTMIN + 3);
      UnixSignal::updateSigMask(&set);

      //sigaddset(&set, SIGRTMIN);
      if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) {
         Log::error("timerTask: error blocking signals");
         throw theInternalTaskSignal;
      }

      fds = signalfd(-1, &set, 0);

      if (fds == -1) {
         Log::error("timerTask: error opening signalfd");
         throw theInternalTaskSignal;
      }

      while (1) {
         /* The buffer for read(), this structure contains information
         about the signal we've read. */
         struct signalfd_siginfo si;
         ssize_t res;
         res = read(fds, &si, sizeof(si));

         if (res < 0) {
            Log::error("timerTask: read error (%s)", strerror(errno));
         }

         if (res != sizeof(si)) {
            Log::error("timerTask: got wrong size");
         }

         TaskTimer * t = (TaskTimer*)si.ssi_ptr;

         if (si.ssi_signo == (uint32_t)SIGRTMIN + 1) {  // activate
            t->update();
         } else if (si.ssi_signo == (uint32_t)SIGRTMIN + 3) {   // continue
            t->update();
         } else if (UnixSignal::treat(si.ssi_signo)) {
            // do nothing here - action is in treat()-method
         } else {
            Log::error("signalThread: got unexpeceted signal %d",
                       si.ssi_signo);
         }
      }

      return NULL;  // never reached
   }

   void TaskTimer::detailedStatus(char *id, char * line) {
      struct itimerspec its;
      float next, rept;

      timer_gettime(timer, &its);
      next = its.it_value.tv_sec + its.it_value.tv_nsec / 1.e9;
      rept = its.it_interval.tv_sec + its.it_interval.tv_nsec / 1.e9;

      if (counts > 0) {
         sprintf(line,
                 "%s next %.1f sec : all %.1f sec : %d times remaining",
                 id, next, rept, counts);
      } else {
         sprintf(line,
                 "%s next %.1f sec : all %.1f sec : eternally",
                 id, next, rept);
      }

   }

}
