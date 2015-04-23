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

\brief Bit format read test program

\page Testprograms

\section bit_read_test

This program checks whether Bit variables are read as expected.

\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "BitString.h"
#include "GetBitString.h"
#include "GetUtils.h"
#include "Signals.h"
#include "RefChar.h"
#include "RefCharSource.h"

using namespace std;

TEST(GetBitString, B1_format) {
   //                        12345678901234567890
   pearlrt::Character<20> d(" 1011               ");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::BitString<1> x;
   // test format width exceptions
   ASSERT_THROW(
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>) - 1, source),
      pearlrt::BitFormatSignal);
   // test reading with width=0 and continue with width>0
   ASSERT_NO_THROW(
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)0, source));
   {
      pearlrt::BitString<1> x1(0);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::BitString<1> x2(0x1);
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)3, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test reading of 2 not separated values
   {
      source.rewind();
      pearlrt::BitString<1> x;
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)2, source);
      pearlrt::BitString<1> x1(1);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::BitString<1> x2(0);
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)1, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test reading of 2 not separated values skipping appended digits at
   // 2nd value
   {
      source.rewind();
      pearlrt::BitString<1> x;
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)2, source);
      pearlrt::BitString<1> x1(1);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::BitString<1> x2(0);
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test illegal values
   {
      pearlrt::Character<20> d(".               ");
      rc.setWork(d);
      source.rewind();
      pearlrt::BitString<1> x;
      ASSERT_THROW(
         pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)2, source),
         pearlrt::BitValueSignal);
   }
   {
      pearlrt::Character<20> d("2               ");
      rc.setWork(d);
      source.rewind();
      pearlrt::BitString<1> x;
      ASSERT_THROW(
         pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)2, source),
         pearlrt::BitValueSignal);
   }
   // test capture data from wide fields
   {
      pearlrt::BitString<1> x2;
      //   1234567890123456789
      pearlrt::Character<20> d(" 1     01    1  ");
      rc.setWork(d);
      source.rewind();
      pearlrt::BitString<1> x;
      x2 = pearlrt::BitString<1>(1);
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
      x2 = pearlrt::BitString<1>(0);
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
      x2 = pearlrt::BitString<1>(1);
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
      x2 = pearlrt::BitString<1>(1);
      pearlrt::GetBitString<1>::fromB1(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
}


TEST(GetBitString, B4_format) {
   //                        12345678901234567890
   pearlrt::Character<20> d(" 1A34               ");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::BitString<5> x;
   // test format width exceptions
   ASSERT_THROW(
      pearlrt::GetBitString<5>::fromB4(x, (pearlrt::Fixed<15>) - 1, source),
      pearlrt::BitFormatSignal);
   // test reading with width=0 and continue with width>0
   ASSERT_NO_THROW(
      pearlrt::GetBitString<5>::fromB4(x, (pearlrt::Fixed<15>)0, source));
   {
      pearlrt::BitString<5> x1(0);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::BitString<5> x2(0x3);
      pearlrt::GetBitString<5>::fromB4(x, (pearlrt::Fixed<15>)3, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test reading of 2 not separated values
   {
      source.rewind();
      pearlrt::BitString<4> x;
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)2, source);
      pearlrt::BitString<4> x1(1);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::BitString<4> x2(0xa);
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)1, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test reading of 2 not separated values skipping appended digits at
   // 2nd value
   {
      source.rewind();
      pearlrt::BitString<4> x;
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)2, source);
      pearlrt::BitString<4> x1(1);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::BitString<4> x2(0xa);
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test illegal values
   {
      pearlrt::Character<20> d(".               ");
      rc.setWork(d);
      source.rewind();
      pearlrt::BitString<4> x;
      ASSERT_THROW(
         pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)2, source),
         pearlrt::BitValueSignal);
   }
   {
      pearlrt::Character<20> d("@               ");
      rc.setWork(d);
      source.rewind();
      pearlrt::BitString<4> x;
      ASSERT_THROW(
         pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)2, source),
         pearlrt::BitValueSignal);
   }
   // test capture data from wide fields
   {
      pearlrt::BitString<4> x2;
      //   1234567890123456789
      pearlrt::Character<20> d(" 3     45    6  ");
      rc.setWork(d);
      source.rewind();
      pearlrt::BitString<4> x;
      x2 = pearlrt::BitString<4>(3);
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
      x2 = pearlrt::BitString<4>(4);
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
      x2 = pearlrt::BitString<4>(5);
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
      x2 = pearlrt::BitString<4>(6);
      pearlrt::GetBitString<4>::fromB4(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
}

/**
\endcond
*/
