/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
               2014-2015 Rainer Mueller
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
 \file

 \brief Base class for FreeRTOS based tasks

 */


#ifndef TASK_H_
#define TASK_H_


#include "BitString.h"
#include "Prio.h"
#include "Clock.h"
#include "Duration.h"
#include "Interrupt.h"
#include "TaskCommon.h"
#include "TaskTimer.h"

#include "Signals.h"
#include "FakeTypes.h"

#define STACK_SIZE 4096


namespace pearlrt {
   /**

   \brief Base class for FreeRTOS based tasks

   */
   class Task : public TaskCommon {


   public:
      TaskTimer activateTimer;	///< Timer object
      TaskTimer continueTimer;	///< Timer object
      FakeTaskHandle_t xth;		///< FreeRTOS task handle
      FakePortSHORT stackDepth;	///< Stack depth


   private:
      /**
      hide default ctor
      */
      Task();

      /* the FreeRTOS task control block */
      FakeTCB_t tcb;

      /* the FreeRTOS stack */
      FakeStackType_t stack[STACK_SIZE];

   public:
      /**
      ctor for task object

      \param n the name of the task
      \param prio the PEARL priority of the task
      \param ismain '1'B1, if MAIN attribute is set at the tasks declaration
                      <br>'0'B1 else
      */
      Task(char* n, Prio prio, BitString<1> ismain);

      /**
      do initializations, which must be done in main()

      This method creates the timer objects. This cannot be done at
      time of static initialization.
      */
      void init();

      /**
      start the task with given priority

      \param prio the new current priority of the task
      */
      void directActivate(const Fixed<15>& prio);

      /**
       perform required operations to adjust priority, semaphore
       wait queues, .... when the task got the continue condition

       \param condition indicates if a new priority should be set
       \param prio the new priority for the task

       \note this method expects the tasks mutex to be locked.
             It releases the tasks mutex only in case of throwing an
             exception.
      */
      void continueFromOtherTask(int condition,
                                 Prio prio);
   private:
      void continueSuspended();
      static void restartTaskStatic(Task*t);
      void restartTask();
   public:

      /**
      goto suspend mode
      */
      void resume2();


      /**
      terminate the task by itself
      */
      void terminateMySelf();

      /**
      terminate the task by another task
      */
      void terminateFromOtherTask();

      /**
      suspend the task by itself
      */
      void suspendMySelf();

      /**
      suspend the task by another task
      */
      void suspendFromOtherTask();

      /**
      the tasks body.

      The content of the tasks body is created by the DCLTASK macro
      and the subsequent statements after the DCLTASK macro

      \param me pointer to the tasks object
      */
      virtual void task(Task * me) = 0;

      /**
      the FreeRTOS thread code must be a void function with a void* as
      parameter.
      \param param pointer to the tasks object data
      */
      static void tskfunc(void* param);

      /**
      task entry initilization

      this method is called in startup of the application task.

      */
      void entry();


      /**
        change the tasks priority to the new PEARL prio
        the method returns after setting the new priority

        \param prio the new PEARL priority of the task
      */
      void changeThreadPrio(const Fixed<15>& prio);

      /**
       change the prioriy of the calling thread to maximum
       priority to avoid spurious context switches

       \returns the previous priority
      */
      int switchToThreadPrioMax() ;

      /**
       change the prioriy of the calling thread to (previous)
       priority, which is given as parameter

       \param cp the previous priority
      */
      void switchToThreadPrioCurrent(int cp) ;

      /**
      return the FreeRTOS handle for diagnosic purpuses 
      \return the FreeRTOS handle of the current task, if the task is not
              terminated
      \return NULL else
      */
      FakeTaskHandle_t getFreeRTOSTaskHandle();
   };

}

#endif /* TASK_H_ */
