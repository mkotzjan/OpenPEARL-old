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

\brief API demo program

\page Testprograms

\section api tests/api.cc

This program demonstrates how the PEARL run time system may be used.
It activates two tasks.

The PEARL code for this program would be:
\verbatim
MODULE(api);
PROBLEM;
SPC console DATION ALPHIC OUT;

t1: TASK PRIO 1 MAIN;
   DCL d DURATION INIT(5 SEC);
   DCL dur DURATION;
   dur := d * 10.0;
   PUT '5 sec delay' TO console BY A,SKIP;
   AFTER d RESUME;
   PUT 'after 5 sec all 2 sec during 30 sec activate t2' TO console BY A,SKIP;
   AFTER 5 SEC ALL 2 SEC DURING 30 SEC ACTIVATE t2;
END;

DCL t2counter FIXED INIT(0);

t2: TASK PRIO 20;
   Duration d INIT(10 SEC);
   PUT 't2 started ',F(3) TO console BY A, SKIP;
   d := d / (t2counter-2);   // cause divide by zero signal
END;

MODEND.
\endverbatim

\cond TREAT_EXAMPLES
*/

/*
 TASKS
   t1
   t2 
*/
#include "PearlIncludes.h"

using namespace std;
char*filename = (char*)"api.prl";
SPCTASK(t1);
SPCTASK(t2);

DCLTASK(t1,pearlrt::Prio(10),pearlrt::BitString<1>(1)) {
   me->setLocation(10,filename);
   pearlrt::Duration d(5.0);
   me->setLocation(11,filename);
   pearlrt::Duration dur;
   pearlrt::Float<24> f(10.0);
   me->setLocation(12,filename);
   dur = f*d;
   dur = d*f;
   me->setLocation(13,filename);
   printf("5 sec delay\n");
   me->setLocation(14,filename);
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(),d, 0);
   me->setLocation(15,filename);
   printf("after 5 sec all 2 sec during 30 sec activate  t2\n");
   printf("  -> 13 activations derived from the effective period of 25 sec\n");
   printf("  -> activation #2 will die due to div0 signal\n");
   me->setLocation(16,filename);
   t2.activate(&t2, pearlrt::Task::PRIO |
               pearlrt::Task::AFTER|pearlrt::Task::DURING|pearlrt::Task::ALL, 
	       pearlrt::Prio(30),
               pearlrt::Clock(), d, pearlrt::Duration(2.0), // at, all 
               pearlrt::Clock(),d*(pearlrt::Float<24>)6.0) ; 
}

int t2counter=0;
DCLTASK(t2,pearlrt::Prio(20),pearlrt::BitString<1>(0)) {
   me->setLocation(17,filename);
   printf("t2 started %d\n", ++t2counter);
   me->setLocation(18,filename);
   pearlrt::Duration d(10.0);
   me->setLocation(19,filename);
   d = d / pearlrt::Float<24>(t2counter-2);
}

/**
\endcond 
*/
