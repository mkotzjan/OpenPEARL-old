/*
 * time_addons.h
 *
 *  Created on: 04.12.2015
 *      Author: r. mueller
 */
/*
 [The "BSD license"]
 Copyright (c) 2015 rainer mueller
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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
\file

This module extends the function interface of the UNIX time.h.
This extension is used in the adaption layer for FreeRTOS for time.h
*/

/**
register the time base function.

Depending on the selected time base, there are different functions
which deliver the current time. This may be the RTC or other sources

\param set  arm the timer to inform timer.c about the timeout
\param get  pointer to the function, which return the current time
*/
void register_timer_source( void (*set)(uint64_t ns),
                            void (*get)(uint64_t * ns));


/**
register the time base function.

configure clock.c to work with a proper time base

\param get  pointer to the function, which return the current time
*/
void set_clock_gettime_cb(void (*get)(uint64_t *ns));

/**
get current time in nsec 

\param nsec pointer to uint64_t value which receives the curent time
*/
void clock_gettime_nsec(uint64_t * nsec);

/**
trigger the timer task to check which timers reached their timeout 

This function must be invoked from the time base classes.
*/
void resume_timer_task_from_ISR();

#ifdef __cplusplus
}
#endif

