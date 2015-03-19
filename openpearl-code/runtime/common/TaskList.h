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

#ifndef TASKLIST_INCLUDED
#define TASKLIST_INCLUDED


/**
\file

\brief list of all tasks
*/

#include <vector>
#include "Task.h"

namespace pearlrt {

   /**
      \brief Provide access to all defined tasks at run time

      This class prvides a list of all tasks in the system.

      To get ride of the "static initializer fiasco" this class
      is realized as Meyers singleton pattern. All static task object
      register at (static) instantiation at this object. The sequence
      of static initialisation with more than one compilation unit
      is not defined in C++ standard.

      Since PEARL has no dynamic creation of tasks there is no need to remove
      tasks from the list.
      The list is used in the comand line tool and by the automatic shutdown
      feature.

      Since PEARL provides only static tasks - there may not be a change
      in the tasklist during run time.
      There is not need for mutual exclusion protection.
   */
   class TaskList {
   private:
      TaskList();                               // hidden
      TaskList(TaskList const &);               // hidden
      TaskList& operator=(TaskList const &);  // hidden

      /**
      all defined tasks are stored in a vector. This is done befor activation
      of the first task. During runtine this vector is unchanged. Therefor
      runtime problems with memory fragmentation will not happen.
      In case of the existene of a module linker, this should be replaced
      by a static array.
      */
      std::vector <Task*> taskList;

      /**
      Number of tasks with the attribute 'MAIN'
      */
      int nbrOfMainTasks;
   public:
      /**
      get access to the tasklist (meyers singleton implementation)

      \returns the (only) tasklist object
      */
      static TaskList& Instance();

      /**
         add a task to the list

         \param t pointer to the task
      */
      void add(Task* t);

      /**
         sort list according the tasks default priority
      */
      void sort();

      /**
         return the number of defined tasks

         \returns number of defined tasks
      */
      int size();

      /**
         find a task object by the thread process id of the
         running thread (task)

         \returns the pointer to the task object
      */
//      virtual Task* getTaskSelf() = 0;

      /**
          find a task by the index in the TaskList

          \param i the index of the Task in the list

          \returns the pointer to the task object
      */

      Task* getTaskByIndex(unsigned int i);

      /**
          get number of main tasks
          \returns number of registered task with attribute MAIN
      */
      int getNbrOfMainTasks();
   };
}

#endif
