/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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

#ifndef TASKCOMMON_INCLUDED
#define TASKCOMMON_INCLUDED


/**
\file

\brief tasking functionality interface towards the compiler

*/


namespace pearlrt {
   class TaskCommon;
   class Semaphore;

   /** reasons for blocking a task
   */
   enum BlockReason {
      NOTBLOCKED,               ///< task is currently not blocked
      REQUEST                   ///< task is block due to request
   };

   /**
    data structure for blocking request

    This structure and the methods block/unblock are needed for the
    plattform independend Semaphore-class.
   */
   struct BlockData {
      BlockReason reason;	///< reason for blocking
      /**
       since a task may be blocked only by one reason - the parameters
       of the blocking request may be stored in a union
      */
      union BlockReasons {
         /**
         blocking due to REQUEST operation
         */
         struct BlockSema {
            int nsemas;	///< number of semaphores in the REQUEST call
            Semaphore **semas;	///< pointer to the array of semaphores
         } sema;	///< \returns the semaphore component of blocking
      } u;		///< \returns the union containing all blocking requests
   };

}

#include "Fixed.h"
#include "Clock.h"
#include "Duration.h"
#include "Interrupt.h"
#include "Prio.h"
#include "Mutex.h"
#include "CSema.h"
#include "UserDation.h"
#include "TaskWhenLinks.h"
#include "TaskTimerCommon.h"

namespace pearlrt {
   /**
   \brief interface to the task object of the indidivual implementation

   This class provides all necessary operations for the compiler to realize the
   tasking functionality required by the PEARL language.

   The common features are
   <ul>
   <li>a mutex to enshure atomic opration on all tasks control data
   <li>priority (default and current)
   <li>the tasks name
   <li>the main attribute
   <li>the tasks state
   <li>the tasks location (file+line)
   </ul>

   The implementation must provide C-macros named

     DCLTASK(tname, prio, ismain)

     SPCTASK(tname)

   The macro DCLTASK must be written in that way that the tasks body code
   may follow immediatelly (with the leading opening brace
   supplied by the compiler).
   The parameters are:
   tname: The name of the task
   prio: the priority of the task
   ismain: 1, if the task shall be started automatically;
           0, else


   \note The type REF TASK should be handled inside the compiler


      To realize blocking due to semaphore, bolt or i/o-operations
      the operations block() and unblock() are provided.

   The task state diagram shows the possibilities to change the state.
    Regard that the state running includes the internal state runnable.
   <ul>
   <li>State terminated<br>
   A task is waiting to become activated.</li>
   <li>State running<br>
   The task is ready for execution or executing, depending
     on the cpu situation. The scheduler performs the switching between these
     two states automatically.
   <li>State suspended<br>
   The task's execution is halted, but may be continued without
   loss of information. The task does not try to allocate cpu time. Allocated
   system ressources remain allocated.
   <li>States blockedSema<br>
   The task cannot continue operation, because it waits for a semaphore.
   <li>State blockedIO<br>
   The task cannot continue operation, because it waits for completion of
   an i/o-operation.
   <li>suspendedSema<br>
   The task was suspended while waiting for a semaphore.
   After continuation, the task waits again for the semaphore.
   <li>suspendedIO<br>
   The task was suspended while waiting for completion of an i/o-operation.
   The transition to suspendedIO is done at a suitable point of execution.
   E.q. after an end of record, or after an data element.
   After continuation the i/o-operation continues at the suspended position.
   </ul>

   \image html  taskStatesPEARL90.jpg
   \image latex taskStatesPEARL90.eps


   */
   class TaskCommon : public TaskWhenLinks {
   protected:
      /** pointer to dation object when doing I/O
      */
      UserDation * dation;

      /**
        tests for correct combination AT/ALL/..

        \param condition bit field with flags for the relevante parameters
        \param during duration of the validity of the schedule command
        \param all repetition rate of the schedule command

        \returns true, if the parameters are ok
        \returns false, else
       */
      void testScheduleCondition(int condition, Duration during, Duration all);
   private:
      static CSema mutexTasks;
   protected:
      /** scheduled structure for activate and continue

      this struct contains all data which are required for a timed
      activation and continueation
      */
      struct Schedule {
         bool whenRegistered; ///< set to true, if WHEN isr registered
         Interrupt * when;  ///< pointer to registered interrupt
         Fixed<15> prio;    ///< prio for new action
         TaskTimerCommon * taskTimer;  ///< the timer
      };

      /** control data for the scheduled activation */
      Schedule schedActivateData;

      /** control data for the scheduled activation.

      The scheduled continuation needs only a subset of the timer
      in comparison with the activation.  */
      Schedule  schedContinueData;

      /**
      flag to register an scheduled activation request while the
      task is still active.

      The task will be restarted immediatelly after its termination
      in case of this flag being set
      */
      bool schedActivateOverrun;

   public:
      /**
      possible states of a thread
      */
      enum TaskState {
         TERMINATED,            //< task is terminated
         RUNNING,               //< task is running (runnable)
         SUSPENDED,             //< task is suspended
         SEMA_SUSPENDED_BLOCKED, //< task is suspended and (prev.) sema op.
         SEMA_BLOCKED,          //< task is blocked due to sema request
         IO_SUSPENDED_BLOCKED, //< task is suspended and (prev.) dation op.
         IO_BLOCKED           //< task is blocked due to dation operation
      };

      /**
         task scheduling parameters may be combined in many combinations

         This enumeration provides the possibility by 'or'ing
         the different values as
         requested from the user program.
      */
      enum TaskScheduling {
         AT = 1, AFTER = 2, WHEN = 4, ALL = 8,
         DURING = 16, UNTIL = 32, PRIO = 64
      };

   protected:
      Fixed<15> defaultPrio;    ///< PEARL priority from TASK statement
      /** current PEARL priority, may be differnt from
          the default priorty
      */
      Fixed<15> currentPrio;     ///< current priority, may be differnt from
      ///< the default priorty
      char * name;     ///< PEARL task name
      int isMain;               ///< MAIN attribute of the PEARL task
      enum TaskState taskState;    ///<  state of the thread
      /* backtrace information for signal handler  */
      int sourceLine;		///< current source file line number
      const char * fileName;	///< current source file name

      /** data to perform task blocking/unblocking requests */
      struct BlockParams {
         CSema semaBlock;   ///< Semaphor for blocking operation
         /** reference to next task which waits on this semaphore*/
         TaskCommon * next;
         /** reason on blocking */
         BlockData why;
      } blockParams;   ///< control block to store the blocking request

      /**
      constructor of the class

      \param n the tasks name as C-string
      \param prio the PEARL priority of the task
      \param isMain the flag showing the presence of the MAIN attribute
             in the tasks declaration
      */
      TaskCommon(char * n, Prio prio, BitString<1> isMain);

   public:
      /**
      suspend a task

      If the task is  in RUNNING state, the task will no longer aquired any
      ressources from the system. Ressources, which are assigned to the task
      will remain assigned to the task.

      If the task is in BLOCKED state, the task will no longer try
      to receive the unblocking condition. After continuation, the task
      will try to get the unblocking condition. (see RTOS-UH p210 21-06-2006)

      If the task is  any other state, no operation is performed.

      \param me pointer to the current tasks object
      */
      void suspend(TaskCommon* me);

      /**
         continue a task

         If a condition is specified, a timer is armed. The timer
         triggers a signal handler, which performs the continue-actions
         as scheduled with the given parameters.

           \param me pointer to the current tasks object
           \param condition pattern of parameters for scheduled activation
                  (see  enum TaskScheduling)
           \param prio the new PEARL priority of the task, if given <br>
                  if not given, the current priority remains unchanged
           \param at the time the task should be continued.
               	   In case the current time is larger than the
                   given time, the action will occur the next day
           \param after the duration which must pass before the action occurs
           \param all the duration between two continuations
           \param until the time which defines the last continuation of
                  the task.  In case the current time
               	is larger than the given time it will be interpreted
          	as the next day
           \param during the time while the actions may occur
           \param when the (external) interrupt which starts the schedule

           \throws InternalTaskSignal if the required system ressources
                 are unavalibale
      */
      void cont(TaskCommon* me,
                int condition = 0,
                Prio prio = Prio(),
                Clock at = 0.0,
                Duration after = 0.0,
                Duration all = 0.0,
                Clock until = 0.0,
                Duration during = 0.0,
                Interrupt* when = 0);

      /**
          Activate a task

          perform all required internal operations before starting
          a new thread

          \param me pointer to the current tasks object
          \param condition pattern of parameters for scheduled activation
          \param prio the new PEARL priority of the task, if given
                (see  enum TaskScheduling)
          \param at the time the task should start.
                 In case the current time is larger than the
                 given time, the activation will occur the next day
          \param after the duration which must pass befor activation occurs
          \param all the duration between two activations
          \param until the time which defines the last activation of the task.
                 In case the current time
               	 is larger than the given time it will be interpreted
                 as the next day
          \param during the time while the activations may occur
          \param when pointer to the (external) interrupt which starts
                 the schedule

          \throws InternalTaskSignal if the required system ressources
                  are unavalibale
          \throws TaskRunningSignal if the tasks thread is already active
      */
      void activate(TaskCommon * me,
                    int condition = 0,
                    Prio prio = Prio(),
                    Clock at = 0.0, Duration after = 0.0,
                    Duration all = 0.0,
                    Clock until = 0.0,
                    Duration during = 0.0,
                    Interrupt * when = 0);


      /**
          scheduled activate for a task

          perform all required internal operations before starting
          a new thread

          \param prio the new PEARL priority of the task
          \param condition pattern of parameters for scheduled activation
                 (see  enum TaskScheduling)
          \param at the time the task should start.
                 In case the current time is larger than the
                 given time, the activation will occur the next day
          \param after the duration which must pass befor activation occurs
          \param all the duration between two activations
          \param until the time which defines the last activation of the task.
                In case the current time
              	is larger than the given time it will be interpreted
                as the next day
          \param during the time while the activations may occur
          \param when the (external) interrupt which start the schedule

          \throws InternalTaskSignal if the required system ressources
                are unavalibale
          \throws TaskRunningSignal if the tasks thread is already active
      */
      void scheduledActivate(int condition,
                             Fixed<15>& prio,
                             Clock& at, Duration& after,
                             Duration& all, Clock& until,
                             Duration& during,
                             Interrupt* when);


      /**
         Terminate a task

         In case of terminating a blocked task, the blocking condition
         is skipped, without modifications of e.g. the semaphore.

           \param me pointer to the current tasks object
      */
      void terminate(TaskCommon * me);

      /**
         Perform a scheduled RESUME

         The method checkes the timer situation and cancels any
         scheduled continues for this task.
         The resume operation itself is delegated to the virtual method
         resume2().

         \param condition a bit map with the resume condition
             (see enum TaskScheduling)
         \param at the time for the continuation
         \param after the requested delay befor continuation
         \param when the (external) Interrupt which should be awaited

      */
      void resume(int condition,
                  Clock at = 0.0,
                  Duration after = 0.0,
                  Interrupt* when = 0);

      /**
      do the resume operation

      All checks are done in the plattform independent method resume().
      This method is call with locked mutexTasks lock. This lock must NOT be
      released.
      */
      virtual void resume2() = 0;
      /**
         remove all scheduled action for the task

         <ul>
         <li>reset all internal scheduling parameters to the reset state
         <li>stop timer
         </ul>
           \param me pointer to the current tasks object

      */
      void prevent(TaskCommon * me);

      /**
          deliver the name of the task

          \returns name of the task as C-string
       */
      char * getName();

      /**
         check if the task should be started automatically at system start

         \returns 0, if no<br>1 if yes
      */
      int getIsMain();

      /**
          get the current task state

          \return the task state
       */
      TaskState getTaskState();

      /**
         retrieve the tasks priority

         delivers the current PEARL priority

         \returns the PEARL priority of the task
      */
      Fixed<15> getPrio();


   public:
      /**
      schedule callback

      This method may be superseeded from a specialization to set a method.
      It is called each time setLocation is called.

      In Linux-environment, it is not possible to terminate an asynchronously
      suspended thread.

      \param isLocked is passed as true if the tasksMutex is currently locked
      */
      virtual void scheduleCallback(bool isLocked = false);

      /**
      set source location

      set location of the current PEARL statement

      \param lineNumber source line number
      \param fileName pointer to a C-string containing the source code
      */
      void setLocation(int lineNumber, const char * fileName);

      /**
         get current source file

         \returns C-string with the last set location file
      */
      const char* getLocationFile();

      /**
          get current source line

          \returns C-string with the last set location file
       */
      int getLocationLine();

      /**
       block a task due to sempahore, bolt or other operation

       \param why the blocking reason
      */
      void block(BlockData * why);

      /**
       unblock a task due to sempahore, bolt or other operation

      */
      void unblock();

      /**
        obtain blocking request
        \param why gets a copy of the blocking reason
      */
      void getBlockingRequest(BlockData *why);

      /**
       get link to next blocked task (waiting for the same ressource)
      \returns pointer to TCB of next task in list
      */
      TaskCommon* getNext();

      /**
       set link to next blocked task (waiting for the same ressource)
       \param t pointer to TCB of next task in the wait queue
      */
      void setNext(TaskCommon*t);

      /**
      grant mutual exclusion access to the tasks data.
      This is done usually with a mutex variable.

      This method is needed for the Semaphore and Interrupt class
      */
      static void mutexLock();

      /**
      release mutual exclusion access to the tasks data

      This method is needed for the Semaphore and Interrupt class
      */
      static void mutexUnlock();

      /**
         enter i/o operation

         updates task state to IO_BLOCKED

         \param d pointer to the dation which performs the i/o
      */
      void enterIO(UserDation * d);

      /**
         leave i/o operation

         updates task state to RUNNING
      */
      void leaveIO();

      /**
        isMySelf

        check if the calling task is the same as the task of this object

        \param me the pointer to the calling tasks object

        \returns true, if it is the same task

      */
      bool isMySelf(TaskCommon * me);

      /**
         Handler to be called after a scheduled activation of a task
         is detected

         It updates the internal data and enshures to cancel further
         activation calls in
         case the final activation is reached

         \param tsk pointer to the task, which shoul be continued
      */
      static void activateHandler(TaskCommon * tsk);

      /**
        Perform an activation when the timer has expired.

        This method is called from the activateHandler.
        In case of the task is still active, the schedActivateOverrun-flag
        is set and the activation is done immediatelly after the
        tasks termination.
      */
      void timedActivate();

      /**
         Handler to be called after a scheduled continuation
         of a task is detected

         It updates the internal data and enshures to cancel further
         activation calls in case the final cycle is reached

         \param tsk pointer to the task, which shoul be continued
      */
      static void continueHandler(TaskCommon * tsk);

      /**
        Perform a continuation when the timer has expired.

        This method is called from the continueHandler.
        In case of the task is nether active nor suspended
        the method just emits an warning to the log
      */
      void timedContinue();

      /**
      method, which is called for the task, when it should be
      continued. The continuation parameters are stored in
      the tasks control block.
      */
      void triggeredContinue();

      /**
      method, which is called for the task, when it should be
      activated. The activation parameters are stored in
      the tasks control block.
      */
      void triggeredActivate();


      /* -----------------------------------------
      define virtual methods which must be suppiled by plattform
      specific task implementation
      ----------------------------------------- */

      /**
      immediate  activation of the task

      activate the task on the target system. All task state
      concerning error checks are done in superior layers.

      The method is called with locke mutexTasks lock.
      It MUST NOT release the lock on completion.

      \param prio the priority of the task
      */
      virtual void directActivate(const Fixed<15>& prio) = 0;

      /**
      terminate the own thread

      The method is called with locked matiexTasks lock and must
      release it at the end.
      */
      virtual void terminateMySelf() = 0;

      /**
      terminate the thread of this object as an action from another task

      The method must deal with all possible task state situations.
      Only error checked (already terminated) ist done in superior
      layers.

      The method is called with locked matiexTasks lock and must
      release it at the end.
      */
      virtual void terminateFromOtherTask() = 0;

      /**
      suspend the task

         this method must treat all relevant taskStates,
         which are RUNNING and IO_BLOCKED(??).

         Only the error checking for not allowed suspend
         calls according to the current task state is
         treated in superior levels

         The method is call with locked mutexTasks and
         must NOT release the mutexTasks lock.
         It may temporarily release the lock and gather it again.
      */
      virtual void suspendMySelf() = 0;

      /**
         suspend a task

         this method must treat all relevant taskStates.
         Only the error checking for not allowed suspend
         calls according to the current task state is
         treated in superior levels

         The method is call with locked mutexTasks and
         must NOT release the mutexTasks lock.
         It may temporarily release the lock and gather it again.
      */
      virtual void suspendFromOtherTask() = 0;

      /**
       perform required operations to adjust priority, semaphore
       wait queues, .... when the task got the continue condition

       \param condition indicates if a new priority should be set
       \param prio the new priority for the task

       \note this method expects the tasks mutex to be locked.
             It releases the tasks mutex only in case of throwing an
             exception.
      */
      virtual void continueFromOtherTask(int condition,
                                         Prio prio) = 0;

   };


}


#endif
