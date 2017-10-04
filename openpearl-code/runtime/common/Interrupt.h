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

#ifndef INTERRUPT_INCLUDED
#define INTERRUPT_INCLUDED

#include "TaskWhenLinks.h"

namespace pearlrt {
   /**
   \addtogroup interrupt
   @{
   */

   /**
   \brief interrupt type

   Interrupts may be used to trigger task activation and continuation.
   The scheduled activation remains active after a trigger - the
   scheduled continuatin is removed automatically after triggering.

   In order to provide a list of registered tasks to a specific interrupt,
   the task control block is extended by 2 pointer. They are used for
   the 2 lists (continueTaskQueue and activateTaskQueue)

   The task objects have to provide methods to be called upon trigger the
   continuation (triggeredContinue()) and activation (triggeredActivate()).

   The sequence of triggering is in order of the list, which has a LIFO
   behavior. The execution according the tasks priority will be treated
   by the operating system. The complete trigger sequence is done while
   the tasks mutex is locked. Rescheduling may happen first after the
   completion of a trigger()-operation.

   Individual interrupt sources must be derived from Interrupt.
   Usually the ctor must register the object to the real interrupt
   source. The execution is usually triggered by a signal handler (UNIX)
   or a thread waiting for data (all systems) or interrupt service routine
   (microcontroller).

   */

   /**
   This class provides some methods for the type interrupt

   \todo realize a real implementation - currently only the default constructor
   is available
   */
   class Interrupt {
   private:
      TaskWhenLinks * headContinueTaskQueue;
      TaskWhenLinks * headActivateTaskQueue;
      bool         isEnabled;

   public:
      /**
         default constructor to be used in tasking headers as placeholder
      */
      Interrupt();

      /**
       enable function represents the ENABLE keyword in the PEARL language
      */
      void enable();

      /**
       disable function represents the DISABLE keyword in the PEARL language
      */
      void disable();

      /**
       trigger function represents the TRIGGER keyword in the PEARL language
      */
      void trigger();

      /**
      register a task for triggered activation

      Theses tasks remain in the link list of registered task for the
      interrupt when the interrupt is triggered.

      \param t pointer to the task object with should be activated when
               the interrupt is triggered
      \param next pointer to a pointer in the task control block to
             link with the next task awaiting the same interrupt
      */
      void registerActivate(TaskWhenLinks* t, TaskWhenLinks **next);

      /**
      register a task for triggered continuation

      Theses tasks re automatically removed from the list
      when the interrupt is triggered.

      \param t pointer to the task object with should be activated when
               the interrupt is triggered
      \param next pointer to a pointer in the task control block to
             link with the next task awaiting the same interrupt
      */
      void registerContinue(TaskWhenLinks* t, TaskWhenLinks **next);

      /**
      unregister a task for triggered activation

      This method is needed for the PREVENT operation

      \param t pointer to the task object with should be activated when
               the interrupt is triggered
      */
      void unregisterActivate(TaskWhenLinks* t);


      /**
      unregister a task for triggered continuation

      This method is needed for the PREVENT operation

      \param t pointer to the task object with should be continuated when
               the interrupt is triggered
      */
      void unregisterContinue(TaskWhenLinks* t);

      protected:
      /**
      enable interrupt in interrupt emitting device

      the device shall only trigger the treatment, if the interrupt
      is enabled by the application. This method is called from the 
      enable()-method.
      */
      virtual void devEnable()=0;

      /**
      disable interrupt in interrupt emitting device

      the device shall only trigger the treatment, if the interrupt
      is enabled by the application. This method is called from the 
      disable()-method.
      */
      virtual void devDisable()=0;

   };
   /** @} */
}

#endif

