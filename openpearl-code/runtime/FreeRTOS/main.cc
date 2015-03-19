/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
 Copyright (c) 2014 Rainer Mueller
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
	\brief PEARL90 entry

	system start

 */



//#include <debug_frmwrk.h>


/* Scheduler includes. */
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <semphr.h>


#include "GenericTask.h"
#include "Signals.h"
#include "TaskList.h"
#include "Task.h"
//#include "TimerList.h"
#include "Log.h"
#include "Clock.h"
#include "RTC.h"
//#include "LPC1768Function.h"

using namespace pearlrt;
/*-----------------------------------------------------------*/

int main(void) {
   char line[40];


// prvSetupHardware();
//intDebugMessage("main started\r\n");
//LED_On(7);
   //ED_On(5);
// LED_On(3);
// LED_On(1);
// LED_Off(7);
   Log::info("Defined Tasks");
   Log::info("main started");
   /*
    * This task starts all PEARL90 main tasks, afterwards the
    * task suspends itself until another task resume it
    */
// LED_On(7);
// LED_On(5);
// LED_On(3);
// LED_On(1);
// LED_Off(7);
   Log::info("Defined Tasks");
   sprintf(line, "%-10.10s %4s %s", "Name", "Prio", "isMain");
   Log::info(line);
   TaskList::Instance().sort(); // sort taskList

   for (int i = 0; i < TaskList::Instance().size(); i++) {
      Task *t = TaskList::Instance().getTaskByIndex(i);
      sprintf(line, "%-10.10s  %3d  %2d", t->getName(),
              (t->getPrio()).x,
              t->getIsMain());
      Log::info(line);
      t->init();
   }

// LED_Off(5);

   if (TaskList::Instance().size() == 0) {
      printf("no task defined --> exit.\n");
      //exit(1);
   }

   /*****************init end*******************/
   //activate all threads which declared with "main"
   Log::info("start all main-threads");

   for (int i = 0; i < TaskList::Instance().size();  i++) {
      Task *t = TaskList::Instance().getTaskByIndex(i);

      if (t->getIsMain()) {
         t->activate(t);
      }
   }

   Log::info("system startup complete");
   /* Start the scheduler. */
   pearlrt::RTC::init();
   pearlrt::Clock::setupFromRTC();
   vTaskStartScheduler();
   Log::error("main.c: Scheduler terminated");
   /* IDLE task is declared in LPC1768Function.cc */
}

void vConfigureTimerForRunTimeStats(void) {
}

void vApplicationIdleHook(void) {
   Log::info("idle..");
}

