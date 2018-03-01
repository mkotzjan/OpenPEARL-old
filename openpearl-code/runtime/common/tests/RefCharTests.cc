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


#include <stdio.h>
#include <stdlib.h>
#include "gtest.h"

#include "PearlIncludes.h"

using namespace std;

TEST(RefCharTests, CTors) {
   {
      pearlrt::Character<10> c;
      pearlrt::RefCharacter rc(c);
      EXPECT_EQ(rc.getMax(), 10);
      EXPECT_EQ(rc.getCurrent(), 0);
   }
   {
      pearlrt::RefCharacter rc;
      pearlrt::Character<10> c;
      rc.setWork(c);
      EXPECT_EQ(rc.getMax(), 10);
      EXPECT_EQ(rc.getCurrent(), 0);
   }
}
TEST(RefCharTests, Operations) {
   {
      pearlrt::RefCharacter rc;
      pearlrt::Character<10> c;
      rc.setWork(c);
      EXPECT_EQ(rc.getMax(), 10);
      EXPECT_EQ(rc.getCurrent(), 0);
      rc.clear();
      EXPECT_EQ(rc.getMax(), 10);
      EXPECT_EQ(rc.getCurrent(), 0);
      pearlrt::Character<3> h(3, (char*)"Hal");
      rc.add(h);
      pearlrt::Character<2> h1(2, (char*)"lo ");
      pearlrt::Character<5> hallo(5, (char*)"Hallo");
      rc.add(h1);
      rc.fill();
      EXPECT_TRUE((c == hallo).getBoolean());
      rc.add(h);
      rc.add(h1);
      {
         pearlrt::Character<1> sc;
         ASSERT_THROW(rc.add(sc), pearlrt::CharacterTooLongSignal);
      }
      ASSERT_THROW(rc.getCstring(), pearlrt::CharacterTooLongSignal);
   }
   {
      pearlrt::Character<6> hello(6, (char*)"hello ");
      pearlrt::Character<5> world(5, (char*)"world");
      pearlrt::Character<11> helloworld(11, (char*)"hello world");
      pearlrt::Character<14> hw; // will be extended with spaces
      pearlrt::Character<100> content;
      pearlrt::RefCharacter rc;
      rc.setWork(content);
      rc.clear();
      rc.add(hello);
      rc.add(world);
      rc.store(hw);
      EXPECT_TRUE((hw == helloworld).getBoolean());
   }
   {
//        printf("\n\n test overflow on store of ref char\n");
//        printf("     DCL hw      CHAR(14); \n");
//        printf("     DCL content CHAR(100); \n     DCL rc REF CHAR();\n");
//        printf("     rc := content; \n ");
//        printf("     CONT rc := 'hello ' CAT 'world';\n");
//        printf("     CONT rc := (CONT rc) CAT 'hello ';\n");
//        printf("     hw := CONT rc; \n");
//        printf(" ... ");
      pearlrt::Character<6> hello(6, (char*)"hello ");
      pearlrt::Character<5> world(5, (char*)"world");
      pearlrt::Character<11> helloworld(11, (char*)"hello world");
      pearlrt::Character<14> hw; // will be extended with spaces
      pearlrt::Character<100> content;
      pearlrt::RefCharacter rc;
      rc.setWork(content);
      rc.clear();
      rc.add(hello);
      rc.add(world);
      rc.add(hello);
      ASSERT_THROW(
         rc.store(hw), pearlrt::CharacterTooLongSignal);
   }
   {
      // getCharAt
      pearlrt::Character<6> hello(6, (char*)"hello ");
      pearlrt::RefCharacter rc;
      rc.setWork(hello);
      ASSERT_THROW(rc.getCharAt(-1), pearlrt::CharacterIndexOutOfRangeSignal);
      ASSERT_THROW(rc.getCharAt(6), pearlrt::CharacterIndexOutOfRangeSignal);
      ASSERT_NO_THROW(rc.getCharAt(0));
      ASSERT_NO_THROW(rc.getCharAt(5));
      ASSERT_EQ(rc.getCharAt(0), 'h');
      ASSERT_EQ(rc.getCharAt(1), 'e');
      ASSERT_EQ(rc.getCharAt(2), 'l');
      ASSERT_EQ(rc.getCharAt(3), 'l');
      ASSERT_EQ(rc.getCharAt(4), 'o');
      ASSERT_EQ(rc.getCharAt(5), ' ');
   }
}


