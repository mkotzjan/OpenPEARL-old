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

\brief demo for interrupt handling

\page Testprograms

\section interruptTest

\verbatim
MODULE(interruptTest);
PROBLEM;
SPC ctrlc INTERRUPT;

TASK1: TASK PRIO 2 MAIN;
BEGIN

     ENABLE ctrlc;

     WHEN ctrlc ACTIVATE TASK2;
     WHEN ctrlc AFTER 1 SEC ACTIVATE TASK3;
     WHEN ctrlc CONTINUE TASK4;
     AFTER 5 SEC RESUME;
END;

TASK2: TASK PRIO 4;
   AFTER 5 SEC ALL 3 SEC ACTIVATE TASK3;
   REPEAT
      PUT NOW(),'            2 laeuft' TO TERM BY D(15,6),A,SKIP;
      AFTER 5 SEC RESUME;
   END;
END;

MODEND.
\endverbatim


\cond TREAT_EXAMPLES
*/

#include "PearlIncludes.h"

using namespace std;

SPCTASK(TASK1);
SPCTASK(TASK2);
SPCTASK(TASK3);
SPCTASK(TASK4);
SPCTASK(TASK5);
SPCTASK(TASK6);

//static pearlrt::UnixSignal sys_ctrlc(2);
static pearlrt::SoftInt sys_ctrlc(2);
pearlrt::Interrupt * _ctrlc = (pearlrt::Interrupt*)&sys_ctrlc;

volatile int x = 0;
volatile int c = 0;
int success = 1;
//execute function for working thread 1
DCLTASK(TASK1, pearlrt::Prio(2), pearlrt::BitString<1>(1)) {
   /*do something*/
//   int caught;
   printf("TASK1 laeuft\n");

//   caught = 0;

//   try {
   TASK2.cont(me, pearlrt::Task::PRIO | pearlrt::Task::WHEN,
              pearlrt::Prio(80),
              pearlrt::Clock(),
              pearlrt::Duration(),
              pearlrt::Duration(),
              pearlrt::Clock(),
              pearlrt::Duration(),
              _ctrlc);
//  } catch (pearlrt::PriorityNotMapableSignal &s) {
//     caught = 1;
//  }

   // if (caught == 1) {
   //    printf("ok - invalid priority for continue detected \n");
   // } else {
   //    printf("fail - invalid priority for continue "
   //           "not detected (run as root!)\n");
   //    success = 0;
   //}

   TASK2.prevent(me);

   //caught = 0;

//  try {
   TASK2.activate(me, pearlrt::Task::PRIO | pearlrt::Task::WHEN,
                  pearlrt::Prio(80),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);
   // } catch (pearlrt::PriorityNotMapableSignal &s) {
   //   caught = 1;
//  }

//  if (caught == 1) {
//     printf("ok - invalid priority detected for activate \n");
//  } else {
//     printf("fail - invalid priority not detected "
//            " for activate(run as root!)\n");
//     success = 0;
//  }

   //TASK2.prevent(me);
   // wait for completion and reset x in case of used as
   // non root
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(1.5), 0);
   x = 0;

   TASK2.activate(me, pearlrt::Task::PRIO | pearlrt::Task::WHEN,
                  pearlrt::Prio(20),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);

   printf("TASK1 test immediate triggered activate disabled interrupt \n");
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.5), 0);

   if (x != 0) {
      printf("simple trigger disabled fail (x=%d)\n", x);
      success = 0;
   }

   _ctrlc->enable();

   // test simple trigger
   printf("TASK1 test immediate triggered activate\n");
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.5), 0);

   if (x != 1) {
      printf("simple trigger 1 fail (x=%d)\n", x);
      success = 0;
   }

   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.5), 0);

   if (x != 2) {
      printf("simple trigger 2 fail (x=%d)\n", x);
      success = 0;
   }

   TASK2.prevent(me);
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.5), 0);

   if (x != 2) {
      printf("prevent task2 fail (x=%d)\n", x);
      success = 0;
   }

   pearlrt::Log::error("********************************");
   x = 0;
   printf("TASK1 test delayed activate\n");
   pearlrt::Log::info("delayed trigger start");
   TASK3.activate(me,
                  pearlrt::Task::PRIO | pearlrt::Task::WHEN |
                  pearlrt::Task::AFTER,
                  pearlrt::Prio(10),
                  pearlrt::Clock(),
                  pearlrt::Duration(1.0),  // after
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);

   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (x != 0) {
      printf("delayed trigger 1a fail (x=%d)\n", x);
      success = 0;
   }

   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.4), 0);

   if (x != 1) {
      printf("delayed trigger 1b fail (x=%d)\n", x);
      success = 0;
   }

   pearlrt::Log::error("********************************");

   x = 0;
   pearlrt::Log::info("delayed trigger 2.0");
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.7), 0);
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.7), 0);
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.7), 0);

   if (x != 0) {
      printf("delayed trigger 2a fail (x=%d)\n", x);
      success = 0;
   }

   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.5), 0);

   if (x != 1) {
      printf("delayed trigger 2b fail (x=%d)\n", x);
      success = 0;
   }

   pearlrt::Log::info("delayed trigger end");
   pearlrt::Log::error("********************************");
   x = 0;
   printf("TASK1 test immediate and delayed activate (TASK2 and TASK3)\n");
   TASK2.activate(me, pearlrt::Task::PRIO | pearlrt::Task::WHEN,
                  pearlrt::Prio(10),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);

   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (x != 1) {
      printf("delayed trigger 2 tasks fail@1  (x=%d)\n", x);
      success = 0;
   }

   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.4), 0);

   if (x != 2) {
      printf("delayed trigger 2 tasks fail@2 (x=%d)\n", x);
      success = 0;
   }

   TASK2.prevent(me);
   TASK3.prevent(me);


   // ---------------------------------------------------------
   printf("TASK1 test long working activate\n");
   x = 0;
   TASK4.activate(me,
                  pearlrt::Task::PRIO | pearlrt::Task::WHEN,
                  pearlrt::Prio(10),
                  pearlrt::Clock(),
                  pearlrt::Duration(),  // after
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);

   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (x != 0) {
      printf("delayed trigger 2 tasks fail@1 (x=%d)\n", x);
      success = 0;
   }

   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.4), 0);

   if (x != 1) {
      printf("delayed trigger 2 tasks fail@2 (x=%d)\n", x);
      success = 0;
   }

   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.3), 0);
   _ctrlc->trigger();  // Task4 active --> buffer activation
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.3), 0);
   _ctrlc->trigger();  // Task4 active --> overrun
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.3), 0);

   if (x != 1) {
      printf("delayed trigger 2 tasks fail@3 (x=%d)\n", x);
      success = 0;
   }

   // wait for pending run to be finished
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(1.0), 0);

   if (x != 2) {
      printf("delayed trigger 2 tasks fail@4 (x=%d)\n", x);
      success = 0;
   }

   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (x != 3) {
      printf("delayed trigger 2 tasks fail@5 (x=%d)\n", x);
      success = 0;
   }

   TASK4.prevent(me);
   // ---------------------------------------------------------
   pearlrt::Log::info("simple activate must remove when");
   x = 0;
   TASK2.activate(me, pearlrt::Task::PRIO | pearlrt::Task::WHEN,
                  pearlrt::Prio(10),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (x != 1) {
      printf("simple activate must remove when fail@1 (x=%d)\n", x);
      success = 0;
   }

   TASK2.activate(me, pearlrt::Task::PRIO ,
                  pearlrt::Prio(10),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (x != 2) {
      printf("simple activate must memove when fail@2 (x=%d)\n", x);
      success = 0;
   }

   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (x != 2) {
      printf("simple activate must memove when fail@3 (x=%d)\n", x);
      success = 0;
   }

   TASK2.prevent(me);

   // ---------------------------------------------------------
   printf("test when on continue   \n");
   c = 0;

   TASK5.activate(me, pearlrt::Task::PRIO,
                  pearlrt::Prio(10),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  pearlrt::Duration(),
                  pearlrt::Clock(),
                  pearlrt::Duration(),
                  _ctrlc);
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);
   TASK5.cont(me, pearlrt::Task::WHEN,
              pearlrt::Prio(), pearlrt::Clock(), pearlrt::Duration(),
              pearlrt::Duration(), pearlrt::Clock(), pearlrt::Duration(),
              _ctrlc);
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (c != 1) {
      printf("when continue failed (c=%d)\n", c);
      success = 0;
   }

   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (c != 1) {
      printf("when continue failed -- still triggered (c=%d)\n", c);
      success = 0;
   }

   TASK5.cont(me, pearlrt::Task::WHEN,
              pearlrt::Prio(), pearlrt::Clock(), pearlrt::Duration(),
              pearlrt::Duration(), pearlrt::Clock(), pearlrt::Duration(),
              _ctrlc);
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.8), 0);

   if (c != 2) {
      printf("when continue failed -- still triggered (c=%d)\n", c);
      success = 0;
   }

   TASK5.prevent(me);

   // ----------------------------------------------------
   printf("WHEN ... AFTER ... CONTINUE\n");
   c = 0;
   TASK6.cont(me, pearlrt::Task::WHEN | pearlrt::Task::AFTER,
              pearlrt::Prio(),
              pearlrt::Clock(),
              pearlrt::Duration(1.0), // after
              pearlrt::Duration(),
              pearlrt::Clock(),
              pearlrt::Duration(),
              _ctrlc);
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.1), 0);
   TASK6.activate(me);
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.5), 0);
   _ctrlc->trigger();
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(), pearlrt::Duration(0.5), 0);

   if (c != 0) {
      printf("when after continue failed@1 (c=%d)\n", c);
      success = 0;
   }

   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(),
               pearlrt::Duration(0.8), 0);

   if (c != 1) {
      printf("when after continue failed@2 (c=%d)\n", c);
      success = 0;
      pearlrt::Log::error("*** emergency contine for TASK6");
      TASK6.cont(me); // emergency continue
   }



   if (success) {
      printf("success\n");
   } else {
      printf("fail\n");
   }

}

//execute function for working thread 2
DCLTASK(TASK2, pearlrt::Prio(4), pearlrt::BitString<1>(0)) {
   x ++;
   printf("Task2: x=%d\n", x);
}
//execute function for working thread 3
DCLTASK(TASK3, pearlrt::Prio(4), pearlrt::BitString<1>(0)) {
   x ++;
   printf("Task3: x=%d\n", x);
}
//execute function for working thread 4
DCLTASK(TASK4, pearlrt::Prio(4), pearlrt::BitString<1>(0)) {
   me ->resume(pearlrt::Task::AFTER,
               pearlrt::Clock(),
               pearlrt::Duration(1.0), 0);
   x ++;
   printf("Task4: x=%d\n", x);
}
//execute function for working thread 5
DCLTASK(TASK5, pearlrt::Prio(4), pearlrt::BitString<1>(0)) {
   me->suspend(me);
   c ++;
   printf("Task5: c=%d\n", c);
   me->suspend(me);
   c ++;
   printf("Task5: c=%d\n", c);
}

DCLTASK(TASK6, pearlrt::Prio(4), pearlrt::BitString<1>(0)) {
   me->suspend(me);
   c ++;
   printf("Task6: c=%d\n", c);
}

/**
\endcond
*/

