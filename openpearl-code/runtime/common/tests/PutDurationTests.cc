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

\brief DURATION output formating test program

\page Testprograms

\section duration_test PUT DURATION Test

\cond TREAT_EXAMPLES
*/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gtest.h"
#include "Duration.h"
#include "PutDuration.h"
#include "Signals.h"
#include "Character.h"
#include "RefChar.h"
#include "RefCharSink.h"

using namespace std;

TEST(PutDuration, PutTests) {
   pearlrt::Character<100> wrk;
   pearlrt::RefCharacter rc;
   rc.setWork(wrk);
   pearlrt::RefCharSink sink(rc);
   pearlrt::Duration d(1.0);
   rc.clear();
   ASSERT_THROW(
      pearlrt::PutDuration::toD(d, 30, -1, sink),
      pearlrt::DurationFormatSignal);
   rc.clear();
   ASSERT_NO_THROW(
      pearlrt::PutDuration::toD(d, 5, 0, sink));
   ASSERT_THROW(
      pearlrt::PutDuration::toD(d, 4, 0, sink),
      pearlrt::DurationFormatSignal);
   rc.clear();
   ASSERT_THROW(
      pearlrt::PutDuration::toD(d, 11, 6, sink),
      pearlrt::DurationValueSignal);
   rc.clear();
   pearlrt::PutDuration::toD(d, 27, 6, sink);
   EXPECT_STREQ(rc.getCstring(), " 0 HRS 00 MIN 01.000000 SEC");
   rc.clear();
   d = pearlrt::Duration(1.0);
   pearlrt::PutDuration::toD(d, 27, 6, sink);
   EXPECT_STREQ(rc.getCstring(), " 0 HRS 00 MIN 01.000000 SEC");
   rc.clear();
   d = pearlrt::Duration(9 * 3600.0 + 8 * 60.0 + 3.1234);
   pearlrt::PutDuration::toD(d, 27, 6, sink);
   EXPECT_STREQ(rc.getCstring(), " 9 HRS 08 MIN 03.123400 SEC");
   rc.clear();
   d = pearlrt::Duration(9 * 3600.0 + 8 * 60.0 + 3.1234);
   pearlrt::PutDuration::toD(d, 23, 2, sink);
   EXPECT_STREQ(rc.getCstring(), " 9 HRS 08 MIN 03.12 SEC");
   rc.clear();
   d = pearlrt::Duration(9 * 3600.0 + 8 * 60.0 + 3.1555);
   pearlrt::PutDuration::toD(d, 23, 2, sink);
   EXPECT_STREQ(rc.getCstring(), " 9 HRS 08 MIN 03.16 SEC");
   rc.clear();
   d = pearlrt::Duration(-1);
   pearlrt::PutDuration::toD(d, 27, 6, sink);
   EXPECT_STREQ(rc.getCstring(), "-0 HRS 00 MIN 01.000000 SEC");
}

TEST(PutDuration, PutTestsShort) {
   pearlrt::Character<100> wrk;
   pearlrt::RefCharacter rc;
   rc.setWork(wrk);
   pearlrt::RefCharSink sink(rc);
   pearlrt::Duration d(1.0);

   rc.clear();
   d = pearlrt::Duration(9 * 3600.0 + 8 * 60.0 + 3.1234);
   ASSERT_THROW(
   //                               12345678901234567890123
   //                              " 9 HRS 08 MIN 03.12 SEC");
      pearlrt::PutDuration::toD(d, 21, 2, sink),
      pearlrt::DurationValueSignal);

   rc.clear();
   d = pearlrt::Duration(0 * 3600.0 + 8 * 60.0 + 3.1234);
   pearlrt::PutDuration::toD(d, 21, 2, sink);
   //                               12345678901234567890123
   //                            "      8 MIN 03.12 SEC");
   EXPECT_STREQ(rc.getCstring(), "      8 MIN 03.12 SEC");

   rc.clear();
   d = pearlrt::Duration(0 * 3600.0 + 8 * 60.0 + 3.1234);
   ASSERT_THROW(
   //                               12345678901234
   //                              " MIN 03.12 SEC");
      pearlrt::PutDuration::toD(d, 14, 2, sink),
      pearlrt::DurationValueSignal);

   rc.clear();
   d = pearlrt::Duration(0 * 3600.0 + 0 * 60.0 + 3.1234);
   pearlrt::PutDuration::toD(d, 8, 2, sink);
   //                             12345678
   //                            "3.12 SEC");
   EXPECT_STREQ(rc.getCstring(), "3.12 SEC");

   rc.clear();
   d = pearlrt::Duration(-(0 * 3600.0 + 0 * 60.0 + 3.1234));
   ASSERT_THROW(
      pearlrt::PutDuration::toD(d, 8, 2, sink),
      pearlrt::DurationValueSignal);

   //EXPECT_STREQ(rc.getCstring(), " 9 HRS 08 MIN 03.16 SEC");

   rc.clear();
   d = pearlrt::Duration(1.0/3.0);
   pearlrt::PutDuration::toD(d, 17, 10, sink);
   //                             12345678901234567
   EXPECT_STREQ(rc.getCstring(), " 0.3333330000 SEC");

}
/**
\endcond
*/
