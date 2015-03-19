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

#ifndef TASKMONITOR_INCLUDED
#define TASKMONITOR_INCLUDED


/**
\file

\brief monitor of non dead tasks

stops the system at the end of the last running task
*/

#include <Mutex.h>

namespace pearlrt {

   /**
      Monitor all tasks

      If no task may run in future, since all are dead and or
      no schedule is remaining, the main thread will get a signal
      to terminate the system.
   */
   class TaskMonitor {
   private:
      volatile int nbrPendingTasks;
      pid_t pid;
      Mutex mutex;
      TaskMonitor();					// hidden!!
      TaskMonitor(TaskMonitor const&);			// hidden!!
      TaskMonitor& operator= (TaskMonitor const&);	// hidden!!

   public:
      /**
       get the object

       Singleton implementation (Meyers version)

      \returns the reference to the (only one) object of this class
      */
      static TaskMonitor& Instance();

      /**
         increment the counter of pending tasks

         Pending tasks are ether running or scheduled.
      */
      void incPendingTasks();

      /**
         decrement the counter of pending tasks

         Pending tasks are ether running or scheduled.
      */
      void decPendingTasks();

      /**
         get the number of pending tasks

         Pending tasks are ether running or scheduled.
         \returns the number of pending tasks
      */
      int getPendingTasks();
   };

}

#endif
