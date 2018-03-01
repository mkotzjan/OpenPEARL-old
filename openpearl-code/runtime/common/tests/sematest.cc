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

\brief test program for simple semaphore operations

\page Testprograms

\section sematest
This module contains some tasks performing semaphore  operations

The solution looks in PEARL like:

\verbatim
DCL s1 SEMA PRESET(10);
DCL s2 SEMA PRESET(1);
DCL s3 SEMA PRESET(5);

DCL overflow SEMA PRESET (4294967294); ! 2^31-2 --> overflow after 2nd release
T1: TASK PRIO 10 MAIN;
   PUT 'T1 started' TO TERMINAL BY A,SKIP;
   REQUEST s1,s2;
   PUT 'T1' TO TERMINAL BY A,SKIP;
   REQUEST s1,s2;

  RELEASE overflow;
  ON SemaOverflow GOTO ok;
  RELEASE overflow;
  PUT 'signal raising failed' to TERMINAL BY A,SKIP;
  goto done;
ok:
  PUT 'got Signal Overflow Signal' to TERMINAL BY A,SKIP;
done:
   PUT 'T1 terminated' TO TERMINAL BY A,SKIP;
END;

T2: Task PRIO 2 MAIN;
   AFTER 5 SEC RESUME;
   PUT 'T2' TO TERMINAL BY A, SKIP;
   RELEASE s1;
   CONT T3 PRIO 5;    ! change prio of t3 -> is must run before T1
   AFTER 5 SEC RESUME;
   PUT 'T2' TO TERMINAL BY A, SKIP;
   RELEASE s2;
   AFTER 5 SEC RESUME;
   PUT 'T2' TO TERMINAL BY A, SKIP;
   RELEASE s2;
END;

T3: TASK PRIO 20 MAIN;
  AFTER 3 SEC RESUME;
  REQUEST s2;
  PUT 'T3' TO TERMINAL BY A,SKIP;
END;
\endverbatim


The output should look like:

\verbatim
Task T1 started
T1
t2
t2
t3
t2
T1
sequence: 1 1 3 2 2 3 2 1
 ok
\endverbatim

\cond TREAT_EXAMPLES
*/


#include "PearlIncludes.h"
#include "Semaphore.h"
#include <stdio.h>
using namespace std;

//SPCSEMA(_s1);
//SPCSEMA(_s2);
DCLSEMA(_s1, 10);
DCLSEMA(_s2, 1);
DCLSEMA(_s3, 5);
DCLSEMA(_overflow, (unsigned int) - 2); // exeeds at 2nd release

SPCTASK(_MAIN);
SPCTASK(_T1);
SPCTASK(_T2);
SPCTASK(_T3);

int buffer[100];
int expected[10] = {1, 1, 3, 2, 2, 3, 2, 1};
int bp = 0;
DCLTASK(_T1, pearlrt::Prio(10), pearlrt::BitString<1>(1)) {
   int i, fail;

   buffer[bp++] = 1;
   //printf("Task T1 started\n");
   {
      pearlrt::Semaphore* s[2] = {&_s1, &_s2};
      pearlrt::Semaphore::request(me, 2, s);
      buffer[bp++] = 1;
      //   printf("T1\n");
      pearlrt::Semaphore::request(me, 2, s);
      buffer[bp++] = 1;
   }
   printf("T1\n");
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(), pearlrt::Duration(10),
              0);
   printf("sequence: ");
   fail = 0;

   for (i = 0; i < bp; i++) {
      printf("%d ", buffer[i]);

      if (buffer[i] != expected[i]) {
         fail = 1;
      }
   }

   if (fail) {
      printf("\nerror in excution sequence (run as root?)\n");
      pearlrt::Control::setExitCode(1);
   } else {
      printf("\n sequence: ok\n");
   }

   // test sema overflow
   {
      pearlrt::Semaphore* s[1] = {&_overflow};
      pearlrt::Semaphore::release(me, 1, s);

      try {
         pearlrt::Semaphore::release(me, 1, s);
         printf("no signal -- failed\n");
         pearlrt::Control::setExitCode(1);
      } catch (pearlrt::SemaOverflowSignal e) {
         printf("Sema Overflow received -- ok \n");
      }
   }
}

DCLTASK(_T2, pearlrt::Prio(20), pearlrt::BitString<1>(1)) {
   buffer[bp++] = 2;
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(), pearlrt::Duration(5));
   {
      //   printf("t2\n");
      pearlrt::Semaphore* s[2] = {&_s1};
      pearlrt::Semaphore::release(me, 1, s);
   }
   // change prio of t3 -> is must run before t1
   _T3.cont(me, pearlrt::Task::PRIO, pearlrt::Prio(5));
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(), pearlrt::Duration(5));
   {
      buffer[bp++] = 2;
      //   printf("t2\n");
      pearlrt::Semaphore* s[2] = {&_s2};
      pearlrt::Semaphore::release(me, 1, s);
   }
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(), pearlrt::Duration(5));
   {
      buffer[bp++] = 2;
      //  printf("t2\n");
      pearlrt::Semaphore* s[2] = {&_s2};
      pearlrt::Semaphore::release(me, 1, s);
   }
}

DCLTASK(_T3, pearlrt::Prio(15), pearlrt::BitString<1>(1)) {
   buffer[bp++] = 3;
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(), pearlrt::Duration(3));
   {
      pearlrt::Semaphore* s[2] = {&_s2};
      pearlrt::Semaphore::request(me, 1, s);
      buffer[bp++] = 3;
      // printf("t3\n");
   }
}

/**
\endcond
*/

