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

\brief list of all tasks

Provide access to all defined tasks.
Since PEARL has no dynamic task creation there is no need
to remove tasks from the list


\author R. Mueller
*/
#include <vector>
#include <algorithm>

#include "TaskList.h"
#include "Task.h"
#include "Log.h"
#include "Signals.h"
#include "compare.h"

namespace pearlrt {

   TaskList::TaskList() {
      nbrOfMainTasks = 0;
   }

   TaskList& TaskList::Instance() {
      // static objects are initialized at the first call
      static TaskList tl;
      return tl;
   }

   void TaskList::add(Task * t) {
      taskList.push_back(t);

      if (t->getIsMain()) {
         nbrOfMainTasks ++;
      }
   }

   static bool comparePrio(Task * i, Task * j) {
      bool diff = (i->getPrio() < j->getPrio()).getBoolean();

      if (diff == 0) {
         diff = i->getName() < j-> getName();
      }

      return diff;
   }

   void TaskList::sort() {
      std::sort(taskList.begin(), taskList.end(), comparePrio);
   }

   int TaskList::size() {
      int ret;
      ret = (int)taskList.size();
      return ret;
   }

   Task * TaskList::getTaskByIndex(unsigned int i) {
      Task * t;

      if (i < 0 || i > taskList.size()) {
         Log::error((char*)"getTaskByIndex: index out of bounds");
         throw theInternalTaskSignal;
      }

      t = taskList[i];
      return t;
   }

   int TaskList::getNbrOfMainTasks() {
      return nbrOfMainTasks;
   }

}
