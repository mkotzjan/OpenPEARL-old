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

#ifndef TASKTIMERINCLUDED
#define TASKTIMERINCLUDED

/**
\file

\brief timer facility for task scheduling

*/


#include "Clock.h"
#include "Duration.h"
#include "Log.h"
#include "Fixed.h"
#include "TaskCommon.h"
#include "TaskTimerCommon.h"

#include <time.h>

namespace pearlrt {
   /**
   \brief timer facility for task scheduling

   This class provides the timer functions required for task scheduling.

   A timer may be set using the PEARL scheduling parameters.
   A timer my be started. Using the start operation repeatedly will
   restart the timer if it is not expired, or will restart it.
   A timer may be cancelled to disable any further start unless
   it is set again.

   The linux itimer facility is used to realize the timers.
   These itimers are implemented in the FreeRTOS/addOns/timer.c.

   The FreeRTOS variant don't emit a signal, they invoke a callback
   function at the expiration of a timer.
   The timer thread delegates the treatment
   of the timer event of a callback function, which is specified in the
   create()-method
   */
   class TaskTimer : public TaskTimerCommon {
   public:
      /**
      pointer to a callback function defined for easier coding
      */
      typedef void (*TimerCallback)(TaskCommon*);
   private:
      int counts;   //remaining number of timer periods to repeat the action
      int countsBackup;  //number of timer periods to repeat the action
      TaskCommon* task;
      TimerCallback callback;
      int signalNumber;
      timer_t timer;     // the timer
      struct itimerspec its;  // the timer data required in triggered when
      struct {
      		  void *cb;
      		  void *th;
      	  }timer_callback;
   public:

      /**
      ctor - just construct the timer data

      The timer data is set up - the timer is nether set, nor started
      The timer is set via the set() method , which take all
      time related scheduling parameters as given by the scheduling
      operation.
      The timer cancels automatically, if the schedule expires.
      */
      TaskTimer();

      /*
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
      void set(
         int condition,
         Clock at,
         Duration after,
         Duration all,
         Clock until,
         Duration during);

      /**
      cancel the timer.
      A stopped timer may be started again.
      A cancelled timer must be set before it may be restarted.

      In case the timer is active, it is stopped and cancelled.
      If the timer is not active it is only cancelled.

      \returns 0 on success; <br>-1 on error
      */
      int cancel();

   private:
      /**
      stop the timer.

      A stopped timer may be started again.
      A cancelled timer must be set before it may be restarted.

      In case the timer is active, it is stopped.
      If the timer is not active: no operation.

      \returns 0 on success; <br>-1 on error
      */
      int stop();

   public:
      /**
      start the timer.

      At each timer event the callback function is called.

      \returns 0 on success; -1 else
      */
      int start();

      /** check if timer is active
      \returns true, if the timer is active<br>
               false, if the timer is not active
      */
      bool isActive();

      /** check if timer is set

      \returns true, if the timer is set<br>false, if the timer is not set
      */
      bool isSet();

      /**
      initialize the timer facility

      This method must be called at system startup

      \param p the timer threads priority. If -1 the normal scheduling
               is used. If > 0 the FIFO-scheduling with this priority
               is used.

      linux specific initializations:
      <ul>
      <li>setup of time thread
      <li>check for availability of RT-Signals
      </ul>
      */
      static void init(int p);

      /**
      create the timer internal data

      \param task pointer to the task which will use this timer
      \param signalNumber number of the realtime signal for the
             timer
      \param cb callback function, which will be called on expiration
             of the timer
      */
      void create(TaskCommon * task, int signalNumber, TimerCallback cb);

      /**
      update at timer event.

      This method is only required inside of the class.
      It is common to all plattforms. The plattform specific part
      must call this method on each timer event.
      */
      void update();

      /**
      print detailed status of timer into given string

      \param id short form of the time 'ACT' or 'CONT'. This
             parameter is set by the calling function and passed
             to the resulting string
      \param line string which receives the details about the timer
             status
      */
      void detailedStatus(char * id, char * line);
   };




}
#endif

