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

\brief FIXED format reading test program

\page Testprograms

\section fixed_read_test

This program checks whether the FIXED values are read as the expected.


\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "Fixed.h"
#include "compare.h"
#include "GetFixed.h"
#include "GetUtils.h"
#include "Signals.h"
#include "Character.h"
#include "RefChar.h"
#include "RefCharSource.h"

using namespace std;

TEST(GetFixed, conversions) {
   //                        123456789012345678901234567890
   pearlrt::Character<30> d(" 12        X");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Fixed<15> f15;
   // normal operations
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFixed<15>::fromF(f15, 6, 0, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(12)).getBoolean());
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFixed<15>::fromF(f15, 3, 0, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(12)).getBoolean());
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFixed<15>::fromF(f15, 2, 0, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(1)).getBoolean());
      EXPECT_NO_THROW(pearlrt::GetFixed<15>::fromF(f15, 2, 0, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(2)).getBoolean());
   }
   // illegal w,d
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 0, 0, 0, source),
         pearlrt::FixedFormatSignal);
      EXPECT_THROW(
         pearlrt::GetFixed<15>::fromF(f15, -1, 0, 0, source),
         pearlrt::FixedFormatSignal);
      EXPECT_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 6, -1, 0, source),
         pearlrt::FixedFormatSignal);
   }
   // rounding
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12345    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFixed<15>::fromF(f15, 6, 1, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(1235)).getBoolean());
   }
   // negative
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("-12345    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFixed<15>::fromF(f15, 6, 1, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(-1234)).getBoolean());
   }
   // range overflow
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 32768    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 6, 0, 0, source),
         pearlrt::FixedRangeSignal);
   }
   // range overflow
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("-32769    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 6, 0, 0, source),
         pearlrt::FixedRangeSignal);
   }
   // internal overflow
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("99999999999999999999999999999");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 30, 0, 0, source),
         pearlrt::FixedRangeSignal);
   }
   // skip wrong field
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("r27 69    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 4, 0, 0, source),
         pearlrt::FixedValueSignal);
      EXPECT_NO_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 2, 0, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(69)).getBoolean());
   }
   // skip remaining after field
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("27   69   X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 5, 0, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(27)).getBoolean());
      EXPECT_NO_THROW(
         pearlrt::GetFixed<15>::fromF(f15, 2, 0, 0, source));
      EXPECT_TRUE((f15 == pearlrt::Fixed<15>(69)).getBoolean());
   }
}

/**
\endcond
*/
