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
\file

\brief monitor of runningtasks

stops the system when no more activity may occur

\author R. Mueller
*/

#include <stdio.h>

#include "TaskMonitor.h"
#include "Log.h"

namespace pearlrt {

   TaskMonitor::TaskMonitor() {
      nbrPendingTasks = 0;
      mutex.name("TaskMonitor");
   }

   void TaskMonitor::decPendingTasks() {
      mutex.lock();
      nbrPendingTasks --;
      Log::debug("TaskMonitor: dec: %d task active/pending", nbrPendingTasks);
      mutex.unlock();

      if (nbrPendingTasks == 0) {
         // we dont kill the scheduler. FreeRTOS will present a assert-warning
         // if we would do. 
         //    vTaskEndScheduler();
         // Just print the end message to show the user that his application
         // has finished
         printf("last task exited -- end.\n");
         while(1);
      }
   }

}
