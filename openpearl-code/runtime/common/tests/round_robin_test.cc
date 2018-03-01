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

\brief round robin test

\page Testprograms

\section round_robin_test
This module contains a procedute which starts twi tasks which
run the round robin test. PEARL requires that runnable tasks with
the same priority are scheduled with round robin behavior.

\note There is a problem in the linux scheduler in kernel 2.6. 
Threads are not handed over immediatelly to the SCHED_RR. This leeds
to a longer active time at the first loop.

The solution looks in PEARL like:

\verbatim
...

DCL stop_rr_test FIXED INIT(0);

rr_test: PROC;
    DCL start CLOCK;
    start := NOW() + 2 SEC;
    AT start ACIVATE RR1 PRIO 3;
    AT start ACTIVTE RR2 PRIO 3;
    AFTER 10 SEC RESUME;
    stop_rr_test := 1;
END;

rr_test_function: PROC(id FIXED);
   DCL (min,max,sum,dur) DURATION;
   DCL first(10) DURATION;
   DCL loopcounter FIXED INIT(1);

   max := 0 SEC;
   min := 0 SEC;
   WHILE stop_rr_test EQ 1 REPEAT
      start := NOW();
      commox_x = id;
      WHILE common_x EQ id AND stop_rr_test EQ 0 REPEAT END;
      IF NOT stop_rr_test THEN 
          dur := NOW() - start;
          IF (loopcounter EQ 0) THEN
              min := dur;
              max := dur;
              sum := dur;
          ELSE
              IF (dur < min) THEN 
                 min := dur;
              FIN;
              IF (dur > max) THEN 
                 max := dur;
              FIN;
              sum := sum + dur;
          FIN;
          loopcounter := loopcounter + 1;
          IF loopcounter LE 10 THEN
             first(loopcounter) := dur;
          FIN;
      FIN;
   END;

   IF stop_rr_test > 0 AND loopcounter EQ 0 THEN
       PUT 'RR',id,': round robin seens to be off' TO console BY A,F(1),A,SKIP;
   ELSE
       PUT 'RR',id,':+++++++++++++++++++++++',
                'round robin scheduling works' TO console 
           BY A,F(1),A,,SKIP,A,SKIP;
       IF first(1) > 1 SEC THEN
          PUT '   !!! first duration > 1 sec' TO console BY A, SKIP;
          sum := sum - first(1);
          loopcounter := loopcounter - 1;
       FIN;
       PUT 'RR',id,' time sline min:=',min TO console 
          BY A,F(1),A,DUR(30,6),SKIP;
       PUT 'RR',id,' time sline max:=',max TO console 
          BY A,F(1),A,DUR(30,6),SKIP;
       sum := sum / loopcounter; 
       PUT 'RR',id,' time sline avg:=',sum TO console 
          BY A,F(1),A,DUR(30,6),SKIP;
   FIN;
END;

RR1: TASK;
   CALL rr_test_function(1);
END

RR2: TASK;
   CALL rr_test_function(2);
END

\endverbatim


The output should look like:

\verbatim
RR1 and RR2 will stop  soon
RR1: +++++++++++++++++++
round robin scheduling works
    !!! first time slice > 1s 
    RR1: time slice size: min=    0 HRS 00 MIN 00.195971 SEC
    RR1: time slice size: max=    0 HRS 00 MIN 02.342754 SEC
    RR1: time slice size: avg=    0 HRS 00 MIN 00.225593 SEC
RR2: +++++++++++++++++++
round robin scheduling works
    !!! first time slice > 1s 
    RR2: time slice size: min=    0 HRS 00 MIN 00.195989 SEC
    RR2: time slice size: max=    0 HRS 00 MIN 01.451971 SEC
    RR2: time slice size: avg=    0 HRS 00 MIN 00.226657 SEC
\endverbatim

\cond TREAT_EXAMPLES
*/
/*
Module information (to be treated by module linker)
compile date: ... 
compiler version: ....

Begin(Tasks): (prio, ismain)
RR1 6,0
RR2 6,0
End(Tasks)

Begin(Semas)
End(Semas)

*/
#include <stdio.h>

#include "PearlIncludes.h"
using namespace pearlrt;

SPCTASK(RR1);
SPCTASK(RR2);

static volatile int stop_rr_test = 0;
static volatile int common_x = 0;


static void rr_test_function(Task* me, int id) {
   Clock start;
   Duration dur,min,max,sum;
   Duration first[10];
   int loopcounter = 0;
   Character<40> wrk;
   RefCharacter rc(wrk);
   RefCharSink sink(rc);

   max = Duration();
   sum = Duration();
printf("rr_test func %d\n",id);
   while(!stop_rr_test) {
      start = Clock::now();
      common_x = id;
      // wait for other thread
      while(common_x == id && ! stop_rr_test) {
//         start = Clock::now();
      }
      // thread was preempted or terminating flag is set
      if (!stop_rr_test) {
         dur = Clock::now()-start;   
         if (loopcounter++ == 0) {
            min = dur;
            max = dur;
            sum = dur;
         } else {
            if (dur.compare(min) < 0) min = dur;
            if (dur.compare(max) > 0) max = dur;
            sum += dur;
         }
         if (loopcounter <10) {
            first[loopcounter-1] = dur;
         }
      } 
   }

  if (stop_rr_test && (loopcounter == 0)) {
      printf("RR%d: round robin scheduling seems to be off\n",id);
  } else {
      printf("RR%d: +++++++++++++++++++\n"
             "round robin scheduling works (%d loops)\n",id,loopcounter);
      if (first[0].compare(Duration(1.0)) > 0 ) {
          printf("    !!! first time slice > 1s \n");
          sum -= first[0];
          loopcounter --;
      }
      printf("    RR%d: time slice size: min=" , id);
           rc.clear();        
           PutDuration::toD(min,30,6, sink);
           printf("%s \n", rc.getCstring());
      printf("    RR%d: time slice size: max=" , id); 
           rc.clear();        
           PutDuration::toD(max,30,6, sink);
           printf("%s \n", rc.getCstring());
      sum = sum / (Float<24>)(loopcounter);
      printf("    RR%d: time slice size: avg=" , id);
           rc.clear();        
           PutDuration::toD(sum,30,6, sink);
           printf("%s \n", rc.getCstring());
  }
}

void rr_test(Task * me) {
   Clock end;
Log::info("rrtest: called");
   printf("RR1 and RR2 start in 2s running 10s\n");
   end = Clock::now() + Duration(2.0);
   RR1.activate(me, pearlrt::Task::PRIO | Task::AT ,
                        pearlrt::Prio(3), 
			end,  // at
			Duration(), Duration(),Clock(), Duration());
Log::info("rrtest: RR1 scheduled");
   RR2.activate(me, pearlrt::Task::PRIO | Task::AT ,
                        pearlrt::Prio(3), 
			end,  // at
			Duration(), Duration(),Clock(), Duration());
   me->resume(Task::AFTER, 
           Clock(), 
           Duration(10.0),
           0);
   stop_rr_test = 1 ;
   printf("RR1 and RR2 will stop  soon\n");
}   


DCLTASK(RR1,pearlrt::Prio(6),pearlrt::BitString<1>(0)) {
   rr_test_function(me,1);
}

DCLTASK(RR2,pearlrt::Prio(6),pearlrt::BitString<1>(0)) {
   rr_test_function(me,2);
}


/**
\endcond
*/

