/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
 Copyright (c) 2013-2014 Holger Koelle
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

\brief class which provides mutual exclusion locking for dation operations

*/
#include <stdio.h>
#include "UserDation.h"
#include "Signals.h"
#include "TaskCommon.h"

namespace pearlrt {

   UserDation::UserDation() {
      currentTask = NULL;
      rstValue = NULL;
      mutex.name("UserDation");
   }

   void UserDation::internalDationClose(const int p) {

      assertOpen();

      if (p & CLOSEMASK) {
         if (!!(p & PRM) && !!(p & CAN)) {
            Log::error("UserDation: ether CAN or PRM allowed");
            throw theInternalDationSignal;
         }

         // superseed previous settings
         dationParams &= ~CLOSEMASK;
         dationParams |= p;
      }

      // mark the dation to be closed, even if errors during
      // closing the system dation occur
      dationStatus = CLOSED;
      closeSystemDation(dationParams);
   }

   void UserDation::beginSequence(TaskCommon * me,
                                  Dation::DationParams direction) {
      // this method is called before any dation operation starts
      // in the application

      // verify that the dation is really open
      assertOpen();


      mutex.lock();

      currentTask = me;
      rstValue = NULL;
      rstVoidPointer = NULL;

      if (me) {
         // for testing purpose it it possible to use
         // without a task object
         me->enterIO(this);
      }

      currentDirection = direction;

      // if multiple IO-requests are allowed, the beginSequenceHook
      // must deal with the task blocking
      beginSequenceHook(me);
   }

   void UserDation::endSequence() {

      endSequenceHook();

      if (currentTask) {
         // for testing purpose it it possible to use
         // without a task object
         currentTask->leaveIO();
      }

   }

   void UserDation::suspend() {
      mutex.unlock();
   }

   void UserDation::cont() {
      mutex.lock();
   }

   void UserDation::assertOpen() {
      if (dationStatus == CLOSED) {
         Log::error("dation open required");
         throw theDationNotOpenSignal;
      }
   }

}
