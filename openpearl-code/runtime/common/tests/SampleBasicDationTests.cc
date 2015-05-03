/*
 [The "BSD license"]
 Copyright (c) 2015-2015 Rainer Mueller
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

\brief demo for DationTS usage

\page Testprograms

\section DationTS Test

\verbatim
MODULE(dationTS_Test);
SYSTEM;

dat : SampleBasicDation(); ! send Fixed(15); and return it with TAKE


PROBLEM;
SPC dat DATION INOUT SYSTEM BASIC FIXED(15) GLOBAL;

TASK1: TASK PRIO 2 MAIN;
BEGIN
    DCL f FIXED(15) INIT(13);
    DCL f1 FIXED(15) INIT(1);

    DCL user DATION INOUT BASIC FIXED(15) CREATED(dat);
    OPEN user;
       SEND f TO user;
       TAKE f1 FROM user;
       IF f /= f1 THEN
          ! error
       FIN; 
     CLOSE user;
END
MODEND.
\endverbatim


\cond TREAT_EXAMPLES
*/

#include "PearlIncludes.h"
#include <iostream>
using namespace std;

SPCTASK(TASK1);

static pearlrt::SampleBasicDation s_dat;
pearlrt::Device * d_dat = (pearlrt::Device*)&s_dat;

int success = 1;
extern pearlrt::Device* d_dat;
static pearlrt::SystemDationB* _dat = (pearlrt::SystemDationB*)d_dat;

//execute function for working thread 1
DCLTASK(TASK1, pearlrt::Prio(2), pearlrt::BitString<1>(1)) {
   pearlrt::Fixed<15> f(13);
   pearlrt::Fixed<15> f1(1);

  //!  DCL user DATION INOUT BASIC FIXED(15) CREATED(dat);
   pearlrt::DationTS user(_dat,pearlrt::Dation::INOUT);

    user.dationOpen(0,(pearlrt::Character<1>*)0, (pearlrt::Fixed<31>*)0);
    user.dationWrite(&f, sizeof(f));
    user.dationRead(&f1, sizeof(f1));
    if ((f != f1).getBoolean()) {
        cout << " read failed: got " << f1.x << " instead of "<< f.x << endl;
    }
    user.dationClose(0, (pearlrt::Fixed<15>*)0);

   if (success) {
      printf("success\n");
   } else {
      printf("fail\n");
   }

}

/**
\endcond
*/

