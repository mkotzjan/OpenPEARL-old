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

\brief suspend continue test

\page Testprograms

\section suspend_continue_test
This module contains a procedute which starts two tasks which
tests whether suspend and continue operates correctly.

The solution looks in PEARL like:

\verbatim
...


DCL s CLOCK;
DCL d DURATION;
DCL counter1 FIXED63 INIT(0);
DCL counter2 FIXED63 INIT(0);

SUSP_CONT_TEST: TASK PRIO 1 MAIN;
    ACTIVATE SUSPENDED_TASK PRIO 30;
    ACTIVATE CONTINUER PRIO 31; ! continue should be detected immediatelly
    AFTER 4 SEC RESUME;
    IF d < 0.1 SEC THEN
        PUT 'ok' TO screen BY A, SKIP;
    ELSE
        PUT 'too long delay!' TO screen BY A, SKIP;
    FIN;
    ! perform priority change test 
    s := NOW();
    ACTIVATE C1;
    ACTIVATE C2;
    AFTER 5 SEC RESUME; ! wait for C1,C2 to finish
   
   ! perform remote termination of selfSUSPENDED_TASK task
   ACTIVATE SUSPENDED_TASK;
   AFTER 0.5 SEC RESUME;
   TERMINATE SUSPENDED_TASK;
   PUT 'remote termination of self suspended works' TO screen BY A, SKIP;

   ! perform remote suspending and continue tests
   counter1 := 0;
   ACTIVATE  COUNTER PRIO 255;
   AFTER 1 SEC RESUME;
   IF counter1 eq 0 THEN
      PUT 'Task COUNTER did not start - fail' TO screen BY A,SKIP;
   FIN;
   SUSPEND COUNTER;
   counter1 := 0;
   AFTER 1 SEC RESUME;
   IF counter1 eq 0 THEN
      PUT 'Task COUNTER unchanged - ok' TO screen BY A,SKIP;
   ELSE
      PUT 'Task COUNTER changed Task not suspended - fail - ' 
         TO screen BY A,SKIP;
   FIN;
   TERMINATE COUNTER;

   ! perform cyclic continue tests
END;

SUSPENDED_TASK: TASK;
    SUSPEND;
    d := NOW()-s;
END;
 
CONTINUER: TASK;
   AFTER 1 SEC RESUME;
   s := NOW();
   CONTINUE SUSPENDED_TASK;
   WHILE (s+3SEC) < NOW() REPEAT
   END;
END;

!
! Test priority change and premption on the fly

C1: TASK PRIO 35;
   DCL c2start FIXED(63);
   c2start := counter2;
   WHILE (s+3SEC) < NOW() REPEAT
      counter1 := counter1 + 1;
   END;
   if counter2 eq c2start then 
      PUT ' counter2 should remain unchanged - ok' TO screen BY A, SKIP;
   else
      PUT ' counter2 changed - fail' TO screen BY A, SKIP;
   fin;
END; 

C2: TASK PRIO 33;
   WHILE(s+1SEC) < NOW() REPEAT
      counter2 := counter2 + 1;
   END;
   if counter1 ne 0 then 
      PUT ' counter1 should be 0 is ', counter1, " - fail' TO screen BY 
           A, F(4), SKIP;
   else
      PUT ' counter1 is 0 - ok' TO screen BY A, SKIP;
   fin;
   CONTINUE C1 PRIO 30; ! test priority change and premption on the fly
   WHILE(s+3SEC) < NOW() REPEAT
      counter2 := counter2 + 1;
   END;
END;

COUNTER: TASK;
  CLOCK e;
  e := NOW() + 5SEC;
  WHILE(e > NOW()) REPEAT
     counter1 := counter1 + 1;
  END;
END;
 
 \endverbatim


The output should look like:

\verbatim
ok
\endverbatim

\cond TREAT_EXAMPLES
*/

#include <stdio.h>
#include "PearlIncludes.h"
#include <inttypes.h>     
using namespace pearlrt;
using namespace std;

char fn[] = "SUSP_CONT_TEST.prl";


SPCTASK(SUSPENDED_TASK);
SPCTASK(CONTINUER);
SPCTASK(SUSP_CONT_TEST);
SPCTASK(C1);
SPCTASK(C2);
SPCTASK(COUNTER);

static Clock s;
static Duration d;
static Fixed63 counter1((Fixed63::Fixed63_t)0LL);
static Fixed63 counter2((Fixed63::Fixed63_t)0LL);

DCLTASK(SUSP_CONT_TEST,pearlrt::Prio(1),pearlrt::BitString<1>(1)) 
{
   me->setLocation(__LINE__,fn);
   SUSPENDED_TASK.activate(me, pearlrt::Task::PRIO, pearlrt::Prio(30));
   me->setLocation(__LINE__,fn);
   CONTINUER.activate(me, pearlrt::Task::PRIO, pearlrt::Prio(31));
   me->setLocation(__LINE__,fn);
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(4.0),
           0);
   me->setLocation(__LINE__,fn);
   if (d.compare(Duration(0.1)) < 0) {
      printf("SUSP_CONT_TEST: not preemptied: ok\n");
   } else {
      printf("SUSP_CONT_TEST: preemptied by other task - fail\n");
  }

   me->setLocation(__LINE__,fn);
   SUSPENDED_TASK.activate(me, pearlrt::Task::PRIO, pearlrt::Prio(32));
   me->setLocation(__LINE__,fn);
   CONTINUER.activate(me, pearlrt::Task::PRIO, pearlrt::Prio(31));
   me->setLocation(__LINE__,fn);
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(4.0),
           0);
   me->setLocation(__LINE__,fn);
   if (d.compare(Duration(0.1)) < 0) {
      printf("priority change problem -- fail!\n");
   } else {
      printf("long delay is ok\n");
   }
   me->setLocation(__LINE__,fn);
   s = Clock::now();
   me->setLocation(__LINE__,fn);
   C1.activate(me);
   me->setLocation(__LINE__,fn);
   C2.activate(me);
   me->setLocation(__LINE__,fn);
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(5.0),
           0);

   // perform remote termination of selfSUSPENDED_TASK task
   me->setLocation(__LINE__,fn);
   for (int i=0; i<10; i++) {
   printf("remote termination test (%d of 10) \n",i+1);
   Log::info("************************************************");
   Log::info("remote termination test (%d of 10)",i+1);
   me->setLocation(__LINE__,fn);
   SUSPENDED_TASK.activate(me, pearlrt::Task::PRIO, pearlrt::Prio(32));
   me->setLocation(__LINE__,fn);
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(1.0),
           0);
   me->setLocation(__LINE__,fn);
   SUSPENDED_TASK.terminate(me);
   printf("remote termination of self suspended works (%d of 10) \n",i+1);
   Log::info("remote termination test (%d of 10) -- done",i+1);
  } 
    
   // perform remote suspending and continue tests
   printf("do remote suspend/continue tests... \n");
   me->setLocation(__LINE__,fn);
   counter1 = Fixed63((Fixed63::Fixed63_t)0LL);
   me->setLocation(__LINE__,fn);
   COUNTER.activate(me, pearlrt::Task::PRIO, pearlrt::Prio(255));
   me->setLocation(__LINE__,fn);
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(1.0),
           0);
   me->setLocation(__LINE__,fn);
   if (counter1.get() == 0 ) {
      printf("Task COUNTER did not start - fail\n");
   }
   me->setLocation(__LINE__,fn);
   COUNTER.suspend(me);
   me->setLocation(__LINE__,fn);
   counter1 = Fixed63((Fixed63::Fixed63_t)0LL);
   me->setLocation(__LINE__,fn);
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(1.0),
           0);
   me->setLocation(__LINE__,fn);
   if (counter1.get() == 0 ) {
      printf("Task COUNTER did not anything - ok\n");
   } else {
      printf("Task COUNTER changed Task not suspended - fail - \n");
   }
   me->setLocation(__LINE__,fn);
   printf(" terminate remote COUNTER\n");
   COUNTER.terminate(me);
   
   printf("--- done --- \n");
}


DCLTASK(SUSPENDED_TASK,pearlrt::Prio(255),pearlrt::BitString<1>(0)) 
{
   me->setLocation(__LINE__,fn);
Log::info("SUSPENDED_TASK: going to suspend\n");
   me->suspend(me);
Log::info("SUSPENDED_TASK: resumed\n");
   me->setLocation(__LINE__,fn);
   d = Clock::now() - s;
Log::info("SUSPENDED_TASK: end of code\n");
   me->setLocation(__LINE__,fn);
}

DCLTASK(CONTINUER,pearlrt::Prio(255),pearlrt::BitString<1>(0)) 
{
   me->setLocation(__LINE__,fn);
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(1.0),
           0);
   me->setLocation(__LINE__,fn);
   s = Clock::now();
printf("CONTINUER: continue SUSPENDED_TASK\n");
   me->setLocation(__LINE__,fn);
   SUSPENDED_TASK.cont(me);
printf("CONTINUER: continue SUSPENDED_TASK done\n");

   me->setLocation(__LINE__,fn);
   while ((s+Duration(3.0)).compare(Clock::now()) > 0) {
//      me->setLocation(__LINE__,fn);
   }
Log::info("CONTINUER: end of code\n");

}

/*
Test priority change and premption on the fly
*/

DCLTASK(C1,pearlrt::Prio(35),pearlrt::BitString<1>(0))
{
   Fixed63 c2start;
   Clock e;
   me->setLocation(__LINE__,fn);
   e = s + Duration(3.0);
   c2start = counter2;
   while (e.compare(Clock::now()) > 0) {
      me->setLocation(__LINE__,fn);
      counter1 = counter1 + Fixed63((Fixed63::Fixed63_t)1LL);
   }
   me->setLocation(__LINE__,fn);
   if (counter2.get() == c2start.get()) {
      printf("C1: counter2 should remain unchanged - ok\n");
   } else {
      printf("C1: counter2 changed - fail\n");
   }
}

DCLTASK(C2, pearlrt::Prio(33),pearlrt::BitString<1>(0))
{
   Clock e;
   me->setLocation(__LINE__,fn);
   e = s + Duration(1.0);
Log::info("C2: 1 sec active delay start");
   me->setLocation(__LINE__,fn);
   while (e.compare(Clock::now()) > 0) {
      me->setLocation(__LINE__,fn);
      counter2 = counter2 + Fixed63((Fixed63::Fixed63_t)1); 
   }
Log::info("C2: 1 sec active");
   me->setLocation(__LINE__,fn);
   if (counter1.get() > 0) {
      printf("C2: counter1 should be 0 - is (%" PRId64 ") fail\n", counter1.get());
   } else {
      printf("C2: counter1 is  0 - ok\n");
   }
   // test priority change and premption on the fly
   me->setLocation(__LINE__,fn);
Log::info("C2: C1.cont 30");
   C1.cont(me,pearlrt::Task::PRIO, pearlrt::Prio(30));
   me->setLocation(__LINE__,fn);
   e = Clock::now() + Duration(3.0);
   me->setLocation(__LINE__,fn);
   while (e.compare(Clock::now())> 0) {
      me->setLocation(__LINE__,fn);
      counter2 = counter2 + Fixed63((Fixed63::Fixed63_t)1LL); 
   }
   me->setLocation(__LINE__,fn);
}

DCLTASK(COUNTER,pearlrt::Prio(255),pearlrt::BitString<1>(0))
{
   Clock e;
   me->setLocation(__LINE__,fn);
   e = Clock::now() + Duration(15.0);
   me->setLocation(__LINE__,fn);
   while (e.compare(Clock::now()) > 0) {
      me->setLocation(__LINE__,fn);
      counter1 = counter1 + Fixed63((Fixed63::Fixed63_t)1LL); 
      me->setLocation(__LINE__,fn);
   }
   me->setLocation(__LINE__,fn);
   Log::info("COUNTER: end reached");
   me->setLocation(__LINE__,fn);
}

/**
\endcond
*/

