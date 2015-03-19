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

Module tests for fixed formatting using the A(w)- and A-format

*/

#include <stdio.h>
#include <stdlib.h>
#include "gtest.h"

#include "PearlIncludes.h"
#include "Character.h"
#include "RefCharSink.h"
#include "PutCharacter.h"

using namespace std;

/**
unit tests for character output formatting
*/
TEST(PutCharacter, simpleCharacter) {
   pearlrt::RefCharacter rc;
   pearlrt::Character<30> c;
   pearlrt::Character<30> d("1234567890");
   pearlrt::Character<4> d1("1234"), d2;
   pearlrt::RefCharSink sink(rc);
   pearlrt::Character<5> x("PEARL");
   rc.setWork(c);
   {
      rc.clear();
      ASSERT_THROW(
         pearlrt::PutCharacter<5>::toA(x,
                                       (pearlrt::Fixed<15>) - 1,
                                       sink),
         pearlrt::CharacterFormatSignal);
      rc.clear();
      pearlrt::PutCharacter<5>::toA(x,
                                    (pearlrt::Fixed<15>)0 ,
                                    sink);
      EXPECT_EQ(rc.getCurrent(), 0);
      rc.clear();
      pearlrt::PutCharacter<5>::toA(x,
                                    (pearlrt::Fixed<15>)2 ,
                                    sink);
      EXPECT_EQ(rc.getCurrent(), 2);
      EXPECT_STREQ(rc.getCstring(), "PE");
      rc.clear();
      pearlrt::PutCharacter<5>::toA(x,
                                    (pearlrt::Fixed<15>)5 ,
                                    sink);
      EXPECT_EQ(rc.getCurrent(), 5);
      EXPECT_STREQ(rc.getCstring(), "PEARL");
      rc.clear();
      pearlrt::PutCharacter<5>::toA(x,
                                    (pearlrt::Fixed<15>)7 ,
                                    sink);
      EXPECT_EQ(rc.getCurrent(), 7);
      EXPECT_STREQ(rc.getCstring(), "PEARL  ");
      rc.clear();
      pearlrt::PutCharacter<5>::toA(x,
                                    sink);
      EXPECT_EQ(rc.getCurrent(), 5);
      EXPECT_STREQ(rc.getCstring(), "PEARL");
   }
}


