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

#ifndef TASKWHENLINKS_INCLUDED
#define TASKWHENLINKS_INCLUDED


/**
\file

\brief supply linked lists for task scheduling on interrupts

*/

namespace pearlrt {
   /**
   \brief supply linked lists for task scheduling on interrupts

   PEARL allows several tasking methods to be scheduled on one
   interrupt. When the interrupt occurs, all scheduled tasking
   operation must be executed.
   To realize this behavior, a linked list of tasks is established.
   The links are strore in the task control block, which is derived
   from this class.

   This class provides attributes and methods to create two linked
   lists for tasks being scheduled with WHEN for activation and
   continuation.
   */
   class TaskWhenLinks {
   protected:
      /** link to the next task waiting to be activated on the same interrupt.
      This value is 0, if this task is the last task in the list waiting
      for the interrupt
      */
      TaskWhenLinks* nextActivate;
      /** link to the next task waiting to be continued on the same interrupt.
      This value is 0, if this task is the last task in the list waiting
      for the interrupt
      */
      TaskWhenLinks* nextContinue;
   public:
      /**
      deliver the next task object, which wait to be continued

      \returns the next task waiting on the same interrupt; or 0, if
               no other task ist waiting
      */
      TaskWhenLinks* getNextContinue();

      /**
      set the next task object, which wait to be continued

      \param next pointer to next task in list
      */
      void setNextContinue(TaskWhenLinks * next);

      /**
      deliver the next task object, which wait to become activated

      \returns the next task waiting on the same interrupt; or 0, if
               no other task ist waiting
      */
      TaskWhenLinks* getNextActivate();

      /**
      set the next task object, which wait to be continued

      \param next pointer to next task in list
      */
      void setNextActivate(TaskWhenLinks * next);

      /**
      method, which is called for the task, when it should be
      continued. The continuation parameters must be stored in
      the derived tasks control block.

      \note This method must be called with locked mutexTasks
      */
      virtual void triggeredContinue() = 0;

      /**
      method, which is called for the task, when it should be
      activated. The activation parameters must be stored in
      the derived tasks control block.

      \note This method must be called with locked mutexTasks
      */
      virtual void triggeredActivate() = 0;
   };

}

#endif
