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

#ifndef PRLSEMA_INCLUDED
#define PRLSEMA_INCLUDED

/**
\file

\brief semaphore implementation for posix threads

*/
namespace pearlrt {
   class Semaphore;
}

#include <stdint.h>
#include "Mutex.h"
#include "BitString.h"
#include "Task.h"   // intermediate BlockData
#include "TaskCommon.h"
#include "PriorityQueue.h"

namespace pearlrt {
   /**
   Implementation of the PEARL Semaphore using nativ counting semaphores

   The API-methods are realized as class methods, since requst and
   release are defined to work on multiple semaphores.
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

   The counter for the semaphore is implemented as unsigned int with 32 bits.
   In case of a numerical overflow, the SemaOverflowSignal will be raised.

   \note Inspite of the fact that posic threads provide priority ceiling
   when using standard semaphores, this feature is currently not supported
   for the PEARL semaphores.
   */
   class Semaphore {
   private:
      Semaphore();                // hide default constructor
      uint32_t value;           // the internal semaphore value
      const char * name;
      static PriorityQueue waiters;
      static int check(BlockData::BlockReasons::BlockSema *bd);

   public:
      /**
      create the semaphore object with the given preset
      defaulting to 0
      \param preset the preset value (defaults to 0)
      \param n a string containing the name of  the semaphore
      */
      Semaphore(uint32_t  preset = 0, const char * n = NULL);

      /**
         get the name of the semaphore

        \returns pointer to a c-string containing the name of the semaphore

      \note for internal usage only
      */
      const char* getName(void);

      /**
         decrement the value of the semaphore
      \note for internal usage only
      */
      void decrement(void);

      /**
         get the value of the semaphore

      \note for internal usage only
      */
      void increment(void);

      /**
         get the value of the semaphore
         \returns the current value of the counting semaphore

      \note for internal usage only
      */
      uint32_t getValue(void);

   public:

      /**
      perform a request (decrement) of all given semaphores if possible.
      Block the task in the other case.

      \param me a pointer to the calling task
      \param nbrOfSemas the number of requested semaphores
      \param semas a list of pointer to the semphore objects
      */
      static void request(TaskCommon* me,  int nbrOfSemas, Semaphore** semas);

      /**
      perform the TRY operation

      If the semaphores were free, they will be decremented.

      \param me a pointer to the calling task
      \param nbrOfSemas the number of requested semaphores
      \param semas a list of pointer to the semphore objects

      \returns 1, if they were free<br>
              0, else
      */
      static BitString<1> dotry(TaskCommon* me,  int nbrOfSemas, Semaphore** semas);

      /**
      perform a release (increment) of all given semaphores.
      Blocked tasks will become unblocked if possible. This is done
      with respect to the tasks priority.

      \param me a pointer to the calling task
      \param nbrOfSemas the number of requested semaphores
      \param semas a list of pointer to the semphore objects

      \throws SemaOverflowSignal
      */
      static void release(TaskCommon* me,  int nbrOfSemas, Semaphore** semas);

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
   };
}

/**
Macro to import a semaphore

\param  x the name of the semaphore
\param fromModule module, where the semaphore is defined
\param inModule module, this module's name
*/
#define SPCSEMA(x,fromModule,inModule) 	\
}   						\
namespace pearl_ ## fromNameSpace {		\
   extern pearlrt::Semaphore x;			\
}						\
namespace pearl_ ## inNameSpace {

/**
Macro to define a semaphore

\param x the name of the semaphore
\param preset the preset value
*/
#define DCLSEMA(x,preset) \
static  pearlrt::Semaphore x(preset,#x)

/**
Macro to define a global semaphore

\param x the name of the semaphore
\param preset the preset value
*/
#define DCLSEMAGLOBAL(x,preset) \
   pearlrt::Semaphore x(preset,#x)

#endif

