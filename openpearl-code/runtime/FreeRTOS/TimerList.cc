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
 * TimerList.cc
 *
 *  Created on: 30.05.2014
 *      Author: Florian Mahlecke
 */


#include "TimerList.h"
#include "chip.h"	// LpcOpen

namespace pearlrt {

   /*********************************************************************//**
    * @brief		List of sorted timer objects
    *
    * <b>Note:</b> Sorted in ascending order according to the execution
    * time of tasks
    **********************************************************************/


   TimerList& TimerList::Instance() {
      static TimerList tl;
      return tl;
   }

   /*********************************************************************//**
    * @brief		Comparator function
    * @param[in]	left Timer object pointer
    * @param[in]	right Timer object pointer
    *
    * <b>Note:</b> Sorts the timer objects
    **********************************************************************/

   bool CompareTimer::operator()(const Timer* left, const Timer* right) {
      if (left->ti.nextRun < right->ti.nextRun) {
         return true;
      } else {
         return false;
      }
   }

   /*********************************************************************//**
    * @brief		Add timer object to timer object list
    * @param[in]	timer Timer object reference
    **********************************************************************/


   void TimerList::addTimerToList(Timer* timer) {
      TimerList::Instance().tl.insert(timer);
   }

   /*********************************************************************//**
    * @brief		Remove timer object from timer object list
    * @param[in]	timer Timer object reference
    **********************************************************************/

   void TimerList::removeTimerFromList(Timer* timer) {
      tl.erase(timer);
   }

   /*********************************************************************//**
    * @brief		Returns Timer pointer on the first timer object
    *                   in timer object list
    * @return		Timer object pointer
    **********************************************************************/

   Timer* TimerList::getTimer() {
      // find first element in list
      std::set<Timer*, CompareTimer>::iterator it;
      it = TimerList::Instance().tl.begin();
      Timer *tm;
      tm = *it;
      return *it;
   }

   int TimerList::getSize() {
      return TimerList::Instance().tl.size();
   }


   void TimerList::setAlaram() {
      Timer* timer = getTimer();

      if (timer->rtcTime.sec != 255) {
         Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);
      }

      if (timer->rtcTime.min != 255) {
         Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMMIN, ENABLE);
      }

      if (timer->rtcTime.hour != 255) {
         Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMHOUR, ENABLE);
      }

      timer->secondsToTimerFormat(timer->ti.nextRun);
   }


   /*********************************************************************//**
    * @brief		Debug function
    * @param[in]	none
    *
    * <b>Note:</b> Print sorted timer object list to serial port
    **********************************************************************/

#ifdef DEBUGMODE
   void TimerList::printList() {
      std::set<Timer*, CompareTimer>::iterator it;
      printDebugMessage("tasks sorted by nextRun");
      Timer* debugTimer;
      char debugTaskName[20];

      for (it = tl.begin(); it != tl.end(); it++) {
         debugTimer = *it;
         printDebugMessage(const_cast<char*>(debugTimer->ti.tsk->getName()));
      }

      printDebugMessage("starting debug loop in TimerList::printList()");

      // debug loop
      while (1) {}
   }
#endif

// end of namespace
}




