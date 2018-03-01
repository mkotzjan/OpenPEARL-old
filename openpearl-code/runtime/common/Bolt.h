/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

#ifndef PRLBOLT_INCLUDED
#define PRLBOLT_INCLUDED

/**
\file

\brief BOLT implementation for posix threads

*/
namespace pearlrt {
   class Bolt;
}

#include <stdint.h>
#include "Mutex.h"
#include "Task.h"   // intermediate BlockData
#include "TaskCommon.h"
#include "PriorityQueue.h"

namespace pearlrt {
   /**
   Implementation of the PEARL Bolt using a native counting semaphores

   The API-methods are realized as class methods, since enter, leave,
   reserve and free
   are defined to work on multiple bolt variables.
   The class methods are atomistic due to the all tasks mutex variable, which
   enshures the atomistic operation of all tasking related methods.
   In case a request-call should block the calling task, the block()-method
   of the calling task will be called. This must block (e.q. with
   a request to a task specific posix semaphore).
   In the tasks context, the blocking condition is stored.

   In case of a release, all blocked tasks are checked, whether they can
   be unblocked.

   Terminating blocked tasks will remove them from the wait queue.
   Suspending a blocked task will remove the task temporarily from
   the wait queue. For details see task.h

   \note Inspite of the fact that posix threads provide priority ceiling
   when using standard semaphores, this feature is currently not supported
   for the PEARL semaphores.
   */
   class Bolt {
   private:
      Bolt();                // hide default constructor
      uint state;            // the internal bolt variable state
      uint32_t nbrOfEnterOperations;
      const char * name;
      static PriorityQueue waiters;
      static int check(BlockReason r, BlockData::BlockReasons::BlockBolt *bd);

      enum BoltStates {FREE = 0, ENTERED = 1, RESERVED = 2};

      /**
         set the  state

      \param newState one of FREE, ENTERED, RESERVED

      */
      void setState(int newState);

      /**
         get the state of the bolt
         \returns the current status (free, entered, reserved)

      */
      int getState(void);



      /**
        perform ENTER or RESERVE

      Perform a enter or reserve  of all given bolt if possible.
      Block the task in the other case.

      \param me a pointer to the calling task
      \param operation the indicator for ENTER or RESERVE
      \param newState the new state if successful operation
      \param nbrOfBolts the number of requested bolts
      \param bolts a list of pointer to the bolt objects
      */
      static void enterOrReserve(TaskCommon* me,
                                 BlockReason operation,
                                 int newState,  int nbrOfBolts, Bolt** bolts);

      /**
      Perform a LEAVE or FREE  of all given bolt.

      Blocked tasks will become unblocked if possible. This is done
      with respect to the tasks priority.

      \param me a pointer to the calling task
      \param oldState expected state of the bolt objects
      \param nbrOfBolts the number of leaved bolts
      \param bolts a list of pointer to the bolt objects

      \throws BoltStateSignal if a bolt variable has a wrong state
      */
      static void leaveOrFree(TaskCommon* me,  int oldState,
                              int nbrOfBolts, Bolt** bolts);

      void incrementEnter();
      void decrementEnter();
      uint32_t getNbrOfEnterOperations();

   public:

      /**
      create the Bolt object with the preset free
      \param n a string containing the name of  the bolt
      */
      Bolt(const char * n = NULL);

      /**
         get the name of the bolt

        \returns pointer to a c-string containing the name of the semaphore

      \note for internal usage only
      */
      const char* getName(void);

      /**
      ENTER allows multiple (reading) tasks on the resource.

      Perform a enter  of all given bolt if possible.
      Block the task in the other case.

      \param me a pointer to the calling task
      \param nbrOfBolts the number of requested bolts
      \param bolts a list of pointer to the bolt objects
      */
      static void enter(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);

      /**
      RESERVE allows only one task to use the ressource.

      Perform a reserve  of all given bolt if possible.
      Block the task if if the bolt is ether entered or reserved

      \param me a pointer to the calling task
      \param nbrOfBolts the number of requested bolts
      \param bolts a list of pointer to the bolt objects
      */
      static void reserve(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);


      /**
      LEAVE is the inserse operation to ENTER
      Perform a LEAVE  of all given bolt.

      Blocked tasks will become unblocked if possible. This is done
      with respect to the tasks priority.

      \param me a pointer to the calling task
      \param nbrOfBolts the number of leaved bolts
      \param bolts a list of pointer to the bolt objects
      */
      static void leave(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);


      /**
      FREE is the inserse operation to RESERVE.

      Perform a FREE  of all given bolt.

      Blocked tasks will become unblocked if possible. This is done
      with respect to the tasks priority.

      \param me a pointer to the calling task
      \param nbrOfBolts the number of freed bolts
      \param bolts a list of pointer to the bolt objects
      */
      static void free(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);

      /**
      remove a task from the wait queue.

      This method must be called, when a blocked task becomes suspended
      As long as it is not continued - it should not aquire for unblocking

      \param t pointer to the task to be removed
      */
      static void removeFromWaitQueue(TaskCommon * t);

      /**
      add to wait queue.

      This method must be called, when a blocked ans suspended task becomes
      continued. So it must try to get the unblocking condition again.
      In case the given task may continue due to a fulfilled blocking
      condition it will not be added to the queue. The semaphore request
      operation will be performed.

      \param t pointer to the task to be added
      */
      static void addToWaitQueue(TaskCommon * t);

      /**
      update the wait queue.

      This method must be called, when a blocked task get a new priority
      assigned.

      \param t pointer to the task to be rearranged
      */
      static void updateWaitQueue(TaskCommon * t);

      /**
         get the state of the bolt as C-string
         \returns the current status (free, entered, reserved)

      */
      char* getStateName(void);
   };
}

/**
Macro to import or forward declare a semaphore

\param  x the name of the semaphore
\param fromModule module, where the bolt is defined
\param inModule module, this module's name

*/
#define SPCBOLT(x, fromModule, inModule) 	\
}						\
namespace pearl_ ## fromModule {		\
   extern pearlrt::Bolt x;			\
}						\
namespace pearl_ ##  inModule {

/**
Macro to define a BOLT

\param x the name of the bolt
*/
#define DCLBOLT(x) \
static pearlrt::Bolt x(#x)

/**
Macro to define a global BOLT

\param x the name of the bolt
*/
#define DCLBOLTGLOBAL(x) \
   pearlrt::Bolt x(#x)

#endif

