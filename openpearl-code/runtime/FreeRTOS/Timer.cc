/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
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

/*
 * Timer.cc
 *
 *  Created on: 30.05.2014
 *      Author: florian
 */



#include "Timer.h"
#include "TimerList.h"
#include "TaskCommon.h"
#include "chip.h"		// LpcOpen


namespace pearlrt {


   Timer::Timer() {
      rtcTime.sec = 255;
      rtcTime.min = 255;
      rtcTime.hour = 255;
   }



   /*********************************************************************//**
    * @brief		Set RTC timer
    *
    * <b>Note:</b> Set timer to timer object list, fetch the next
    *               upcoming alarm from list and set timer to RTC
    **********************************************************************/

   void Timer::add(Task* tsk) {
      Timer* tm = &tsk->timer;
      TimerList::Instance().addTimerToList(tm);
   }

   /*********************************************************************//**
    * @brief		Remove RTC timer from list
    *
    * <b>Note:</b> Remove RTC timer from list
    **********************************************************************/

   void Timer::remove() {
      TimerList::Instance().removeTimerFromList(
         TimerList::Instance().getTimer());
   }

   void Timer::resetTimer() {
      LPC_RTC->CIIR = 0;
      LPC_RTC->AMR = 255;
      LPC_RTC->ILR = 0x02;
   }


   void Timer::RTC_ConfigureRTC(uint32_t sec) {
      Chip_RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, sec);
   }


   void Timer::secondsToTimerFormat(int seconds) {
      int tmp;
      rtcTime.hour = seconds / 3600;
      tmp = seconds % 3600;
      rtcTime.min = tmp / 60;
      rtcTime.sec = tmp % 60;
   }


   int Timer::timerFormatToSeconds() {
      return (rtcTime.hour * 3600 + rtcTime.min * 60 + rtcTime.sec);
   }


   int Timer::getCurrentRTCTimeInSeconds() {
      RTC_TIME_T time;
      Chip_RTC_GetFullTime(LPC_RTC, &time);
      return (time.time[RTC_TIMETYPE_HOUR] * 3600 +
              time.time[RTC_TIMETYPE_MINUTE] * 60 +
              time.time[RTC_TIMETYPE_SECOND]);
   }





// end of namespace
}



void RTC_IRQHandler() {
   if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
      // get current timer object from list
      pearlrt::Timer* timer = pearlrt::TimerList::getTimer();
      // get prio
//		pearlrt::Fixed<15> fp = timer->ti.tsk->PearlPriority.get();
      pearlrt::Fixed<15> fp = timer->ti.tsk->getPrio();
      unsigned portBASE_TYPE p =
         pearlrt::PrioMapper::getInstance()->fromPearl(fp);
      // activate current task
      timer->ti.tsk->directActivate(p);
      // remove current timer object from list
      timer->remove();

      if (timer->ti.finalRun <= timer->getCurrentRTCTimeInSeconds()) {
         if (timer->ti.period != 0) {
            timer->ti.nextRun = timer->ti.nextRun + timer->ti.period;
            timer->add(timer->ti.tsk);
         }
      }

      // remove pending INT
      Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
      int size = pearlrt::TimerList::Instance().getSize();

      if (size != 0) {
         pearlrt::Timer* nextTimer;
         nextTimer = pearlrt::TimerList::getTimer();
         Chip_RTC_SetAlarmTime(LPC_RTC, RTC_TIMETYPE_SECOND, nextTimer->ti.nextRun);
      } else {
         // reset timer
         timer->resetTimer();
      }
   }
}




