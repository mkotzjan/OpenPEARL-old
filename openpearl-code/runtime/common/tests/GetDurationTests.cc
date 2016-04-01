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

\brief DURATIOn fromat reading test program

\page Testprograms

\section duration_read_test

This program checks whether the Duration values are read as the expected.


\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "Duration.h"
#include "GetDuration.h"
#include "GetUtils.h"
#include "Signals.h"
#include "Character.h"
#include "RefChar.h"
#include "RefCharSource.h"

using namespace std;

TEST(GetDuration, conversions) {
   //                        123456789012345678901234567890
   pearlrt::Character<30> d(" 12 HRS 05 MIN 12 SEC        X");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Duration dur;
   // normal operation
   EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 22, 0, source));
   EXPECT_TRUE((dur == pearlrt::Duration((12 * 60 + 5) * 60 + 12.0))
               .getBoolean());
   // test format parameters
   {
      pearlrt::Character<30> d(" 12 HRS 05 MIN 12 SEC        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationFormatSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, -1, 0, source));
      EXPECT_EQ(pearlrt::theDurationFormatSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 0, 0, source));
      EXPECT_EQ(pearlrt::theDurationFormatSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 20, -1, source));
   }
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("2 HRS 05 MIN 12 SEC        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 18, 0, source));
   }
   // min width
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("2 HRS 05 MIN 12 SEC        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 19, 0, source));
      EXPECT_TRUE((dur == pearlrt::Duration((2 * 60 + 5) * 60 + 12.0))
                  .getBoolean());
   }
   // min width with decimals
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("2 HRS 05 MIN 12.5 SEC        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 21, 0, source));
      EXPECT_TRUE((dur == pearlrt::Duration((2 * 60 + 5) * 60 + 12.5))
                  .getBoolean());
   }
#if 0
   // min width with implicit decimals
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("2 HRS 05 MIN 125 SEC        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 21, 1, source));
      EXPECT_TRUE((dur == pearlrt::Duration((2 * 60 + 5) * 60 + 12.5))
                  .getBoolean());
   }
   // min width with implicit decimals missing prepoint seconds
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("2 HRS 05 MIN 0125 SEC       X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 22, 2, source));
      EXPECT_TRUE((dur == pearlrt::Duration((2 * 60 + 5) * 60 + 1.25))
                  .getBoolean());
   }
   // large hours value with implicit decimals missing prepoint seconds
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12342 HRS 05 MIN 0125 SEC    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 25, 2, source));
      EXPECT_TRUE((dur == pearlrt::Duration((12342 * 60 + 5) * 60 + 1.25))
                  .getBoolean());
   }
   // negative large hours value with implicit decimals
   //  missing prepoint seconds
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("-12342 HRS 05 MIN 0125 SEC  X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 27, 2, source));
      EXPECT_TRUE((dur == pearlrt::Duration(-((12342 * 60 + 5) * 60 + 1.25)))
                  .getBoolean());
   }
#endif
}

TEST(GetDuration, errorDetection) {
   //                        123456789012345678901234567890
   pearlrt::Character<30> d(" 12 HRS 05 MIN 12 SEC        X");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Duration dur;
   // error detection tests
   // no 'SEC' tag
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 05 MIN 12.5        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
#if 0
   // only 1 digit sec
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 05 MIN 2.5 SEC    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
#endif
   // only 1 digit sec with implicit decimals
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 05 MIN 251 SEC    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
   // 3 digit sec with implicit decimals
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 05 MIN 00251 SEC    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
   // no 'MIN' tag
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 05 min 12.5        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
#if 0
   // only 1 digit min
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS  5 MIN 2.5 SEC    X");
      rc.setWork(d);
      source.rewind();

      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
#endif
   // X in field
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 5 MIN 2.5 SEC    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
   // only 3 digit min
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 112 MIN 2.5 SEC    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
   // only 60 min
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 HRS 60 MIN 2.5 SEC    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
   // no 'HRS' tag
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12 hrs 05 MIN 12.5        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 2, source));
   }
   // no hours value
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("  HRS 05 MIN 12.5 SEC        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 21, 2, source));
   }
   // test for proper continuation after error in reading
   {
      pearlrt::Character<60>
      // 12345678901234567890123456789012345678901234567890
      //                          1234567890123456789012345678
      d("12 hrs 05 MIN 12.5       12 HRS 10 MIN 01 SECX");
      rc.setWork(d);
      source.rewind();
      EXPECT_EQ(pearlrt::theDurationValueSignal.whichRST(),
                pearlrt::GetDuration::fromD(dur, 25, 0, source));
      EXPECT_EQ(0, pearlrt::GetDuration::fromD(dur, 20, 0, source));
      EXPECT_TRUE((dur == pearlrt::Duration((12 * 60 + 10) * 60 + 1))
                  .getBoolean());
   }
}

/**
\endcond
*/
