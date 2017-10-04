/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

\brief test program for simple bolt operations

\page Testprograms

\section bolttest
This module contains some tasks performing bolt  operations

The solution looks in PEARL like:

\verbatim
DCL s1 BOLT;
DCL s2 BOLT;
DCL s3 BOLT;

T1: TASK PRIO 10 MAIN;
   PUT 'T1 started' TO TERMINAL BY A,SKIP;
   ENTER s1;
   LEAVE s1;
   RESERVE s1;
   FREE s1;

  ! must raise BoltStateSignal
  ON BoltStateSignal GOTO ok;
  LEAVE s1;
  RELEASE overflow;
  PUT 'signal raising failed' to TERMINAL BY A,SKIP;
  goto done;
ok:
  PUT 'got Signal BoltStateSignal' to TERMINAL BY A,SKIP;
done:
   PUT 'T1 terminated' TO TERMINAL BY A,SKIP;
END;
\endverbatim

\cond TREAT_EXAMPLES
*/


#include "PearlIncludes.h"
#include <stdio.h>
using namespace std;

SPCBOLT(_s1);
SPCBOLT(_s2);
DCLBOLT(_s1);
DCLBOLT(_s2);

SPCTASK(_T1);
SPCTASK(_T2);
SPCTASK(_T3);
SPCTASK(_R1);
SPCTASK(_R2);

pearlrt::Fixed<15> n;

char buffer[100];

DCLTASK(_T1, pearlrt::Prio(10), pearlrt::BitString<1>(1)) {

   printf("Task T1: ENTER-LEAVE\n");
   {
      pearlrt::Bolt* s[2] = {&_s1};
      pearlrt::Bolt::enter(me, 1, s);
      pearlrt::Bolt::leave(me, 1, s);
   }
   
   printf("Task T1: RESERVE-FREE\n");
   {
      pearlrt::Bolt* s[2] = {&_s1};
      pearlrt::Bolt::reserve(me, 1, s);
      pearlrt::Bolt::free(me, 1, s);
   }

   printf("Task T1: just LEAVE\n");
   {  int ok = 0;
      pearlrt::Bolt* s[2] = {&_s1};
      try {
         pearlrt::Bolt::leave(me, 1, s);
      } catch (BoltStateSignal s) {
         ok = 1;
         printf("*** success got BoltStateSignal\n");
      }
      if (ok==0) {
         printf("*** fail got no BoltStateSignal\n");
      }
   }

   // test multiple enter
   n = 0;
   *buffer = 0;
   _T2.activate(me);
   _T3.activate(me);
  me->resume(Task::AFTER, Clock(), Duration(1.0),0);
   if (n.x == 2) {
      printf("*** multiple enter is ok\n");
   } else {
      printf("*** multiple enter failed\n");
   }
  me->resume(Task::AFTER, Clock(), Duration(3.0),0);
   if (n.x == 0) {
      printf("*** multiple leave is ok\n");
   } else {
      printf("*** multiple leave failed\n");
   }
   printf("sequence: %s\n", buffer);

   // test multiple reserve
   n = 0;
   *buffer = 0;
   _R1.activate(me);
   _R2.activate(me);
  me->resume(Task::AFTER, Clock(), Duration(1.0),0);
   if (n.x == 1) {
      printf("*** multiple reserve is ok\n");
   } else {
      printf("*** multiple reserve failed\n");
   }
  me->resume(Task::AFTER, Clock(), Duration(3.0),0);
   if (n.x == 0) {
      printf("*** multiple free is ok\n");
   } else {
      printf("*** multiple free failed\n");
   }
   printf("sequence: %s\n", buffer);

   // test priority of reserve before enter
   printf("test reserve priority \n");
   n = 0;
   *buffer = 0;
   _R1.activate(me);
   _T2.activate(me);
   _R2.activate(me);
  me->resume(Task::AFTER, Clock(), Duration(5.0),0);
   if (strcmp(buffer,"R1 R2 T2 ") == 0) {
      printf("*** reserve has priority is ok\n");
   } else {
      printf("*** reserve has priority failed\n");
   }
   printf("sequence: %s\n", buffer);

}

DCLTASK(_T2, pearlrt::Prio(20), pearlrt::BitString<1>(0)) {
  pearlrt::Bolt* s[2] = {&_s1, &_s2};

  pearlrt::Bolt::enter(me, 2, s); 
  n.x++;
  strcat(buffer,"T2 ");
  me->resume(Task::AFTER, Clock(), Duration(2.0),0);
  pearlrt::Bolt::leave(me, 2, s); 
  n.x--;
}

DCLTASK(_T3, pearlrt::Prio(30), pearlrt::BitString<1>(0)) {
  pearlrt::Bolt* s[2] = {&_s1, &_s2};

  pearlrt::Bolt::enter(me, 2, s); 
  n.x++;
  strcat(buffer,"T3 ");
  me->resume(Task::AFTER, Clock(), Duration(2.0),0);
  pearlrt::Bolt::leave(me, 2, s); 
  n.x--;
}

DCLTASK(_R1, pearlrt::Prio(30), pearlrt::BitString<1>(0)) {
  pearlrt::Bolt* s[2] = {&_s1, &_s2};

  pearlrt::Bolt::reserve(me, 2, s); 
  n.x++;
  strcat(buffer,"R1 ");
  me->resume(Task::AFTER, Clock(), Duration(1.0),0);
  pearlrt::Bolt::free(me, 2, s); 
  n.x--;
}

DCLTASK(_R2, pearlrt::Prio(30), pearlrt::BitString<1>(0)) {
  pearlrt::Bolt* s[2] = {&_s1, &_s2};

  pearlrt::Bolt::reserve(me, 2, s); 
  n.x++;
  strcat(buffer,"R2 ");
  me->resume(Task::AFTER, Clock(), Duration(1.0),0);
  pearlrt::Bolt::free(me, 2, s); 
  n.x--;
}


/**
\endcond
*/

