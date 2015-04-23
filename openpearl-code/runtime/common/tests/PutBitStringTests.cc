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

\brief unit tests for BitString output formatting

\page Testprograms

\section PutBitString  Tests

These routines check the proper behavior of the output formating
routines for the BitString class.

\cond TREAT_EXAMPLES
*/

#include <stdio.h>
#include "gtest.h"

#include "PearlIncludes.h"

using namespace std;


TEST(PutBitString, B4) {
   pearlrt::BitString<7> x;
   pearlrt::Character<20> r1;
   pearlrt::RefCharacter rc;
   pearlrt::RefCharSink sink(rc);
   rc.setWork(r1);
   ASSERT_THROW(
      pearlrt::PutBitString<7>::toB4(x, 0, sink),
      pearlrt::BitFormatSignal);
   {
      x = (pearlrt::BitString<7>)1;
      rc.clear();
      pearlrt::PutBitString<7>::toB4(x, 2, sink);
      EXPECT_STREQ(rc.getCstring(), "02");
   }
   {
      x = (pearlrt::BitString<7>)0x7f;
      rc.clear();
      pearlrt::PutBitString<7>::toB4(x, 2, sink);
      EXPECT_STREQ(rc.getCstring(), "FE");
   }
   {
      x = (pearlrt::BitString<7>)0x7f;
      rc.clear();
      pearlrt::PutBitString<7>::toB4(x, 1, sink);
      EXPECT_STREQ(rc.getCstring(), "F");
   }
   {
      x = (pearlrt::BitString<7>)0x7f;
      rc.clear();
      pearlrt::PutBitString<7>::toB4(x, 3, sink);
      EXPECT_STREQ(rc.getCstring(), "FE0");
   }
   {
      x = (pearlrt::BitString<7>)0x7f;
      rc.clear();
      pearlrt::PutBitString<7>::toB4(x, sink);
      EXPECT_STREQ(rc.getCstring(), "FE");
   }
}


TEST(PutBitString, B1) {
   pearlrt::BitString<7> x;
   pearlrt::Character<20> r1;
   pearlrt::RefCharacter rc;
   pearlrt::RefCharSink sink(rc);
   rc.setWork(r1);
   ASSERT_THROW(
      pearlrt::PutBitString<7>::toB1(x, 0, sink),
      pearlrt::BitFormatSignal);
   {
      x = (pearlrt::BitString<7>)1;
      rc.clear();
      pearlrt::PutBitString<7>::toB1(x, 7, sink);
      EXPECT_STREQ(rc.getCstring(), "0000001");
   }
   {
      x = (pearlrt::BitString<7>)1;
      rc.clear();
      pearlrt::PutBitString<7>::toB1(x, 6, sink);
      EXPECT_STREQ(rc.getCstring(), "000000");
   }
   {
      x = (pearlrt::BitString<7>)1;
      rc.clear();
      pearlrt::PutBitString<7>::toB1(x, 8, sink);
      EXPECT_STREQ(rc.getCstring(), "00000010");
   }
   {
      x = (pearlrt::BitString<7>)0x7f;
      rc.clear();
      pearlrt::PutBitString<7>::toB1(x, 7, sink);
      EXPECT_STREQ(rc.getCstring(), "1111111");
   }
   {
      x = (pearlrt::BitString<7>)0x7f;
      rc.clear();
      pearlrt::PutBitString<7>::toB1(x, 1, sink);
      EXPECT_STREQ(rc.getCstring(), "1");
   }
   {
      x = (pearlrt::BitString<7>)0x7f;
      rc.clear();
      pearlrt::PutBitString<7>::toB1(x, sink);
      EXPECT_STREQ(rc.getCstring(), "1111111");
   }
}
/**
\endcond
*/

