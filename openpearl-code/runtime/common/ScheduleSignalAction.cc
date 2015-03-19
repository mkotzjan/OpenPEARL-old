/*
[The "BSD license"]
Copyright (c) 2014-2014 Rainer Mueller
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
\brief a helper class to determine whether a signal has
   currently a handler.
*/
#include "ScheduleSignalAction.h"
#include "Signals.h"

namespace pearlrt {

   ScheduleSignalAction::ScheduleSignalAction(Signal * s) {
      signal = s;
      currentHandlerIndex = 0;
   }
   int ScheduleSignalAction::getAction(
      Signal *s,
      size_t nbrOfActions,
      ScheduleSignalAction scheduleSignalAction[]) {
      int indexOfGroup = -1;
      int currentRST = s->whichRST();
      int scheduledRST;

      for (size_t i = 0; i < nbrOfActions; i++) {
         scheduledRST = scheduleSignalAction[i].signal->whichRST();

         if (scheduledRST == currentRST &&
               scheduleSignalAction[i].currentHandlerIndex != 0) {
            return scheduleSignalAction[i].currentHandlerIndex;
         }

         /* test for group scheduled group signal */
         if (scheduledRST == (currentRST / 100) * 100) {
            indexOfGroup = i;
         }
      }

      if (indexOfGroup >= 0) {
         return scheduleSignalAction[indexOfGroup].currentHandlerIndex;
      }

      return 0;
   }

   void ScheduleSignalAction::setActionIndex(int index) {
      currentHandlerIndex = index;
   }
}
