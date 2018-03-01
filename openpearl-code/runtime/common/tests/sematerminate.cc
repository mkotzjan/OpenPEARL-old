/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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

\brief test program for terminate while blocked on semaphore operations

\page Testprograms

\section sematest
This module tests whether tasks can be terminated while
waiting for a semaphore

The solution looks in PEARL like:

\verbatim
DCL s1 SEMA PRESET(0);
DCL s2 SEMA PRESET(10);

T1: TASK PRIO 10 MAIN;
   PUT 'T1 started' TO TERMINAL BY A,SKIP;
   TERMINATE T2;
   PUT 'T1 finished' TO TERMINAL BY A,SKIP;
END;

T2: Task PRIO 2 MAIN;
   PUT 'T2' TO TERMINAL BY A, SKIP;
   REQUEST s1,s2;
END;
\endverbatim


The output should look like:

\verbatim
Task T2 started
Task T1 started
Task T1 finished
\endverbatim

\cond TREAT_EXAMPLES
*/


#include "PearlIncludes.h"
#include "Semaphore.h"
#include <stdio.h>
using namespace std;

//SPCSEMA(_s1);
//SPCSEMA(_s2);
DCLSEMA(_s1, 0);
DCLSEMA(_s2, 10);

SPCTASK(MAIN);
SPCTASK(T1);
SPCTASK(T2);

DCLTASK(T1, pearlrt::Prio(10), pearlrt::BitString<1>(1)) {
   int ts;
   me->setLocation(__LINE__, __FILE__);
   printf("Task T1 started -- do 10 slow terminates\n");
   me->setLocation(__LINE__, __FILE__);

   for (int i = 0; i < 10; i++) {
      printf("test #%d\n", i);
      me->setLocation(__LINE__, __FILE__);
      T2.activate(me);
      me->setLocation(__LINE__, __FILE__);
      me->resume(pearlrt::Task::AFTER,
                 pearlrt::Clock(), pearlrt::Duration(0.1));
      me->setLocation(__LINE__, __FILE__);
      ts = T2.getTaskState();

      if (ts != pearlrt::TaskCommon::SEMA_BLOCKED) {
         printf("*** wrong task state (%d) (SEMA_BLOCKED expected)\n", ts);
      }

      me->setLocation(__LINE__, __FILE__);
      T2.terminate(me);
      me->setLocation(__LINE__, __FILE__);
      ts = T2.getTaskState();
      me->setLocation(__LINE__, __FILE__);
      me->resume(pearlrt::Task::AFTER,
                 pearlrt::Clock(), pearlrt::Duration(0.1));
      me->setLocation(__LINE__, __FILE__);

      if (ts != pearlrt::TaskCommon::TERMINATED) {
         printf("*** wrong task state (%d) (TERMINATED expected)\n", ts);
      }
   }

   printf("\nT1  -- do 10 fast terminates\n");
   me->setLocation(__LINE__, __FILE__);

   for (int i = 0; i < 10; i++) {
      printf("test #%d\n", i);
      me->setLocation(__LINE__, __FILE__);
      T2.activate(me);
      me->setLocation(__LINE__, __FILE__);
      ts = T2.getTaskState();

      if (ts != pearlrt::TaskCommon::SEMA_BLOCKED) {
         printf("*** wrong task state (%d) (SEMA_BLOCKED expected)\n", ts);
      }

      me->setLocation(__LINE__, __FILE__);
      T2.terminate(me);
      me->setLocation(__LINE__, __FILE__);
      ts = T2.getTaskState();
      me->setLocation(__LINE__, __FILE__);

      if (ts != pearlrt::TaskCommon::TERMINATED) {
         printf("*** wrong task state (%d)(TERMINATED expected)\n", ts);
      }
   }

   me->setLocation(__LINE__, __FILE__);
   printf("Task T1 finished\n");
}
DCLTASK(T2, pearlrt::Prio(2), pearlrt::BitString<1>(0)) {
//   printf("Task T2 started\n");
   me->setLocation(__LINE__, __FILE__);
   {
      pearlrt::Semaphore* s[2] = {&_s1, &_s2};
      me->setLocation(__LINE__, __FILE__);
      pearlrt::Semaphore::request(me, 2, s);
      me->setLocation(__LINE__, __FILE__);
   }
}

/**
\endcond
*/
