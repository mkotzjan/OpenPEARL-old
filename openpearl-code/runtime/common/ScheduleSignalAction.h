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

#ifndef SCHEULEDSIGNALACTION_INCLUDES
#define SCHEULEDSIGNALACTION_INCLUDES

#include <stdlib.h>
#include "Signals.h"

/**
\file
\brief a helper class to determine whether a signal has
   currently a handler.
*/

namespace pearlrt {

   /**
   the signal handling requires a lookup of currently handled
   signals.
   */
   class ScheduleSignalAction {
   private:
      Signal* signal;
      int currentHandlerIndex;

   public:
      /** ctor
      \param s the signal which has a handler
      */
      ScheduleSignalAction(Signal * s);

      /**
      lookup the index of the action if the given signal is contained
      in the list of ScheduleSignalActions

      \param s pointer to the current signal
      \param nbrOfActions number of actions in ScheduleSigbalAction[]
      \param scheduleSignalAction is an array of scheduled actions on signals

      \returns current index of the action of the given signal<br>
               0, if the signal is not contained in the list
      */
      static int getAction(Signal *s,
                           size_t nbrOfActions,
                           ScheduleSignalAction scheduleSignalAction[]);

      /**
      set the index of the sscheduled signal handler according to the
      signal handling array.

      \param index number of the currently selected signal handler
      */
      void setActionIndex(int index);
   };
}

#endif
