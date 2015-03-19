/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Rainer Mueller
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

\brief tasking

\author R. Mueller

*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <bits/local_lim.h> // PTHREAD_STACK_MIN
#include <sched.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "Task.h"
#include "Log.h"
#include "TaskList.h"
#include "TaskMonitor.h"
#include "Interrupt.h"
#include "PrioMapper.h"
#include "BitString.h"
#include "Semaphore.h"

namespace pearlrt {

   Task::Task() : TaskCommon(NULL, Prio(), BitString<1>(0)) {
      Log::error("task %s: illegal call to default constructor of Task::",
                 name);
   }

   // tasks are created statically - no mutex protection needed
   Task::Task(void (*entry)(Task *),
              char * n,
              Prio prio, BitString<1> isMain) : TaskCommon(n, prio, isMain) {
      int ret;

      entryPoint = entry;   //set thread execute function

      schedActivateData.taskTimer = &activateTaskTimer;
      schedContinueData.taskTimer = &continueTaskTimer;

      //create activate timer for SIGRTMIN+1 if expired and
      //create continue timer for SIGRTMIN+3 if expired
      ((TaskTimer*)schedActivateData.taskTimer)->create(
         this, SIGRTMIN + 1, activateHandler);
      ((TaskTimer*)schedContinueData.taskTimer)->create(
         this, SIGRTMIN + 3, continueHandler);

      if (pthread_attr_init(&attr) != 0) {
         Log::error("task %s: error initializing pthread_attr", name);
         throw theInternalTaskSignal;
      }

      if (pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN) < 0) {
         Log::error("task %s: error setting pthread_attr_stacksize", name);
         throw theInternalTaskSignal;
      }

      //no inheritance of the main thread priority
      if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0) {
         Log::error("task %s: error setting pthread inheritance attributes",
                    name);
         throw theInternalTaskSignal;
      }

      ret = pipe(pipeResume);

      if (ret) {
         Log::error("task %s: error creating pipeResume (%s)",
                    name, strerror(errno));
         throw theInternalTaskSignal;
      }

      TaskList::Instance().add(this);
   }

   // call with locked task lock
   void Task::continueFromOtherTask(
      int condition,
      Prio prio) {
      // this function is called with locked tasks mutex
      // unlocking is done in calling function, except
      // this function throws an exception
      switch (taskState) {
      case SEMA_SUSPENDED_BLOCKED:
         // reinsert the task in the semaphores wait queue
         taskState = SEMA_BLOCKED;
         Semaphore::addToWaitQueue(this);

      // no break here! setting the priority is included
      //    in RUNNING/SUSPENDED
      case SEMA_BLOCKED:

         // adjust the tasks position in the semaphores wait queue
         // according the tasks (new) priority
         if (condition & PRIO) {
            currentPrio = prio.get();
         }

         Semaphore::updateWaitQueue(this);

      // no break here! setting the priority is included
      //    in RUNNING/SUSPENDED
      case RUNNING:

         // adjust the tasks priority according the tasks (new) priority
         try {
            if (condition & PRIO) {
               changeThreadPrio(prio.get());   // just set new priority
            }
         } catch (Signal s) {
            mutexUnlock();
            throw;
         }

//????         mutexUnlock();

         break;

      case SUSPENDED:
//         try {
//            if (condition & PRIO) {
//               changeThreadPrio(prio.get());   // just set new priority
//            }
//         } catch (Signal s) {
//            mutexUnlock();
//            throw;
//         }
//
//         sendContinueCondition();
         continueSuspended();

         break;

      case IO_BLOCKED:

         if (asyncSuspendRequested) {
            Log::debug("   asyncSuspendRequest resetted");
            asyncSuspendRequested = false;
         }

         try {
            if (condition & PRIO) {
               Log::debug("   set new prio in IO_BLOCKED");
               changeThreadPrio(prio.get());  // set new priority
            }
         } catch (Signal s) {
            mutexUnlock();
            throw;
         }

//        mutexUnlock();
         break;

      case IO_SUSPENDED_BLOCKED:
         Log::debug("   continue in IO_SUSPENDED_BLOCKED detected");

//         try {
//            if (condition & PRIO) {
//               Log::debug("   set new prio in IO_BLOCKED");
//               changeThreadPrio(prio.get());  // set new priority
//            }
//         } catch (Signal s) {
//            mutexUnlock();
//            throw;
//         }
//
//         sendContinueCondition();
         continueSuspended();

//         mutexUnlock();
         break;

      case TERMINATED:
         Log::error("task %s: continue at terminated state", name);
         mutexUnlock();
         throw theTaskTerminatedSignal;

      default:
         Log::error("   task %s: continue at untreated state %d",
                    name, taskState);
         mutexUnlock();
         throw theInternalTaskSignal;
      }
   }

   void Task::continueSuspended() {
      try {
         if (schedContinueData.prio.x > 0) {
            // just set new priority
            changeThreadPrio(schedContinueData.prio);
         }
      } catch (Signal s) {
         Log::error("   task %s: setting priority", name);
         mutexUnlock();
         throw;
      }

      Log::debug("   task %s: send continuation data",
                 name);
      char dummy = 'c';
      continueWaiters++;
      write(pipeResume[1], &dummy, 1);
      mutexUnlock();
      continueDone.request();
      Log::debug("   task %s: send continuation data: acknowledged",
                 name);
      mutexLock();
   }


   void Task::suspendFromOtherTask() {
      switch (taskState) {
      case SEMA_BLOCKED:
         // leave the task blocked on its block-semaphore
         // and remove it from the wait queue
         taskState = SEMA_SUSPENDED_BLOCKED;
         Semaphore::removeFromWaitQueue(this);
         break;

      case IO_BLOCKED:
         Log::debug("   task %s: set suspend request flag in IO_BLOCKED",
                    name);
         asyncSuspendRequested = true;
         switchToSchedPrioMax();
//???         mutexUnlock();
         // no jobDoneRequest request here!
         // the suspending task shall be delayed until the
         // io-doing task reaches the suspending point
         // jobDone.request();
         break;

      case RUNNING:
         asyncSuspendRequested = true;
         Log::debug("   task %s: set suspend request flag", name);
         suspendWaiters ++;
         mutexUnlock();
         switchToSchedPrioMax();
         suspendDone.request();
         mutexLock();
         break;

      default:
         Log::error("   suspendFromOtherTask taskState %d not treated",
                    taskState);
         throw theInternalTaskSignal;
      }
   }

   void Task::setSuspendRequested() {
      asyncSuspendRequested = true;
   }

   void Task::suspendMySelf() {
      internalSuspendMySelf(false);
   }

   void Task::internalSuspendMySelf(bool releaseJobDone) {
      char dummy;

      switch (taskState) {
      case RUNNING:
         taskState = Task::SUSPENDED;
         break;

      case IO_BLOCKED:
         taskState = IO_SUSPENDED_BLOCKED;
         break;

      default:
         Log::error("   Task::internalsSuspendMySelf: unknown taskState =%d",
                    taskState);
         mutexUnlock();
         throw theInternalTaskSignal;
      }


//      if (releaseJobDone) {
//         jobDone.release();
//      }
      while (suspendWaiters > 0) {
         suspendWaiters --;
         suspendDone.release();
      }

      Log::debug("   task %s: suspended (wait for contine data) ", name);
      mutexUnlock();
      read(pipeResume[0], &dummy, 1);

      mutexLock();
      Log::debug("   suspendMySelf:: %s got data %c", name, dummy);

      switch (dummy) {
      case 't' :
         internalTerminateMySelf(true);
         break;

      case 'c' :

         switch (taskState) {
         case SUSPENDED:
            taskState = Task::RUNNING;
            break;

         case IO_SUSPENDED_BLOCKED:
            taskState = IO_BLOCKED;
            break;

         default:
            Log::error(
               "   Task::internalSuspendMySelf: unknown taskState =%d",
               taskState);
            mutexUnlock();
            throw theInternalTaskSignal;
         }

         while (continueWaiters > 0) {
            continueWaiters --;
            continueDone.release();
         }

         Log::info("   task %s: continue from suspend done", name);
         switchToSchedPrioCurrent();
//???         mutexUnlock();
         break;

      default:
         Log::error("   task %s: resume: received unknown continue (%c)",
                    name, dummy);
         break;
      }
   }

   void Task::switchToSchedPrioMax() {
      switchThreadSchedPrio(schedPrioMax);
   }

   void Task::switchToSchedPrioCurrent() {
      changeThreadPrio(currentPrio);
   }

   void Task::switchThreadSchedPrio(int p) {
      struct sched_param sp;

      if (! useNormalSchedulerFlag) {
         param.sched_priority = p;
         sp.sched_priority = p;

         //setting up the new priority
         Log::debug("setting linux prio for task %s to %d",
                    name, param.sched_priority);

         if (pthread_setschedparam(threadPid, SCHED_RR, &sp)) {
            mutexUnlock();
            Log::error(
               "task %s: error on setting new priority",
               name);
            throw theInternalTaskSignal;
         }
      }
   }

   void Task::changeThreadPrio(const Fixed<15>& prio) {
      currentPrio = prio;

      if (! useNormalSchedulerFlag) {
         int p = PrioMapper::getInstance()->fromPearl(prio);
         switchThreadSchedPrio(p);
      }
   }

   void Task::scheduleCallback(bool isLocked) {

      if (asyncTerminateRequested) {
         if (! isLocked) {
            mutexLock();
         }

         asyncTerminateRequested = false;

         if (taskState == IO_BLOCKED) {
            dation->suspend();
         }

         internalTerminateMySelf(taskState != IO_BLOCKED);
      }

      if (asyncSuspendRequested) {
         Log::debug("Task %s: scheduledCallback : suspend request detected",
                    name);

         if (!isLocked) {
            mutexLock();
         }

         asyncSuspendRequested = false;

         // the paremeter denotes wether the suspending task should
         // be informed about reaching the suspended state
         // if the task is io-blocked, the suspending task does not wait
         // for the completion notice
         if (taskState == IO_BLOCKED) {
            dation->suspend();  // release dations mutex to allow other
            // tasks to work with this dation
         }

         internalSuspendMySelf(taskState != IO_BLOCKED);

         if (taskState == IO_BLOCKED) {
            dation->cont();     // lock the mutex again when
            // continuing the operation
         }

         if (!isLocked) {
            mutexUnlock();
         }
      }
   }

   void Task::entry() {
      //block timer signals, they should only delivered to main thread
      sigset_t set;
      sigemptyset(&set);
      asyncTerminateRequested = false;
      asyncSuspendRequested = false;
      suspendWaiters = 0;
      continueWaiters = 0;
      terminateWaiters = 0;
      // store own pid
      threadPid = pthread_self();
      taskState = Task::RUNNING;
      activateDone.release();
      switchToSchedPrioCurrent();
      Log::info("Task %s activation completed", name);
   }


   void Task::directActivate(const Fixed<15>& prio) {
      int ret;
      currentPrio = prio;

      //set up the new scheduling policies for preemptive priority
      if (!useNormalSchedulerFlag) {
         // the task will be activate with best priority. After
         // finished initialization it will fall back to currentPrio.
         // This happens in entry(). A non mapable priority must
         // give a signal to the executing task. Thats why we test
         // whether priority is mapable now.
         PrioMapper::getInstance()->fromPearl(currentPrio);

         // the possibility of setting SCHED_RR is detected at
         // system start. Therefore this attribute is not set
         // statically in the constructor.
         if (pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0) {
            Log::error("task %s: error setting SCHED_RR scheduler", name);
            throw theInternalTaskSignal;
         }

         // start with maximum priority --> switch to given
         // priority occurs in thread start routine
         param.sched_priority = schedPrioMax;

         if (pthread_attr_setschedparam(&attr, &param) != 0) {
            Log::error("task %s: error on setting priority", name);
            throw theInternalTaskSignal;
         }
      }

      ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

      if (ret != 0) {
         Log::error("task %s: error on setting cancellation type", name);
         throw theInternalTaskSignal;
      }


      //create the thread
      if (pthread_create(&threadPid, &attr,
                         (void * (*)(void*))entryPoint,
                         (void*)this) != 0) {
         Log::error("task %s: could not create thread (%s))",
                    name, strerror(errno));
         throw theInternalTaskSignal;
      }

      activateDone.request();

      return;
   }

   int Task::detailedTaskState(char * line[3]) {
      int i = 0;
      char help[20];
      mutexLock();

      if (schedActivateData.taskTimer->isActive()) {
         ((TaskTimer*)(schedActivateData.taskTimer))->detailedStatus(
            (char*)"ACT", line[i]);
         i++;
      }

      if (schedContinueData.taskTimer->isActive()) {
         ((TaskTimer*)(schedContinueData.taskTimer))->detailedStatus(
            (char*)"CONT", line[i]);
         i++;
      }


      if (taskState == SEMA_BLOCKED) {
         switch (blockParams.why.reason) {
         case REQUEST:
            sprintf(line[i], "REQUESTing %d SEMAs:",
                    blockParams.why.u.sema.nsemas);

            for (int j = 0; j < blockParams.why.u.sema.nsemas; j++) {
               Semaphore * s = blockParams.why.u.sema.semas[j] ;
               sprintf(help, " %s(%d)", s->getName(), s->getValue());

               if (strlen(line[i]) + strlen(help) < 80) {
                  strcat(line[i], help);
               }
            }

            break;

         default:
            sprintf(line[i], "unknown blocking reason(%d)",
                    blockParams.why.reason);
            break;
         }

         i++;
      }

      mutexUnlock();
      return i;
   }

   void Task::terminateFromOtherTask() {
      char dummy;
      bool wait4completion = true;
      asyncTerminateRequested = true;
      Log::debug("   task %s: terminateFromOtherTask: state=%d  "
                 "asyncTerminateRequested := true", name, taskState);
      switchToSchedPrioMax();

      switch (taskState) {
      case IO_BLOCKED:
         // perform a pthread_cancel/pthread_join
         // to enshure immediate termination, which is possible here
         /*
         Log::debug("   task %s: terminateFromOtherTask: do pthread_cancel",
                             name);
         Log::debug("   threadPid=%x", (int)threadPid);
         ret = pthread_cancel(threadPid);

         if (ret != 0) {
             Log::error("   task %s: error in pthread_cancel (%s)",
                         name, strerror(ret));
             throw theInternalTaskSignal;
         }
         dation->suspend();

         Log::debug("task %s: terminateFromOtherTask: do pthread_join", name);
         Log::debug("   threadPid=%x", (int)threadPid);
         ret = pthread_join(threadPid, NULL);

         if (ret != 0) {
            Log::error("task %s: error in pthread_join (%s)",
                         name, strerror(ret));
            throw theInternalTaskSignal;
         }
         Log::debug("task %s: terminateFromOtherTask: do pthread_join done",
                     name);
         */
         // unlocking of the dations lock is not guaranteed
         wait4completion = false;
         mutexUnlock();
         break;

      case SEMA_BLOCKED:
         terminateWaiters ++;   // increment before treatment from target task
         Semaphore::removeFromWaitQueue(this);
         taskState = Task::RUNNING;
         blockParams.semaBlock.release();
         mutexUnlock();
         break;

      case SUSPENDED:
         terminateWaiters ++;   // increment before treatment from target task
         Log::info("   task %s: terminateRemote suspended task", name);
         dummy = 't';
         mutexUnlock();
         write(pipeResume[1], &dummy, 1);
         break;

      case RUNNING:
         terminateWaiters ++;   // increment before treatment from target task

         //resetTimer(&schedContinueData, true);
//         if (schedContinueData.taskTimer->cancel()) {
//            throw theInternalTaskSignal;
//         }

         mutexUnlock();
         break;

      case SEMA_SUSPENDED_BLOCKED:
         terminateWaiters ++;   // increment before treatment from target task
         Log::debug("   task %s: terminateRemote susp_blocked task", name);
         taskState = Task::RUNNING;
         blockParams.semaBlock.release();  // unblock
         mutexUnlock();
         break;

      default:
         Log::error("   task %s: unhandled taskState (%d) at TERMINATE",
                    name, taskState);
         mutexUnlock();
         throw theInternalTaskSignal;
      }


      if (wait4completion) {
         Log::debug("   task %s: terminateRemote: wait for completion", name);
         terminateDone.request();
         Log::info("   task %s: terminateRemote: done", name);
      } else {
         Log::info("   task %s: terminateRemote: pending", name);
      }

   }

   void Task::terminateMySelf() {
      internalTerminateMySelf(false);
   }

   void Task::internalTerminateMySelf(bool releaseJobDone) {
      Log::debug("   task %s: terminateMySelf: start", name);
      taskState = Task::TERMINATED;
      threadPid = 0; // invalidate threadid

      Log::info("   task %s: terminates", name);

      if (suspendWaiters > 0) {
         Log::warn("   Task %s terminates while %d other tasks wait"
                   " to suspend it",
                   name, suspendWaiters);
      }

      while (suspendWaiters > 0) {
         suspendWaiters --;
         suspendDone.release();
      }

      if (terminateWaiters > 0) {
         Log::warn("   Task %s terminates while %d other tasks wait"
                   " to terminate it",
                   name, terminateWaiters);
      }

      while (terminateWaiters > 0) {
         terminateWaiters --;
         terminateDone.release();
      }

      if (schedActivateOverrun) {
         // missed one scheduled activation --> do it immediatelly now
         Log::debug("   Task %s terminates with missed scheduled"
                    " activate pending", name);
         schedActivateOverrun = false;
         directActivate(schedActivateData.prio);
         mutexUnlock();
      } else {
         /* still pending ? */
         //if (schedActivateData.counts == 0 &&
         if (schedActivateData.taskTimer->isActive() == false &&
               schedActivateData.whenRegistered == false) {
            mutexUnlock();
            TaskMonitor::Instance().decPendingTasks();
         } else {
            mutexUnlock();
         }
      }

      pthread_exit(0);
   }

   void Task::resume2() {
      char dummy;

      taskState = Task::SUSPENDED;

      Log::debug("task %s: resume: going to suspended", name);
      mutexUnlock();
      read(pipeResume[0], &dummy, 1);
      mutexLock();

      // the continue handler keeps the mutex locked after sending
      // the data --> unlocking of the mutex must be done at the
      // end of treatment here
      Log::debug("task %s: resume: received continue (%c)", name, dummy);

      switch (dummy) {
      case 't' :
         internalTerminateMySelf(true);
         break;

      case 'c' :
         taskState = Task::RUNNING;
         switchToSchedPrioCurrent();

         while (continueWaiters > 0) {
            continueWaiters --;
            continueDone.release();
         }

         break;

      default:
         Log::error("task %s: resume: received unknown continue (%c)",
                    name, dummy);
         break;
      }

   }

   int Task::schedPrioMax = 0;

   void Task::setSchedPrioMax(int p) {
      schedPrioMax = p;
   }

   int Task::useNormalSchedulerFlag = 0;

   void Task::useNormalScheduler() {
      useNormalSchedulerFlag = 1;
   }

   pthread_t Task::getPid() {
      return threadPid;
   }

   Task::TaskEntry Task::getEntry() {
      return entryPoint;
   }

}
