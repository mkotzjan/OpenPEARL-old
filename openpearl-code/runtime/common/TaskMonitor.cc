/*
 [A "BSD license"]
 Copyright (c) 2012-2016 Raier Mueller
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

#include "TaskMonitor.h"

namespace pearlrt {

   TaskMonitor& TaskMonitor::Instance() {
      // static objects are initialized at first call of the function
      // see eg: www.devarticles-com/c/a/Cplusplus/
      //        C-plus-plus-In-Theory-The-Singleton-Pattern-Part-I/4/
      static TaskMonitor tm;
      return tm;
   }

   /*
      TaskMonitor::TaskMonitor() {
         nbrPendingTasks = 0;

         // get current pid; necessary to send signal at the end
         pid = getpid();
         mutex.name("TaskMonitor");
      }
   */

   void TaskMonitor::incPendingTasks() {
      mutex.lock();
      nbrPendingTasks ++;
      mutex.unlock();
   }

   int TaskMonitor::getPendingTasks() {
      return nbrPendingTasks;
   }

}
