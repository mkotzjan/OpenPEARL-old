/*
 [The "BSD license"]
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

\brief Test of suspend and terminate during dation i/o

\page Testprograms

\section DationSuspendTerminateTests tests/DationSuspendSuspendTerminateTests.cc

Dation SUSPEND and TERMINATE tests
Task READER opens dation as input. And tries to read.
Task CONTROL sunspends READER AFTER 1 sec. The task state of READER
should remain ACTIVE until the SKIP is reached.
Then the state schould be SUSPENDED.
The same process is done for FERMNATE.

These tests must be done interactive.
*/
#include "Task.h"
#include "Signals.h"
#include "StdOut.h"
#include "Pipe.h"
#include "Log.h"
#include "Fixed.h"
#include "Character.h"
#include "Dation.h"
#include "DationPG.h"
#include "DationDim2.h"

using namespace std;

static pearlrt::Pipe myPipe("/tmp/pipe1", 10, "CAN OPEN1");
static pearlrt::StdOut stdOut;
static pearlrt::Fixed<31>  x(42);

SPCTASK(READER);
SPCTASK(CONROL);
/**
the READER task
*/
DCLTASK(READER, pearlrt::Prio(30), pearlrt::BitString<1>(0)) {
   pearlrt::DationDim2 dim(80);
   pearlrt::DationPG reader(&myPipe,
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   reader.dationOpen(
         0, //pearlrt::Dation::ANY ,
         (pearlrt::Character<1>*) 0, //&fn,
         (pearlrt::Fixed<15>*)NULL);
for (int i=0; i<10; i++) {
printf("READER@1 me=%p\n", me);
   try {
      reader.beginSequence(me);
         reader.fromF(x, (pearlrt::Fixed<31>)3);
printf("READER x=%d\n", x.x);
         reader.fromSkip(1);
         reader.fromF(x, (pearlrt::Fixed<31>)3);
printf("READER x=%d\n", x.x);
         reader.fromSkip(1);
      reader.endSequence();
   } catch (pearlrt::Signal s) {
printf("READER: got signal\n");
      reader.endSequence();
   }
//      me->resume(pearlrt::Task::AFTER,
//                 pearlrt::Clock(), pearlrt::Duration(1.0));
}
   reader.dationClose(0, (pearlrt::Fixed<15>*)0);
printf("Task READER terminates\n");
}

/**
the CONTROL task
*/
DCLTASK(CONTROL, pearlrt::Prio(10), pearlrt::BitString<1>(1)) {
   pearlrt::Fixed<31>  y;
   pearlrt::DationDim2 dim(80);
   pearlrt::Character<1> fn;
   pearlrt::DationPG writer(&myPipe,
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   pearlrt::DationPG console(&stdOut,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   writer.dationOpen(
         0, //pearlrt::Dation::ANY ,
         (pearlrt::Character<1>*) 0, //&fn,
         (pearlrt::Fixed<15>*)NULL);
   console.dationOpen(
         0, //pearlrt::Dation::ANY ,
         (pearlrt::Character<1>*) 0, //&fn,
         (pearlrt::Fixed<15>*)NULL);
                              // 12345678901234567890123456789012345678901234567890
   pearlrt::Character<50> text0("Test SUSPEND+TERMINATE in Dation i/o");
   pearlrt::Character<50> text1("the value of x should change after CR");
   pearlrt::Character<50> text2("the input detection should work again");
   pearlrt::Character<50> text3("the Task READER should be terminated now");
   console.toA(text0);
   console.toSkip(1);
   READER.activate(me);
       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     
       y = 12; 
       writer.toF(y, (pearlrt::Fixed<31>)3);
       writer.toSkip(1);
printf("CONTROL: written %d\n", y.x);

       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     
       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     
          console.toA(text1);
          console.toSkip(1);
printf("CONTROL: suspending READER\n");
          READER.suspend(me);
printf("CONTROL: READER suspended \n");
      me->resume(pearlrt::Task::AFTER,
                 pearlrt::Clock(), pearlrt::Duration(5.0));

       y = 16; 
printf("CONTROL: try to write in pipe..\n");
       writer.toF(y, (pearlrt::Fixed<31>)3);
       writer.toSkip(1);
printf("CONTROL: written %d\n", y.x);

       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     
       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     
       y = 17; 
       writer.toF(y, (pearlrt::Fixed<31>)3);
       writer.toSkip(1);
printf("CONTROL: written %d\n", y.x);
       y = 18; 
       writer.toF(y, (pearlrt::Fixed<31>)3);
       writer.toSkip(1);
printf("CONTROL: written %d\n", y.x);
       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     
Log::info("**** READER.cont()");
       READER.cont(me);
      me->resume(pearlrt::Task::AFTER,
                 pearlrt::Clock(), pearlrt::Duration(5.0));

       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     

          console.toA(text2);
          console.toSkip(1);
Log::info("**** READER.cont()");
          READER.cont(me);
       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);
     
       console.toF(x, (pearlrt::Fixed<31>)3);
       console.toSkip(1);

          console.toA(text3);
          console.toSkip(1);

Log::info("**** READER.terminate()");
          READER.terminate(me);
       printf("write to pipe to enable reader to finish\n");
       y = 20; 
       writer.toF(y, (pearlrt::Fixed<31>)3);
       writer.toSkip(1);
printf("CONTROL: written %d\n", y.x);

      me->resume(pearlrt::Task::AFTER,
                 pearlrt::Clock(), pearlrt::Duration(5.0));

      
   console.dationClose(0, (pearlrt::Fixed<15>*)0);
}
