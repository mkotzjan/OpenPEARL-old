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

\brief DURATION test program

\page Testprograms

\section duration_test

This program checks whether the Fixed63 class delivers the expected
results.
The program tests all constructors and all operations.
It compares the real results with the expected results.
This includes the exception generation.

All individual tests are logged to the standard out channel together with
a rating of the result.
In case that all tests pass the return value is 0.

\cond TREAT_EXAMPLES
*/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "gtest.h"
#include "Duration.h"
#include "Clock.h"
#include "Signals.h"
#include "compare.h"

using namespace std;

TEST(Duration, Operations) {
   ASSERT_NO_THROW(
//       pearlrt::Duration c(9223372036854.775807));
      pearlrt::Duration c(922337203685.775));
   ASSERT_THROW(
      // threshold shuold should be +0.000001
      pearlrt::Duration c(9223372036854.775807 + 0.001),
      pearlrt::DurationRangeSignal);
   {
      pearlrt::Duration d(24 * 3600.0); // 1 day
      pearlrt::Duration d2;
      pearlrt::Fixed<31> preset(100000000);
      int i = 0;
      int test = 0;

      try {
         d2 = d2 + d * pearlrt::Float<24>(preset + pearlrt::Float<24>(0.0));

         for (i = 0; i < 1000000000; i++) {
            d2 = d2 + d;
         }
      } catch (pearlrt::DurationRangeSignal &s) {
         test = 1;
         printf(".. duration overflow works (%d days)\n", i + preset.x);
      }

      ASSERT_EQ(test, 1);
   }
   {
      pearlrt::Duration d(24 * 3600.0); // 1 day
      pearlrt::Duration d2;
      pearlrt::Fixed<31> preset(100000000);
      int i = 0;
      int test = 0;

      try {
         d2 = d2 - d * pearlrt::Float<24>(preset + pearlrt::Float<24>(0.0));

         for (i = 0; i < 1000000000; i++) {
            d2 = d2 - d;
         }
      } catch (pearlrt::DurationRangeSignal &s) {
         test = 1;
         printf(".. duration underflow works (%d days)\n", i + preset.x);
      }

      ASSERT_EQ(test, 1);
   }
   {
      pearlrt::Duration d(10.0);
      pearlrt::Duration d2(5.0);
      d = d - d2;
      ASSERT_TRUE((d == d2).getBoolean());
   }

   {
      pearlrt::Duration d(5.0);
      pearlrt::Duration d2(5.0);
      d = d + d2;
      ASSERT_TRUE((d == pearlrt::Duration(10.0)).getBoolean());
   }

   {
      pearlrt::Duration d(5.0);
      d = d * pearlrt::Float<24>(2.5);
      ASSERT_TRUE((d == pearlrt::Duration(12.5)).getBoolean());
   }
   {
      pearlrt::Duration d(5.0);
      d = pearlrt::Float<24>(2.5) * d;
      ASSERT_TRUE((d == pearlrt::Duration(12.5)).getBoolean());
   }
   {
      pearlrt::Duration d(5.0);
      d = d / (pearlrt::Float<24>)(0.5);
      ASSERT_TRUE((d == pearlrt::Duration(10.0)).getBoolean());
   }
   ASSERT_THROW(pearlrt::Duration(5.0) / pearlrt::Float<24>(0.0),
                pearlrt::DurationDivideByZeroSignal);
   {
      pearlrt::Duration d(5.0);
      pearlrt::Duration d2(2.5);
      ASSERT_TRUE((d / d2 == pearlrt::Float<24>(2.0)).getBoolean());
   }
   {
      pearlrt::Duration d(5.0);
      ASSERT_TRUE((-d == pearlrt::Duration(-5.0)).getBoolean());
      ASSERT_TRUE((d.abs() == d).getBoolean());
      ASSERT_TRUE((d.sign() == pearlrt::Fixed<1>(1)).getBoolean());
   }
   {
      pearlrt::Duration d(-5.0);
      ASSERT_TRUE((-d == pearlrt::Duration(5.0)).getBoolean());
      ASSERT_TRUE((d.abs() == pearlrt::Duration(5.0)).getBoolean());
      ASSERT_TRUE((d.sign() == pearlrt::Fixed<1>(-1)).getBoolean());
   }
   {
      pearlrt::Duration d(0.0);
      ASSERT_TRUE((-d == d).getBoolean());
      ASSERT_TRUE((d.abs() == d).getBoolean());
      ASSERT_TRUE((d.sign() == pearlrt::Fixed<1>(0)).getBoolean());
   }
   ASSERT_THROW(pearlrt::Duration(5.0) / pearlrt::Duration(),
                pearlrt::DurationDivideByZeroSignal);
}

/**
\endcond
*/
