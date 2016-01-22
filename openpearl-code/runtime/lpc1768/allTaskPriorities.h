/*
 [A "BSD license"]
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

#ifndef ALLTASKPRIORITIES_INCLUDED
#define ALLTASKPRIORITIES_INCLUDED

#include "FreeRTOSConfig.h"

/**
\file

List of task priorities of other system tasks in the complete application.

This list provides the overview of all tasks known by FreeRTOS in the
OpenPEARL application.

There may be tasks for background operations below all PEARL apllications
as well as tasks which must run with better priorities

*/

/*
idle task of FreeRTOS runs with prio 0!
--> let's start with priority 1 as lowest (not important) priority
*/

/**
priority range for PEARL tasks is from PRIO_PEARL_PRIO_MIN 
to PRIO_PEARL_PRIO_MAX
*/
#define PRIO_PEARL_PRIO_MIN 	1
#define PRIO_PEARL_PRIO_MAX  	(PRIO_PEARL_PRIO_MIN+255)

/** 
  the timer task (FreeRTOS/addOns/timer.c) receives the notifications
  from the interrupt service routine. When a time period expired a 
  reschedule may be necessary.
  No application task may run with better priority than this task to enshure
  the correct detection of timeouts
*/
#define PRIO_TASK_SERVICE	(PRIO_PEARL_PRIO_MAX+1)

/**
  The startup and shutdown of application task may not be interrupted
  by other application tasks. This allows a run-to-completion behavior 
*/
#define PRIO_TASK_MAX_PRIO 	(PRIO_TASK_SERVICE+1)

#if (PRIO_TASK_MAX_PRIO >= configMAX_PRIORITIES) 
# error "configMAX_PRIORITIES too small"
#endif

#endif
