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

\brief CLOCK fromat reading test program

\page Testprograms

\section clock_read_test

This program checks whether the Clock values are read as the expected.


\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "Clock.h"
#include "GetClock.h"
#include "GetUtils.h"
#include "Signals.h"
#include "Character.h"
#include "RefChar.h"
#include "RefCharSource.h"

using namespace std;

TEST(GetClock, conversions) {
   //                             12345678901234567890
   pearlrt::Character<20> d(20, "  12:10:21         X");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Clock c, c2;
   pearlrt::GetClock::fromT(c, 19, 0, source);
   // stops at right position?
   source.rewind();
   d = (pearlrt::Character<20>)("  12:10:21.1234    X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 19, 0, source), 0);
   source.rewind();
   d = (pearlrt::Character<20>)("  12:10:21.1234    X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 20, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   // cal hours mod 24?
   source.rewind();
   //                            12345678901234567890
   d = (pearlrt::Character<20>)("   25:10:21.1234   X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 19, 0, source), 0);
   //                              12345678901234567890
   d = (pearlrt::Character<20>)("    1:10:21.1234   X");
   source.rewind();
   ASSERT_EQ(pearlrt::GetClock::fromT(c2, 19, 0, source), 0);
   ASSERT_TRUE((c == c2).getBoolean());
   d = (pearlrt::Character<20>)("    1:10:211234    X");
   source.rewind();
   ASSERT_EQ(pearlrt::GetClock::fromT(c2, 19, 4, source), 0);
   ASSERT_TRUE((c == c2).getBoolean());
   source.rewind();
   d = (pearlrt::Character<20>)("  12:60:21.1234    X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 19, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   source.rewind();
   d = (pearlrt::Character<20>)("  12:10:60.1234    X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 19, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   source.rewind();
   d = (pearlrt::Character<20>)("  12 10:21.1234    X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 19, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   source.rewind();
   d = (pearlrt::Character<20>)("  12:10 21.1234    X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 19, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   source.rewind();
   d = (pearlrt::Character<20>)("  12:10:21 1234    X");
   ASSERT_EQ(pearlrt::GetClock::fromT(c, 19, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   // continue reading at right position?
   //                            12345678901234567890
   d = (pearlrt::Character<20>)("12:10:21 13:11:22 ");
   source.rewind();
   pearlrt::GetClock::fromT(c, 9, 0, source);
   pearlrt::GetClock::fromT(c2, 9, 0, source);
   ASSERT_TRUE((c == pearlrt::Clock(((12 * 60 + 10) * 60) + 21))
               .getBoolean());
   ASSERT_TRUE((c2 == pearlrt::Clock(((13 * 60 + 11) * 60) + 22))
               .getBoolean());
   //                            12345678901234567890
   d = (pearlrt::Character<20>)("1T:10:21 13:11:22 ");
   source.rewind();
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 9, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   pearlrt::GetClock::fromT(c2, 9, 0, source);
   //ASSERT_EQ(c , pearlrt::Clock(((12*60+10)*60)+21) );
   ASSERT_TRUE((c2 == pearlrt::Clock(((13 * 60 + 11) * 60) + 22))
               .getBoolean());
   //                            12345678901234567890
   d = (pearlrt::Character<20>)("1T:10:21 13:11:22 ");
   source.rewind();
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 10, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   pearlrt::GetClock::fromT(c2, 9, 0, source);
   //ASSERT_EQ(c , pearlrt::Clock(((12*60+10)*60)+21) );
   ASSERT_TRUE((c2 == pearlrt::Clock(((3 * 60 + 11) * 60) + 22)).getBoolean());
   // test EOR behavior
   //                            12345678901234567890
   pearlrt::Character<8> d8("14:09:21");
   rc.setWork(d8);
   source.rewind();
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 8, 0, source), 0);
   ASSERT_TRUE((c == pearlrt::Clock(((14 * 60 + 9) * 60) + 21)).getBoolean());
   //                            12345678901234567890
   source.rewind();
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 9, 0, source), 0);
   ASSERT_TRUE((c == pearlrt::Clock(((14 * 60 + 9) * 60) + 21)).getBoolean());
   //                            12345678901234567890
   d = (pearlrt::Character<20>)("12:13:23\n 13:11:22 ");
   rc.setWork(d);
   source.rewind();
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 9, 0, source), 0);
   ASSERT_TRUE((c == pearlrt::Clock(((12 * 60 + 13) * 60) + 23)).getBoolean());
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 9, 0, source),
             pearlrt::theClockValueSignal.whichRST());
   //                            12345678901234567890
   d = (pearlrt::Character<20>)("12:13:23\n 13:11:22 ");
   rc.setWork(d);
   source.rewind();
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 9, 0, source), 0);
   ASSERT_TRUE((c == pearlrt::Clock(((12 * 60 + 13) * 60) + 23)).getBoolean());
   pearlrt::GetUtils::fromSkip(1, source);
   EXPECT_EQ(pearlrt::GetClock::fromT(c, 9, 0, source), 0);
   ASSERT_TRUE((c == pearlrt::Clock(((13 * 60 + 11) * 60) + 22)).getBoolean());
}

/**
\endcond
*/
