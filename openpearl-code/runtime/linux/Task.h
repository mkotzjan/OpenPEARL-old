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

#ifndef TASK_INCLUDED
#define TASK_INCLUDED

/**
\file

\brief tasking functionality with posix threads

*/


#include <stdio.h>
#include <pthread.h>

#include "BitString.h"
#include "PrioMapper.h"
#include "Prio.h"
#include "CSema.h"
#include "Clock.h"
#include "Duration.h"
#include "Interrupt.h"
#include "Log.h"
#include "Fixed.h"
#include "TaskCommon.h"
#include "TaskTimer.h"

namespace pearlrt {

   /**
   \brief wrapper object to map PEARL tasks on posix threads

   This class provides all necessary operations for the compiler to realize the
   tasking functionality required by the PEARL language.

   All tasks use the same mutex from TaskCommon to enshure thread safety.

   The user code of each task is capsulated in a function [task name]_body.
   This body code is called from a wrapper function named [task name]_entry.
   This wrapper function, which is realized in task.h by a C-define,
   enshures the existence of the exeption handler and the propper
   initialization for thread cancellation.

   Task suspending is solved by reading from a pipe (pipeResume),
   continuation is solved by writing a 'c' to the pipe.

   Asynchronous SUSPEND and TERMINATE are realized by setting some flags
   inside a signal handler (without MUTEX-protection) and setting the
   priority of the target task to maximum. The operation ist done in
   the scheduleCallback-method, which is clled at each call of
   TaskCommon::setLocation(). The result is the following task state
   diagram. Note that the operations suspend and terminate differ
   whether they are called from the task itself, or by another task.
   The calling from another task leads to a differed state and
   completion of the action at a suitable point of execution from the
   task itself.

   \image html  taskStatesLinux.jpg
   \image latex taskStatesLinux.pdf

   \note The type REF TASK should be handled inside the compiler

   */
   class Task : public TaskCommon {

   private:
      static int useNormalSchedulerFlag;
      static int schedPrioMax;
      Task();

   public:
      /**
      pointer to a task function defined for easier coding
      */
      typedef void (*TaskEntry)(Task *);

      void scheduleCallback(bool isLocked = false);
   private:

      void (*entryPoint)(Task * me); //< C function containing the code

      /** Semaphor for completion of suspend call (inits by default to 0)   */
      CSema suspendDone;
      int suspendWaiters;

      /** Semaphor for completion of continue call (inits by default to 0)   */
      CSema continueDone;
      int continueWaiters;

      /** Semaphor for completion of terminate call
           (inits by default to 0)   */
      CSema terminateDone;
      int terminateWaiters;

      /** Semaphor for completion message of activate
          (inits by default to 0)   */
      CSema activateDone;

      /** for Thread suspend resume
          the suspended thread waits for data on this pipe
          to continue a 'c' is written to the pipe
          to terminate a 't' is written to the pipe
      */
      int pipeResume[2];

      /* suspend and terminate from other tasks is is conflict
         with the pthread library.
         Thus these requests are set a flags to the thread.
         They are polled in setLocation() and treated from the
         target thread istself.
      */
      volatile bool asyncTerminateRequested;
      volatile bool asyncSuspendRequested;

      pthread_t threadPid; //thread id
      pthread_attr_t attr;  // pthread scheduling parameters
      struct sched_param param; // -"-

      TaskTimer activateTaskTimer;  // the concrete timer
      TaskTimer continueTaskTimer;  // the concrete timer

      /**
      set the threads priority to normal operation (current prio).

      This method is needed to resume normal operation after priority
      ceiling to OS-level to run tasking call completly.
      */
      void setNormalPriority();

      /**
      set the threads priority to maximum.

      This method is needed to complete a tasking method call without
       disturbance of other threads.
      */
      void setOSPriority();

   public:

      /**
         Perform a scheduled RESUME

         The plattform indepened part is done in TaskCommon
      */
      void resume2();

      /**
      set the suspend request bit

      The bit is polled in scheduleCallback and treated there
      */
      void setSuspendRequested();

   private:
      /**
      suspend the task
      */
      void suspendMySelf();

      /**
      suspend the task
      \param releaseJobDone indicates, where the semaphore should be released
      */
      void internalSuspendMySelf(bool releaseJobDone);


      /**
      terminate the own thread
      */
      void terminateMySelf();

      /**
      internal terminate the own thread
      \param releaseJobDone indicates, where the semaphore should be released
      */
      void internalTerminateMySelf(bool releaseJobDone);

      /**
      terminate the thread of this object as an action from another task
      */
      void terminateFromOtherTask();

   public:
      /**
      setup a new task

      <ol>
      <li> Initialize all attributes required by the tasking system.
      <li> setup the posix timers
      <li> setup thread attributes
      <li> register the task in the TaskList object
      </ol>

      \param entry a pointer to the task body function
      \param n a pointer to the task name
      \param prio the priority (in PEARL meaning)
      \param isMain 1, if the task should be started automatically
              at system start <br>0 else

      \throws InternalTaskSignal if system ressources are not available
      */
      Task(void (*entry)(Task*), char * n, Prio prio,
           BitString<1> isMain);

   private:
      /**
         suspend a task

         The pthread library does not support thread suspending.
         To realize the same behavior, the thread requests a read
         from a pipe.
         The continuation is performed by writing a 'c' to that pipe.

         If the calling thread is different from the tasks thread,
         a flag is set for the thread to suspend. This flag is polled
         at each setLocation()-call and each blocking methog call.


      */
      void suspendFromOtherTask();

      /**
        change the threads priority to the new PEARL prio

        the method returns after setting the new priority

        \param prio the new PEARL priority of the task
      */
      void changeThreadPrio(const Fixed<15>& prio);

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

      /**
        fullfill the suspend request of the current task

        The task is suspended by waiting for a 'c' on a pipe.
        The continuation is done by sending a 'c' to the pipe.

        \note the mutexTask is expected to be locked
        \note the taskState must be checked by the calling method

      */
      void continueSuspended();

   public:
      /**
          Perform the dynamic part of task initialising

          This method is called automatically in the DCLTASK macro
      */
      void entry();

   private:
      /**
      immediate  activation of the task

      \param prio the priority of the task

      \todo{add configuration parameter for thread stacksize

      The stack size of a thread limits the number of possible threads.
      By default each thread gets 8MB of stack.
      This leads to a limit of approximatelly 300 threads at one time.
      If this value is set to a smaller value, the thread limit increases.

      As interim solution the stacksize ist set to PTHREAD\_STACK\_MIN.
      For the final implementation this parameter should be set via
      a configuration parameter.

      */
      void directActivate(const Fixed<15>& prio);

   public:

      /**
         retrieve the tasks thread id

         The thread id changed at each activation. Linux
         guarantees no monotonic behavior.

         \returns the posix thread id (pid), if activated<br>
                  or 0 if terminated
      */
      pthread_t getPid();

      /**
         retrieve the tasks entry

         required internally for run time task identification

         \return the function pointer of the posix thread function
      */
      TaskEntry getEntry();

      /**
         switch to normal scheduling

         In case of starting the system without root priviledges,
         there is currently no possibility
         to use the realtime scheduler of linux.
         For many applications normal scheduling policy
         is sufficient. This is set by this method.
      */
      static void useNormalScheduler();

      /**
         store best priority in the system

         Several code segments need to run without dusturbance
         of other user threads. They are executed with "OS"-like
         priority, if SCHED_RR is avaliable. To reduce the number
         of system calls, the maximum priority is stored internally.

         \param p the best priority in the system - in system internal
                  representation
      */
      static void setSchedPrioMax(int p);

      /**
         set the threads priority to the best priority in the system

         Several code segments need to run without dusturbance
         of other user threads. They are executed with "OS"-like
         priority, if SCHED_RR is avaliable. To reduce the number
         of system calls, the maximum priority is stored internally.
      */
      void switchToSchedPrioMax();

      /**
         (re-)set the threads priority to the current priority

         Several code segments need to run without dusturbance
         of other user threads. They are executed with "OS"-like
         priority, if SCHED_RR is avaliable. At the end of these
         segments, the priority must be adjusted.
      */
      void switchToSchedPrioCurrent();

   private:
      /**
      set the threads priority

      \param p new threads priority - in system internal representation
      */
      void switchThreadSchedPrio(int p);

   public:
      /**
        deliver detailed information about this task

        \param lines array of buffers for the information ( type char [3][80] )
        \returns number of information lines
      */
      int detailedTaskState(char * lines[3]);
   };

}

/**
C++ code for the forward declaration of a task

\param t the name of the task without any additions
*/
#define   SPCTASK(t) \
extern pearlrt::Task t;

#if 0
#define   SPCTASKGLOBAL(t,ns) \
extern pearlrt::Task #ns::t;
#endif

/**
create to C++  code for the task definition

this macros also creates the task-body function with the
default exception handler

Target systems without standard output must redefine these macros
in a conventient way in there task-specification file

\param x the name of the task
\param prio the tasks priority (must be set to 255 if not given)
\param ismain 1, if the task is of type MAIN,<br> 0,else
*/
#define DCLTASK(x, prio, ismain) 			\
namespace pearlrt {					\
   static void x ## _entry (pearlrt::Task * me) ;		\
   static void x ## _body (pearlrt::Task * me) ;		\
}							\
pearlrt::Task x ( pearlrt::x ## _entry, (char*)#x, \
                       prio, ismain);	\
namespace pearlrt {					\
static void x ## _entry (pearlrt::Task * me) { 		\
      me->entry();  					\
      try {						\
         x ## _body (me);	 			\
      } catch (pearlrt::Signal & p) {			\
         char line[256];                                \
         printf("++++++++++++++++++++++++++++++\n");	\
         sprintf(line,"%s:%d Task: %s   terminated due to: %s",\
             me->getLocationFile(), me->getLocationLine(), \
             me->getName(), p.which());			\
         printf("%s\n",line);				\
         pearlrt::Log::error(line);			\
         printf("++++++++++++++++++++++++++++++\n");	\
      }							\
      me->terminate(me);  				\
   } 							\
}							\
							\
static void pearlrt::x ## _body (pearlrt::Task * me)

#if 0
#define DCLTASKGLOBAL(x, prio, ismain,ns) 		\
}							\
namespace pearlrt {					\
   static void x ## _entry (pearlrt::Task * me) ;	\
   static void x ## _body (pearlrt::Task * me) ;	\
}							\
namespace #x {						\
pearlrt::Task x ( pearlrt::x ## _entry, (char*)#x, 	\
                       prio, ismain);			\
}							\
namespace pearlrt {					\
static void x ## _entry (pearlrt::Task * me) { 		\
      me->entry();  					\
      try {						\
         x ## _body (me);	 			\
      } catch (pearlrt::Signal & p) {			\
         char line[256];                                \
         printf("++++++++++++++++++++++++++++++\n");	\
         sprintf(line,"%s:%d Task: %s   terminated due to: %s",\
             me->getLocationFile(), me->getLocationLine(), \
             me->getName(), p.which());			\
         printf("%s\n",line);				\
         pearlrt::Log::error(line);			\
         printf("++++++++++++++++++++++++++++++\n");	\
      }							\
      me->terminate(me);  				\
   } 							\
}							\
							\
namespace #x {						\
static void pearlrt::x ## _body (pearlrt::Task * me)
#endif

#endif


