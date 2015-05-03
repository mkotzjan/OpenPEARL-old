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
 * Timer.h
 *
 *  Created on: 30.05.2014
 *      Author: Florian Mahlecke
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "Fixed.h"
#include "PrioMapper.h"


namespace pearlrt {

class Task;

/*
 * RTC_TIMETYPE_SECOND
 * RTC_TIMETYPE_MINUTE
 * RTC_TIMETYPE_HOUR:
 */


typedef struct _RTC_TIME {

	uint32_t	sec;
	uint32_t	min;
	uint32_t	hour;

} RTC_TIME;




/*********************************************************************//**
 * @brief		TimerInfo structure
 *
 * <b>Note:</b> Includes the timer information
 **********************************************************************/

typedef struct _TimerInfo {

	int32_t activateRun;		///< first-time activation of a task in seconds
	int32_t finalRun;			///< last-time activation of a task in seconds (DURING/UNTIL)
	int32_t nextRun;			///< next activation time of a task in seconds (AT)
	int32_t period;				///< activation period of a task in seconds (ALL)
	Task* tsk;					///< task object reference

} TimerInfo;


class Timer {

	public:
	Timer();
	TimerInfo ti;
	RTC_TIME rtcTime;
	void add(Task* tsk);
	void remove();
	void calculateNextRun(int seconds);
	void RTC_ConfigureRTC(uint32_t sec);
	void resetTimer();
	void secondsToTimerFormat(int seconds);
	int timerFormatToSeconds();
	int getCurrentRTCTimeInSeconds();
};

// end of namespace
}


extern "C" void RTC_IRQHandler();


#endif /* TIMER_H_ */
