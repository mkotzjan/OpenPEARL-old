/*
 [The "BSD license"]
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


#include "Task.h"
#include "TaskMonitor.h"
#include <string.h>
#include "TaskList.h"
#include "PrioMapper.h"
#include "Semaphore.h"
#include "Log.h"
#include "FreeRTOS.h"

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
      stackDepth = 400;
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
      int freeRtosPrio = PrioMapper::getInstance()->fromPearl(prio.x);

      Log::debug("%s::directActivate freeRTOSprio=%d", name, freeRtosPrio);

      BaseType_t taskCreation = pdFALSE;

//      if ((schedActivateData.taskTimer->isActive() == false) &&
//          (schedActivateData.whenRegistered == false) &&
//           schedActivateOverrun == false) {
//         TaskMonitor::Instance().incPendingTasks();
//      }


      // the scheduling may only be stopped, if the scheduler
      // was started - the creation of MAIN-tasks need no
      // stop/start of the scheduler, since it is started later
      freeRtosRunning = xTaskGetSchedulerState();

      if (freeRtosRunning) {
         disableScheduling();
      }

      //this-> FreeRTOSPriority = prio;
      taskCreation = xTaskCreate(&tskfunc,
                                 (const char*) this->name,
                                 this->stackDepth, this,
                                 freeRtosPrio, &xth);

      if (taskCreation) {
         Log::debug("%s: started", name);
         taskState = RUNNING;
      } else {
         Log::error("%s: not created", name);
         throw theInternalTaskSignal;
      }

      Log::debug("%s::activated ", name);

      if (freeRtosRunning) {
         enableScheduling();
      }

   }


   void Task::resume2() {
      suspendMySelf();
   }


   void Task::tskfunc(void* param) {
      Task* me = ((Task*)param);
      Log::debug("%s: starts", me->getName());

      try {
         me->task(me);
      } catch (Signal & p) {
         char line[256];
         printf("++++++++++++++++++++++++++++++\n");
         sprintf(line, "%s:%d Task: %s   terminated due to: %s",
                 me->getLocationFile(), me->getLocationLine(),
                 me->getName(), p.which());
         printf("%s\n", line);
         pearlrt::Log::error(line);
         printf("++++++++++++++++++++++++++++++\n");
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
      taskState = TERMINATED;

      if (schedActivateOverrun) {
         Log::error("%s: terminates with schedOverrun flag - start task again",
                    name);
         schedActivateOverrun = false;
         directActivate(schedActivateData.prio);
         mutexUnlock();
      } else {
         if (schedActivateData.taskTimer->isActive() == false &&
               schedActivateData.whenRegistered == false) {
            mutexUnlock();
            TaskMonitor::Instance().decPendingTasks();
         } else {
            mutexUnlock();
         }
      }
{
int f = uxTaskGetStackHighWaterMark(NULL);
printf("Task %s stack useage: %d\n", name, f);
}

      vTaskDelete(oldTaskHandle);
   }

   void Task::terminateFromOtherTask() {
      Log::debug("%s: terminateFromOtherTask", name);
      int cp; // current calling tasks priority

      // set the calling threads priority to maximum priority
      // to enshure the excution of this function without task switch
      cp = switchToThreadPrioMax();

      switch (taskState) {
      case IO_BLOCKED:
         // unlocking of the dations lock is not guaranteed
         Log::warn("terminate remote in i/o blocked not supported");
         break;

      case SEMA_BLOCKED:
         Semaphore::removeFromWaitQueue(this);
         taskState = Task::RUNNING;
         blockParams.semaBlock.release();
         break;

      case SUSPENDED:
         // no problem - just kill
         break;

      case RUNNING:
         break;

      case SEMA_SUSPENDED_BLOCKED:
         Log::debug("   task %s: terminateRemote susp_blocked task", name);
         taskState = Task::RUNNING;
         blockParams.semaBlock.release();  // unblock
         break;

      default:
         Log::error("   task %s: unhandled taskState (%d) at TERMINATE",
                    name, taskState);
         mutexUnlock();
         switchToThreadPrioCurrent(cp);
         throw theInternalTaskSignal;
      }


      taskState = TERMINATED;
      vTaskDelete(this->xth);

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
//    Log::debug("%s: change thread prio from %d to %d",name, currentPrio.x,p);
   }

   void Task::disableScheduling() {
//    Log::debug("vTaskSuspendAll();");
      vTaskSuspendAll();
   }

   void Task::enableScheduling() {
      xTaskResumeAll();
//      Log::debug("xTaskResumeAll();");
   }

   int Task::switchToThreadPrioMax() {
      int cp;
      cp = uxTaskPriorityGet(NULL);
//      Log::debug("%s: switch  task %s from %d to max prio",
//              pcTaskGetTaskName(NULL),
//              name,
//              cp);
      vTaskPrioritySet(NULL, 257);
      return cp;
   }

   void Task::switchToThreadPrioCurrent(int cp) {
      vTaskPrioritySet(NULL, cp);
//      Log::debug("%s: switch task %s back to prio %d",
//                pcTaskGetTaskName(NULL),
//                name,
//                cp);
   }
   TaskHandle_t Task::getFreeRTOSTaskHandle() {
      if (taskState != TERMINATED) return xth;
      return NULL;
   }

}

