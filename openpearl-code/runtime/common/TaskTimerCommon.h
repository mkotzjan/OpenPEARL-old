/*
 [The "BSD license"]
 Copyright (c) 2015 Rainer Mueller
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

#ifndef TASKTIMERCOMMONINCLUDED
#define TASKTIMERCOMMONINCLUDED

/**
\file

\brief timer facility for task scheduling

*/


#include "Clock.h"
#include "Duration.h"
#include "Fixed.h"
#include "TaskCommon.h"

namespace pearlrt {
   /**
   \brief timer facility for task scheduling

   This class provides the timer functions required for task scheduling.

   A timer may be set using the PEARL scheduling parameters.
   A timer my be started. Using the start operation repeatedly will
   restart the timer if it is not expired, or will restart it.
   A timer may be cancelled to disable any further start unless
   it is set again.
   */
   class TaskTimerCommon {
   public:
      /**
      pointer to a callback function defined for easier coding
      */
      typedef void (*TimerCallback)(TaskCommon*);

   public:

      /**
      setup the timer for the given schedule

      The method check for valid scheduling conditions and
      throws exceptions if the scheduling condition is not valid.

      If the scheduling condition is valid, the systems timer data
      structure is set up.

      \param condition 	the scheduling condition. This parametered is an
                       	ORed expression of task scheuduling flags
      \param at	       	an exact start time
      \param after	start delay
      \param all	the cycle period
      \param until    	an exact end time
      \param during   	the execution period for the timer. The enabled
                        time period starts relative to the execution
                        of this statement
      */
      virtual void set(
         int condition,
         Clock at,
         Duration after,
         Duration all,
         Clock until,
         Duration during) = 0;

      /**
      cancel the timer.
      A stopped timer may be started again.
      A cancelled timer must be set before it may be restarted.

      In case the timer is active, it is stopped and cancelled.
      If the timer is not active it is only cancelled.

      \returns 0 on success; <br>-1 on error
      */
      virtual int cancel() = 0;

      /**
      start the timer.

      The time begins its execution. Each concrete TaskTimer must
      contain a callback function, which is call at each elapsed time
      interval.

      \returns 0 on success; -1 else
      */
      virtual int start() = 0;

      /** check if timer is active

      Active means that the timer is started and its final time is
      not reached yet.

      \returns true, if the time is still active<br>false else
      */
      virtual bool isActive() = 0;

      /** check if timer is set

      This function returns true if the method set() was called
      at least one time and is not cancelled.
      Thus the timer be be started again.

      \returns true, if the time is set <br>false else
      */
      virtual bool isSet() = 0;
   };

}
#endif

