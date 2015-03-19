/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Rainer Mueller
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

\brief task load test program

\page Testprograms

\section lot_of_tasks

This program tests the behavior of the system when a lot of tasks
is started. This is done using the API directly, since PEARL does not
support dynamic tasks.

The parameter N is set by a C-define.
N tasks are created in a loop. All task work on the same code, which
just incremsts a static viarable. i
All tasks are scheduled to start at the same time, 
which is the current time + 2 sec.
At the end of all tasks, this 
variable should contain the value N.

The main task prints all 1 second the number of
finished tasks until ether 8 seconds have passed, or all 
N workers have finished.
AT the end diagnostic message (ok/fail) is printed.
In case of fail, the exit code is 1 instead of 0.

\todo Due to an error in the pthread-library the program dies 
occationally with a segmentation fault.
see:  http://zhenprogramming.wordpress.com/2009/08/27/max-number-of-threads/
<br>This was seen with g++ 4.3.1 20080507
<br>In https://bugzilla.yoctoproject.org/show_bug.cgi?id=2862 there is
a note that this bug is fixed; lets wait for flow back to the pthread lib

Sample output at N=500:
\verbatim
wait max 8 sec for completion
1 sec: 28 (of 500) tasks finished
2 sec: 123 (of 500) tasks finished
3 sec: 209 (of 500) tasks finished
4 sec: 304 (of 500) tasks finished
5 sec: 399 (of 500) tasks finished
6 sec: 485 (of 500) tasks finished
7 sec: 500 (of 500) tasks finished
     seems to be ok
*** no more tasks active or scheduled --> stop
\endverbatim


The  log file contains detailed information about the run.

\cond TREAT_EXMAPLES
*/
#include <stdio.h>
#include <stdlib.h>

#include "PearlIncludes.h"

using namespace pearlrt;
#define N  500
static volatile int startcounter =0;
SPCTASK(together);

DCLTASK(together,pearlrt::Prio(4),pearlrt::BitString<1>(0)) {
   startcounter ++;
}
Task* manyTasks[N];

//execute function for working thread 1
DCLTASK(startup,pearlrt::Prio(2),pearlrt::BitString<1>(1)) {
   Clock start;
   Clock time1, time2, time3;
   char* name;
   double delta;

   int i;
   for (i=0; i<N; i++) {
      name=(char*)malloc(100);
      sprintf(name,"task#%d",i);
      manyTasks[i] = new Task(together_entry,
			name,pearlrt::Prio(6),
                        pearlrt::BitString<1>(0));
   } 
   /*do something*/
  
   start= Clock::now() + Duration(4);

   printf("activating %d tasks \n", N);
   Log::warn("activate %d tasks", N);
   time1= Clock::now();
   for (i=0; i<N; i++) {
       manyTasks[i]->activate( me, Task::PRIO | Task::AT,
				pearlrt::Prio(10),
 				start);
   }
   time2= Clock::now();
   Log::warn("activates done");

   printf("wait max 8s for completion \n");
   for (i=1; i<=8 && startcounter != N; i++) { 
     me ->resume(Task::AFTER, 
           Clock(), 
           Duration(1.0));
     printf("%d sec: %d (of %d) tasks finished\n",i, startcounter,N);
   }
   if (startcounter == N) {
      time3= Clock::now();
      printf("     seems to be ok\n");
      Log::warn("all tasks terminated");

   } else {
      printf("     failed\n");
      pearlrt::Control::setExitCode(1);
   }
   printf("found timing:\n");
   delta = (time2-time1).get().get()/1000.0 / N;
   printf("time for activate: %f ms/Task\n",delta);
   delta = (time3-time2).get().get()/1000.0 / N;
   printf("time for complete: %f ms/Task\n",delta);
}


/** 
\endcond
*/
