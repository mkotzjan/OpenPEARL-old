/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

Module tests for float formatting using the F(w,d)- and E(w,d,s)-format

*/

#include <stdio.h>
#include <stdlib.h>
#include "gtest.h"

#include "PearlIncludes.h"
#include "RefCharSink.h"
#include "PutFloat.h"
#include "Log.h"

using namespace std;

/**
unit tests for float output formatting
*/
TEST(PutFloat, simpleFloat) {
   pearlrt::RefCharacter rc;
   pearlrt::Character<30> c;
   pearlrt::Character<30> d(10, (char*)"1234567890");
   pearlrt::Character<4> d1(4, (char*)"1234"), d2;
   pearlrt::RefCharSink sink(rc);
   pearlrt::Float<53> x(-1234);
   rc.setWork(c);
   {
      rc.clear();
      x = (pearlrt::Float<53>) - 1234;
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toF(x,
                                    (pearlrt::Fixed<31>)0,
                                    (pearlrt::Fixed<31>)0,
                                    sink),
         pearlrt::FixedFormatSignal);
      rc.clear();
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toF(x,
                                    (pearlrt::Fixed<31>) - 1,
                                    (pearlrt::Fixed<31>)0,
                                    sink),
         pearlrt::FixedFormatSignal);
      rc.clear();
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toF(x,
                                    (pearlrt::Fixed<31>)5,
                                    (pearlrt::Fixed<31>)1,
                                    sink),
         pearlrt::FixedValueSignal);
   }
   {
      rc.clear();
      x = (pearlrt::Float<53>) - 1234;
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toF(x,
                                 (pearlrt::Fixed<31>)5,
                                 (pearlrt::Fixed<31>)5,
                                 sink),
         pearlrt::FixedFormatSignal);
   }
   {
      pearlrt::Character<5> d1(5, (char*)"-1234"), d2;
      rc.clear();
      x = (pearlrt::Float<53>) - 1234;
      pearlrt::PutFloat<53>::toF(x,
                                 (pearlrt::Fixed<31>)5,
                                 (pearlrt::Fixed<31>)0,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 5);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
      pearlrt::Character<5> d1(5, (char*)"   -1"), d2;
      rc.clear();
      x = (pearlrt::Float<53>) - 1;
      pearlrt::PutFloat<53>::toF(x,
                                 (pearlrt::Fixed<31>)5,
                                 (pearlrt::Fixed<31>)0,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 5);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
      pearlrt::Character<5> d1(5, (char*)"12345"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)12345;
      pearlrt::PutFloat<53>::toF(x,
                                 (pearlrt::Fixed<31>)5,
                                 (pearlrt::Fixed<31>)0,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 5);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
      pearlrt::Character<5> d1(5, (char*)" 12.0"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)12;
      pearlrt::PutFloat<53>::toF(x,
                                 (pearlrt::Fixed<31>)5,
                                 (pearlrt::Fixed<31>)1,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 5);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
}


TEST(PutFloat, exp2Format) {
   pearlrt::RefCharacter rc;
   pearlrt::Character<30> c;
   pearlrt::Character<30> d(10, (char*)"1234567890");
   pearlrt::Character<4> d1(4, (char*)"1234"), d2;
   pearlrt::RefCharSink sink(rc);
   pearlrt::Float<53> x(-1234);
   rc.setWork(c);
   {
      rc.clear();
      x = (pearlrt::Float<53>) - 1234;
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toE(x,
                                    (pearlrt::Fixed<31>)0,
                                    (pearlrt::Fixed<31>)4,
                                    (pearlrt::Fixed<31>)5,
                                    (pearlrt::Fixed<31>)2,
                                    sink),
         pearlrt::ExpFormatSignal);
      rc.clear();
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toE(x,
                                    (pearlrt::Fixed<31>)-1,
                                    (pearlrt::Fixed<31>)4,
                                    (pearlrt::Fixed<31>)5,
                                    (pearlrt::Fixed<31>)2,
                                    sink),
         pearlrt::ExpFormatSignal);
      rc.clear();
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toE(x,
                                    (pearlrt::Fixed<31>)13,
                                    (pearlrt::Fixed<31>)6,
                                    (pearlrt::Fixed<31>)6,
                                    (pearlrt::Fixed<31>)2,
                                    sink),
         pearlrt::ExpFormatSignal);
      rc.clear();
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toE(x,
                                    (pearlrt::Fixed<31>)13,
                                    (pearlrt::Fixed<31>)7,
                                    (pearlrt::Fixed<31>)8,
                                    (pearlrt::Fixed<31>)2,
                                    sink),
         pearlrt::ExpFormatSignal);
      rc.clear();
      ASSERT_THROW(
         pearlrt::PutFloat<53>::toE(x,
                                    (pearlrt::Fixed<31>)13,
                                    (pearlrt::Fixed<31>)6,
                                    (pearlrt::Fixed<31>)0,
                                    (pearlrt::Fixed<31>)2,
                                    sink),
         pearlrt::ExpFormatSignal);
      rc.clear();
   }
   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"     0.0E+00"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)0.0;
      pearlrt::PutFloat<53>::toE(x,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 12);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"    12.3E+02"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)1.2345E3;
      pearlrt::PutFloat<53>::toE(x,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 12);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"   -12.3E+02"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)-1.2345E3;
      pearlrt::PutFloat<53>::toE(x,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 12);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"    12.3E-04"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)1.2345e-3;
      pearlrt::PutFloat<53>::toE(x,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 12);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"   -12.3E-04"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)-1.2345e-3;
      pearlrt::PutFloat<53>::toE(x,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 12);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"   -12.3E-04"), d2;
      rc.clear();
      pearlrt::Float<53> nan;
      EXPECT_THROW(
      pearlrt::PutFloat<53>::toE(nan,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink),
         pearlrt::FloatIsNaNSignal);
   }

   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"     0.0E+00"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)-1.2345e+103;
      EXPECT_THROW(
      pearlrt::PutFloat<53>::toE(x,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink),
         pearlrt::ExpValueSignal);
   }
   {
                                       //   12345678901234567
      pearlrt::Character<12> d1(12, (char*)"     0.0E+00"), d2;
      rc.clear();
      x = (pearlrt::Float<53>)-1.2345e-103;
      pearlrt::PutFloat<53>::toE(x,
                                 (pearlrt::Fixed<31>)12,
                                 (pearlrt::Fixed<31>)1,
                                 (pearlrt::Fixed<31>)3,
                                 (pearlrt::Fixed<31>)2,
                                 sink);
      ASSERT_EQ(rc.getCurrent(), 12);
      rc.store(d2);
      EXPECT_TRUE((d1 == d2).getBoolean());
   }
}

