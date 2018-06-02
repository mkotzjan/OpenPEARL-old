/*
 [A "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
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

/*
 * Task.cc
 *
 *  Created on: 08.02.2014
 *  Author: Florian Mahlecke
 */


/**
 \file

 \brief Base class for FreeRTOS based tasks

 */

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "allTaskPriorities.h"

#include "TaskMonitor.h"
#include "TaskList.h"
#include "PrioMapper.h"
#include "Semaphore.h"
#include "Log.h"
#include "service.h"
#include "Task.h"

namespace pearlrt {

   /*********************************************************************//**
    * @brief		Constructor for FreeRTOS based tasks
    * @param[in]	n task name
    * @param[in]	prio PEARL90 priority
    * @param[in]	ismain PEARL90 isMain flag
    *
    * <b>Note:</b> When using UART in BLOCKING mode,
    *               a time-out condition is used
    * via defined symbol UART_BLOCKING_TIMEOUT.
    **********************************************************************/

   Task::Task(char* n, Prio prio, BitString<1> ismain) :
      TaskCommon(n, prio, ismain) {

      schedActivateData.taskTimer = &activateTimer;
      schedContinueData.taskTimer = &continueTimer;

      // FreeRTOS part
      stackDepth = STACK_SIZE;
      xth = pdFALSE;
      TaskList::Instance().add(this);
   }

   void Task::init() {
      ((TaskTimer*)schedActivateData.taskTimer)->create(this,
            0,
            activateHandler);
      ((TaskTimer*)schedContinueData.taskTimer)->create(this,
            0,
            continueHandler);
   }

   void Task::directActivate(const Fixed<15>& prio) {
      bool freeRtosRunning;
      //StructParameters_t taskParams;
      int cp;  // current prio of calling task

      int freeRtosPrio = PrioMapper::getInstance()->fromPearl(prio.x);

      Log::debug("%s::directActivate freeRTOSprio=%d", name, freeRtosPrio);

      BaseType_t taskCreation = pdFALSE;

      /*
      note: the call of TaskMonitor::...::incPendingTasks()
            is already done in Taskcommon::activate()
      */

      // the scheduling may only be stopped, if the scheduler
      // was started - the creation of MAIN-tasks need no
      // stop/start of the scheduler, since FreeRTOS is started later
      freeRtosRunning =
         (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);

      if (freeRtosRunning) {
         cp = switchToThreadPrioMax();
      }

      //taskParams.pvParameter = (void*) this;
      //taskParams.tcb = &tcb;
      //taskParams.stack = stack;

      //taskCreation = xTaskCreate(&tskfunc,
      //                           (const char*) this->name,
      //                           this->stackDepth, &taskParams,
      //                           freeRtosPrio, &xth);
      //taskCreation = xTaskCreate(&test, (const char*) this->name, this->stackDepth, NULL, freeRtosPrio, &xth);
      xth = xTaskCreateStatic(
			&tskfunc,
			(const char*) this->name,
			this->stackDepth,
			(void*) this,
			freeRtosPrio,
			stack,
			((StaticTask_t*) &tcb));

      if (xth) {
         Log::debug("%s: started", name);
         taskState = RUNNING;
      } else {
         Log::error("%s: not created", name);
         throw theInternalTaskSignal;
      }

      Log::debug("%s::activated ", name);

      if (freeRtosRunning) {
         switchToThreadPrioCurrent(cp);
      }
   }

   void Task::resume2() {
      suspendMySelf();
   }

   void Task::entry() {
      schedActivateOverrun = false;
   }

   void Task::tskfunc(void* param) {
      Task* me = ((Task*)param);
      me->entry();

      Log::debug("%s: starts", me->getName());

      try {
         me->task(me);
      } catch (Signal & p) {
         {
            int f = uxTaskGetStackHighWaterMark(NULL);
            printf("Task stack usage: %d\n", f);
         }
         Log::error("++++++++++++++++++++++++++++++");
         Log::error("%s:%d Task: %s   terminated due to: %s",
                    me->getLocationFile(), me->getLocationLine(),
                    me->getName(), p.which());
         Log::error("++++++++++++++++++++++++++++++");
      }

      me->terminate(me);
   }


   void Task::terminateMySelf() {
      TaskHandle_t oldTaskHandle = xth;
      Log::debug("%s: terminateSelf", name);

      // set the calling tasks priority to maximum to be shure that
      // the following code - especially the restarting of the task itself
      // until vTaskDelete() is executed without task switch
      switchToThreadPrioMax();

      if (schedActivateOverrun) {
         /* leave the tasks state to NOT TERMINATED.
            This make the system insensitiv to retriggering the
            task activation during the time until the automatic restart
            of the task
         */
         ServiceJob s = {(void (*)(void *))restartTaskStatic, this};

         Log::debug("%s: terminates with schedOverrun flag - start task again",
                    name);

         /* note: the current task prio is "RunToCompletion"
                  there will be no taskswitch on a single core
                  cpu, when the service tasks receives the job
                  to start the task --> this tasks completes and
                  then it will be restarted
         */
         printf("dispatch restart\n");
         add_service(&s);
      } else {
         /* set the tasks state to terminated */
         taskState = TERMINATED;

         if (schedActivateData.taskTimer->isActive() == false &&
               schedActivateData.whenRegistered == false) {
            TaskMonitor::Instance().decPendingTasks();
         } else {
            // do nothing
         }
      }
#if 0
      // this block is useful for stack optimizations
      {
         int f = uxTaskGetStackHighWaterMark(NULL);
         printf("Task %s stack usage: %d\n", name, f);
      }
#endif
      mutexUnlock();
      vTaskDelete(oldTaskHandle);
   }

   void Task::terminateFromOtherTask() {
      Log::debug("%s: terminateFromOtherTask", name);
      int cp; // current calling tasks priority

      // set the calling threads priority to maximum priority
      // to enshure the execution of this function without task switch
      cp = switchToThreadPrioMax();

      switch (taskState) {
      case IO_BLOCKED:
         // unlocking of the dations lock is not guaranteed
         Log::warn("terminate remote in i/o blocked not supported");
         break;

      case SEMA_BLOCKED:
         // update the OpenPEARL internal list
         Semaphore::removeFromWaitQueue(this);

         // in FreeRTOS it is easy to kill a task - even if it waits for
         // a semaphore. No need to continue and kill it
         //     blockParams.semaBlock.release(); // this statement is required 
         // only in the linux environment. I leave this reminder to make
         // clear that this is no copy error
         break;

      case SUSPENDED:
         // no problem - just kill
         break;

      case RUNNING:
         break;

      case SEMA_SUSPENDED_BLOCKED:
         Log::debug("   task %s: terminateRemote susp_blocked task", name);

         // in FreeRTOS it is easy to kill a task - even if it waits for
         // a semaphore. No need to continue and kill it
         //     blockParams.semaBlock.release(); // this statement is required 
         // only in the linux environment. I leave this reminder to make
         // clear that this is no copy error
         break;

      default:
         Log::error("   task %s: unhandled taskState (%d) at TERMINATE",
                    name, taskState);
         mutexUnlock();
         throw theInternalTaskSignal;
      }


      taskState = TERMINATED;
      vTaskDelete(this->xth);

      // test if the a scheduled activation was not performed due to 
      // the task was still active. Restart the task right now
      if (schedActivateOverrun) {
         schedActivateOverrun = false;
         directActivate(schedActivateData.prio);
      } else {
         if (schedActivateData.taskTimer->isActive() == false &&
               schedActivateData.whenRegistered == false) {
            mutexUnlock();
            TaskMonitor::Instance().decPendingTasks();
         } else {
            mutexUnlock();
         }
      }

      switchToThreadPrioCurrent(cp);
   }

   void Task::suspendMySelf() {
      int cp; // current calling threads priority
      Log::debug("%s: suspendMyself  taskState=%d", name, taskState);

      switch (taskState) {
      case RUNNING:
         taskState = Task::SUSPENDED;
         break;

      default:
         Log::error("   Task::suspendMySelf: unknown taskState =%d",
                    taskState);
         mutexUnlock();
         throw theInternalTaskSignal;
      }


      // set the priority of this thread to maxium priority to
      // enshure the consecutive operation until at least vTaskSuspend()
      // after continuation the thread is executed until the active task
      // state is set again.
      cp = switchToThreadPrioMax();
      mutexUnlock();
      vTaskSuspend(xth);
      mutexLock();

      taskState = RUNNING;
      switchToThreadPrioCurrent(cp);

      Log::debug("   task %s: continue from suspend done", name);

   }

   void Task::suspendFromOtherTask() {
      int cp; // current calling threads priority

      switch (taskState) {
      case SEMA_BLOCKED:
         taskState = SEMA_SUSPENDED_BLOCKED;
         Semaphore::removeFromWaitQueue(this);
         break;

      case IO_BLOCKED:
         Log::debug("   task %s: set suspend request flag in IO_BLOCKED",
                    name);
//         asyncSuspendRequested = true;
//         switchToSchedPrioMax();
//         mutexUnlock();
         // no jobDoneRequest request here!
         // the suspending task shall be delayed until the
         // io-doing task reaches the suspending point
         // jobDone.request();
         break;

      case RUNNING:
         Log::debug("   task %s: suspend request in RUNNING mode", name);

         cp = switchToThreadPrioMax();
         taskState = SUSPENDED;
         vTaskSuspend(this->xth);
         switchToThreadPrioCurrent(cp);

         break;

      default:
         Log::error("   suspendFromOtherTask taskState %d not treated",
                    taskState);
         mutexUnlock();
         throw theInternalTaskSignal;
      }
   }


   void Task::continueFromOtherTask(int condition, Prio prio) {
      int cp; // current calling threads priority
      // this may be the timer-task from FreeRTOS in case
      // of timed continue

      cp = switchToThreadPrioMax();

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
            // this switch block continues in the switch entry RUNNING
            // updating the threads priority is done there
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

         break;

      case SUSPENDED:
         continueSuspended();

         break;

      case IO_BLOCKED:
         Log::debug("   continue in IO_SUSPENDED_BLOCKED missing");

/////         if (asyncSuspendRequested) {
/////            Log::debug("   asyncSuspendRequest resetted");
/////            asyncSuspendRequested = false;
/////         }
/////
/////         try {
/////            if (condition & PRIO) {
/////               Log::debug("   set new prio in IO_BLOCKED");
/////               changeThreadPrio(prio.get());  // set new priority
/////           }
/////         } catch (Signal s) {
/////            mutexUnlock(); /7 Tasks.release();
/////            throw;
/////         }
         break;

      case IO_SUSPENDED_BLOCKED:
         Log::debug("   continue in IO_SUSPENDED_BLOCKED detected");

//         try {
//            if (condition & PRIO) {
//               Log::debug("   set new prio in IO_BLOCKED");
//               changeThreadPrio(prio.get());  // set new priority
//            }
//         } catch (Signal s) {
//         mutexUnlock();
//         switchToThreadPrioCurrent(cp);
//            throw;
//         }
//
//         sendContinueCondition();
         continueSuspended();
         break;

      case TERMINATED:
         Log::error("task %s: continue at terminated state", name);
         mutexUnlock();
         switchToThreadPrioCurrent(cp);
         throw theTaskTerminatedSignal;

      default:
         Log::error("   task %s: continue at untreated state %d",
                    name, taskState);
         mutexUnlock();
         switchToThreadPrioCurrent(cp);
         throw theInternalTaskSignal;
      }

      switchToThreadPrioCurrent(cp);
   }

   void Task::continueSuspended() {
      try {
         if (schedContinueData.prio.x > 0) {
            // just set new priority
            changeThreadPrio(schedContinueData.prio);
         }
      } catch (Signal s) {
         mutexUnlock();
         throw;
      }

      vTaskResume(xth);
      // update of taskState and release of mutexTask is done in
      // continued task
   }

   void Task::changeThreadPrio(const Fixed<15>& prio) {
      currentPrio = prio;

      int p = PrioMapper::getInstance()->fromPearl(prio);
      vTaskPrioritySet(xth, p);
   }

   int Task::switchToThreadPrioMax() {
      int cp;
      cp = uxTaskPriorityGet(NULL);
      vTaskPrioritySet(NULL, PRIO_TASK_MAX_PRIO);
      return cp;
   }

   void Task::switchToThreadPrioCurrent(int cp) {
      vTaskPrioritySet(NULL, cp);
   }

   TaskHandle_t Task::getFreeRTOSTaskHandle() {
      if (taskState != TERMINATED) {
         return xth;
      }

      return NULL;
   }

   void Task::restartTaskStatic(Task * t) {
      t->restartTask();
   }

   void Task::restartTask() {
      // directActivate must be called with all tasks mutex beeing locked
      mutexLock();
      directActivate(schedActivateData.prio);
      mutexUnlock();
   }
}

