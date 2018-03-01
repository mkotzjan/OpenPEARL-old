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

\brief scheduling behavior demo

\page Testprograms

\section sched_demo

This program checks the schulding behavior.
TASK1 controls the system. 
Task1 activates Task2 with different priority and 
loops until a specified time has passed.
At the end it calls the function rr_test, which shall test whethe the
round-robin scheduling of tasks of the same priority works.

Task2 schedules Task3 for cyclic activation and performs an endless
loop with delays and printing.

Task3 just prints a message at each activation.

The PEARL code would be:
\verbatim
MODULE(sched_demo);
PROBLEM;
SPC console DATION ALPHIC OUT GLOBAL;

SPC rr_test PROC GLOBAL;

TASK1: TASK PRIO 2 MAIN;
BEGIN
     DCL end CLOCK;

     ACTIVATE TASK2 PRIO 10;
     PUT "Task1..." BY A, SKIP;
     end = NOW() + 10 SEC;

     WHILE NOW() < end REPEAT
         PUT NOW(), "Task 1 laeuft" TO TERM BY D(15,6),A,SKIP;
         AFTER 2 SEC RESUME;
     END;
     PUT 'Task1: preventing Task3' TO TERM BY A,SKIP;
     PREVENT TASK3;
     PUT 'Task1: terminating Task2' TO TERM BY A,SKIP;
     TERMINATE TASK2;

     CALL rr_test;

     PUT 'Task1 finished' TO TERM BY A,SKIP;
END;

TASK2: TASK PRIO 4;
   AFTER 5 SEC ALL 3 SEC ACTIVATE TASK3;
   REPEAT
      PUT NOW(),'            2 laeuft' TO TERM BY D(15,6),A,SKIP;
      AFTER 5 SEC RESUME;
   END;
END;

TASK3: TASK PRIO 6;
   PUT NOW(),'                          3 laeuft' TO TERM BY D(15,6),A,SKIP;
END;

MODEND.
\endverbatim


The expected output of schould look like:
\verbatim
 9:44:25.306900: activate TASK2
 9:44:25.309080: 1 laueft
 9:44:25.310312:       2 laeuft
 9:44:27.309900: 1 laueft
 9:44:29.311084: 1 laueft
 9:44:30.311141:       2 laeuft
 9:44:30.311690:                  3 laeuft
 9:44:31.311960: 1 laueft
 9:44:33.311723:                  3 laeuft
 9:44:33.312733: 1 laueft
 9:44:35.311523:       2 laeuft
 9:44:35.314427Task1: preventing TASK3
 9:44:35.316202Task1: terminate Task2
 9:44:35.316290perform rr-test 
 ..... (continuating output is shown in rr_test-section) ....

\endverbatim

\cond TREAT_EXAMPLES
*/

/*
Module information (to be treated by module linker)
compile date: ... 
compiler version: ....

Begin(Tasks): (prio, ismain)
TASK1 2,1
TASK2 4,0
TASK3 6,0
End(Tasks)

Begin(Semas)
End(Semas)

*/
#include "PearlIncludes.h"

using namespace std;
using namespace pearlrt;
extern void rr_test(Task * me);


SPCTASK(TASK1);
SPCTASK(TASK2);
SPCTASK(TASK3);

 
//execute function for working thread 1
DCLTASK(TASK1,pearlrt::Prio(2),pearlrt::BitString<1>(1)) {
   Clock end;
   Clock c;
   Character<16> ttt;
   RefCharacter rc;
   rc.setWork(ttt);
   rc.clear();
   RefCharSink sink(rc);
   /*do something*/
  
   c = Clock::now();
   rc.clear();
   PutClock::toT(c,15,6,sink);
   printf("%s: activate TASK2\n", rc.getCstring());
   TASK2.activate(me, pearlrt::Task::PRIO, pearlrt::Prio(10)); //  priority

   end = Clock::now() + Duration(10);

   while( Clock::now().compare(end) < 0)
   {
     c = Clock::now();
     rc.clear();
     PutClock::toT(c,15,6,sink);
     printf("%s: 1 laueft\n",rc.getCstring());

     me ->resume(Task::AFTER, 
           Clock(), 
           Duration(2.0),
           0);
   }
   c = Clock::now();
   rc.clear();
   PutClock::toT(c,15,6,sink);
   printf("%s:  Task1: preventing TASK3\n",rc.getCstring());
   TASK3.prevent(me);
   c = Clock::now();
   rc.clear();
   PutClock::toT(c,15,6,sink);
   printf("%s: Task1: terminate Task2\n", rc.getCstring());
   TASK2.terminate(me);
 
   /*--- test of round-robin-timeslice within tasks of same priority ---*/
   c = Clock::now();
   rc.clear();
   PutClock::toT(c,15,6,sink);
   printf("%s: Task1: perform rr-test\n", rc.getCstring());
   rr_test((Task*)me);

   c = Clock::now();
   rc.clear();
   PutClock::toT(c,15,6,sink);
   printf("%s: Task1: finished \n", rc.getCstring());

   /*------------------*/
}

//execute function for working thread 2
DCLTASK(TASK2,pearlrt::Prio(4),pearlrt::BitString<1>(0)) {
   Character<16> ttt;
   RefCharacter rc;
   rc.setWork(ttt);
   rc.clear();
   RefCharSink sink(rc);
   
   /* do something*/
   TASK3.activate(me, pearlrt::Task::PRIO | Task::AFTER | Task::ALL ,
                        pearlrt::Prio(3),  // prio
			Clock(0.0),  // at
			Duration(5.0),  // after
			Duration(3.0),  // all
   			Clock(0.0),		// until
			Duration(0.0));		// during
   while(1)
   {
     Clock c = Clock::now();
     rc.clear();
     PutClock::toT(c,15,6,sink);
     printf("%s:       2 laeuft\n", rc.getCstring());
     //sleep for 5 seconds
     
     me->resume(Task::AFTER, 
			Clock(0.0), // at
			Duration(5.0), // after
			0); // when

   }

   /*------------------*/
}

//execute function for working thread 3
DCLTASK(TASK3,pearlrt::Prio(6),pearlrt::BitString<1>(0)) {
   Character<16> ttt;
   RefCharacter rc;
   rc.setWork(ttt);
   rc.clear();
   RefCharSink sink(rc);
   
   /* do something*/
     
     Clock c = Clock::now();
     rc.clear();
     PutClock::toT(c,15,6,sink);
     printf("%s:                  3 laeuft\n", rc.getCstring());
   
   /*------------------*/
}

/**
\endcond
*/

