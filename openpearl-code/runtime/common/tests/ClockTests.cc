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

\brief CLOCK test program

\page Testprograms

\section clock_test

This program checks whether the Clock class delivers the expected
results.
The program tests all constructors and all operations.
It compares the real results with the expected results.
This includes the exception generation.


\cond TREAT_EXAMPLES
*/
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "gtest.h"

#include "Duration.h"
#include "Clock.h"
#include "Signals.h"
#include "compare.h"

using namespace std;

TEST(Clock, Ctors) {
   /*
      ASSERT_THROW(pearlrt::Clock c((char*)"123"),
                   pearlrt::ClockValueSignal);
      ASSERT_THROW(pearlrt::Clock c((char*)"12:60:43.123456");
                   pearlrt::ClockValueSignal);
      ASSERT_THROW(pearlrt::Clock c((char*)"12:43:60.123456");
                   pearlrt::ClockValueSignal);
      ASSERT_THROW(pearlrt::Clock c((char*)"12 43:60.123456");
                   pearlrt::ClockValueSignal);
   */
   // threshold should be +0.000001
   EXPECT_NO_THROW(pearlrt::Clock c(9223372036854.775807 + 0.001));
   EXPECT_NO_THROW(pearlrt::Clock c(24.0 * 60 * 60));
   EXPECT_NO_THROW(pearlrt::Clock c(24.0 * 60 * 60 - 0.01));
   EXPECT_NO_THROW(pearlrt::Clock c(-0.01));
   EXPECT_NO_THROW(pearlrt::Clock c(1.01));
}

TEST(Clock, Operations) {
   pearlrt::Clock c1(1 * 3600 + 2 * 60 + 0);
   pearlrt::Clock c2(1 * 3600 + 2 * 60 + 3);
   pearlrt::Duration d(1.9);
   ASSERT_TRUE((c1 < c2).getBoolean());
   ASSERT_TRUE((c2 == c2).getBoolean());
   ASSERT_TRUE((c2 > c1).getBoolean());
   {
      c1 = pearlrt::Clock(10.1);
      ASSERT_EQ(c1.getSec(), 10);
      ASSERT_EQ(c1.getUsec(), 100000);
   }
   {
      c1 = pearlrt::Clock(10.1);
      d = pearlrt::Duration(1.9);
      ASSERT_TRUE((c1 + d == pearlrt::Clock(12.0)).getBoolean());
   }
   {
      c1 = pearlrt::Clock(23 * 3600 + 59 * 60 + 59.0);
      d = pearlrt::Duration(1.0);
      ASSERT_TRUE((c1 + d == pearlrt::Clock()).getBoolean());
   }
   {
      c1 = pearlrt::Clock();
      d = pearlrt::Duration(-1.0);
      ASSERT_TRUE((c1 + d == pearlrt::Clock(23 * 3600 + 59 * 60 + 59.0))
                  .getBoolean());
   }
   {
      c1 = pearlrt::Clock(10.1);
      d = pearlrt::Duration(1.9);
      ASSERT_TRUE((c1 - d == pearlrt::Clock(8.2)).getBoolean());
   }
   {
      c1 = pearlrt::Clock(23 * 3600 + 59 * 60 + 59.0);
      d = pearlrt::Duration(-1.0);
      ASSERT_TRUE((c1 - d == pearlrt::Clock(0)).getBoolean());
   }
   {
      c1 = pearlrt::Clock();
      d = pearlrt::Duration(1.0);
      ASSERT_TRUE((c1 - d == pearlrt::Clock(23 * 3600 + 59 * 60 + 59.0))
                  .getBoolean());
   }
   {
      c1 = pearlrt::Clock(10.1);
      c2 = pearlrt::Clock(11.1);
      d = c2 - c1;
      ASSERT_TRUE((d == pearlrt::Duration(1.0)).getBoolean());
   }
}


/**
\endcond
*/
