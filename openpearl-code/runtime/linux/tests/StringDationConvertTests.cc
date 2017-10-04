/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

\brief Test routines CONVERT support

\page Testprograms

\section StringDationConvertTests tests/StringDationConvertTests.cc

Test whether the API of StringDationConvert works.
Implemented as gtest unit test

*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "StringDationConvert.h"
#include "Fixed.h"
#include "Character.h"
#include "Log.h"
#include "RefChar.h"
#include "myTests.h"

using namespace std;


/**
  read/write some data to a StringDationConvert and verify operation
  with A and F -format and POS,SOP,X
*/
TEST(Convert, simplePut) {
   pearlrt::Log::info("*** Convert: simplePut start ***");
   {
       pearlrt::Character<8> text("PEARLxxx");
       pearlrt::Fixed<31>  x(42);
       pearlrt::Character<15> result("PEARLxxx 42    ");
       pearlrt::Character<15> string;
       pearlrt::RefCharacter rc(string);
       pearlrt::StringDationConvert stringDation(&rc, true);
       ASSERT_NO_THROW(stringDation.toA(text));
       ASSERT_NO_THROW(stringDation.toF(x, (pearlrt::Fixed<31>)3));
       EXPECT_TRUE((result==string).getBoolean());
//printf("string >%15s< result >%15s<\n", string.data, result.data);
   }
   {
       // test clear of output string
       pearlrt::Character<8> text("PEARLxxx");
       pearlrt::Fixed<31>  x(42);
       pearlrt::Character<15> result(" ");
       pearlrt::Character<15> string("123456789012345");
       pearlrt::RefCharacter rc(string);
       pearlrt::StringDationConvert stringDation(&rc, true);
//printf("string >%15s< result >%15s<\n", string.data, result.data);
       EXPECT_TRUE((result==string).getBoolean());
   }

   {
       // test  output string overflow
       pearlrt::Character<8> text("PEARLxxx");
       pearlrt::Fixed<31>  x(42);
       pearlrt::Character<15> result("PEARLxxx ");
       pearlrt::Character<15> string("123456789012345");
       pearlrt::RefCharacter rc(string);
       pearlrt::StringDationConvert stringDation(&rc, true);
       ASSERT_NO_THROW(stringDation.toA(text));
       ASSERT_THROW(stringDation.toA(text),pearlrt::CharacterTooLongSignal);
       EXPECT_TRUE((result==string).getBoolean());
   }

}

/**
check positioning and read position
*/
TEST(Convert, putPosSop) {
   pearlrt::Log::info("*** Convert: putPosSOP start ***");
   {
       pearlrt::Character<8> text("PEARLxxx");
       pearlrt::Fixed<31>  x(42);
       pearlrt::Fixed<31> p;
       pearlrt::Character<15> result(" 42RLxxx       ");
       pearlrt::Character<15> result1(" 42R xxx       ");
       pearlrt::Character<15> string;
       pearlrt::RefCharacter rc(string);
       pearlrt::StringDationConvert stringDation(&rc, true);
       stringDation.sop(p);
       EXPECT_TRUE((p==pearlrt::Fixed<31>(1)).getBoolean());
       ASSERT_NO_THROW(stringDation.toA(text));
       stringDation.sop(p);
       EXPECT_TRUE((p==pearlrt::Fixed<31>(9)).getBoolean());

       EXPECT_THROW(stringDation.pos(pearlrt::Fixed<31>(0)),
                    pearlrt::DationIndexBoundSignal); 
       EXPECT_NO_THROW(stringDation.pos(pearlrt::Fixed<31>(1)));
       EXPECT_NO_THROW(stringDation.pos(pearlrt::Fixed<31>(15)));
       EXPECT_THROW(stringDation.pos(pearlrt::Fixed<31>(16)),
                    pearlrt::DationIndexBoundSignal); 

       stringDation.sop(p);
       EXPECT_TRUE((p==pearlrt::Fixed<31>(15)).getBoolean());

       stringDation.pos(pearlrt::Fixed<31>(1));
       stringDation.toF(x,3);
//printf("string >%15s< result >%15s<\n", string.data, result.data);
       EXPECT_TRUE((result==string).getBoolean());
       stringDation.adv(pearlrt::Fixed<31>(1));
       stringDation.toX(1);
//printf("string >%15s< result1 >%15s<\n", string.data, result1.data);
   }
}

/**
test CONVERT FROM 
*/
TEST(Convert, simpleGet) {
   pearlrt::Log::info("*** Convert: simpleGut start ***");
   {
       pearlrt::Character<8> text;
       pearlrt::Fixed<31>  x;
       pearlrt::Character<15> string("PEARLxxx 42    ");
       pearlrt::Character<8> result("PEARLxxx");
       pearlrt::RefCharacter rc(string);
       pearlrt::StringDationConvert stringDation(&rc, false);
       ASSERT_NO_THROW(stringDation.fromA(text));
       EXPECT_TRUE((result==text).getBoolean());
//printf("text >%8s< result >%8s<\n", text.data, result.data);
       ASSERT_NO_THROW(stringDation.fromF(x, (pearlrt::Fixed<31>)3));
       EXPECT_TRUE((x==pearlrt::Fixed<31>(42)).getBoolean());
   }
   {
       // test end of input string
       pearlrt::Character<8> text;
       pearlrt::Fixed<31>  x(42);
       pearlrt::Character<8> result("12345678");
       pearlrt::Character<15> string("123456789012345");
       pearlrt::RefCharacter rc(string);
       pearlrt::StringDationConvert stringDation(&rc, false);
       ASSERT_NO_THROW(stringDation.fromA(text));
       EXPECT_TRUE((result==text).getBoolean());

       ASSERT_NO_THROW(stringDation.fromF(x,3));
       EXPECT_TRUE((x==pearlrt::Fixed<31>(901)).getBoolean());
       
       ASSERT_NO_THROW(stringDation.fromF(x,3));
       EXPECT_TRUE((x==pearlrt::Fixed<31>(234)).getBoolean());
       
       ASSERT_THROW(stringDation.fromF(x,3),
                    pearlrt::CharacterTooLongSignal);
   }

}

/**
test positioning and read poosition during CONVERT FROM
*/
TEST(Convert, getPosSop) {
   pearlrt::Log::info("*** Convert: getPosSOP start ***");
   {
       pearlrt::Character<8> text("PEARLxxx");
       pearlrt::Character<8> result("PEA");
       pearlrt::Fixed<31>  x(42);
       pearlrt::Fixed<31> p;
       pearlrt::Character<15> string("PEARLxxx 42    ");
       pearlrt::RefCharacter rc(string);
       pearlrt::StringDationConvert stringDation(&rc, false);
       stringDation.sop(p);
       EXPECT_TRUE((p==pearlrt::Fixed<31>(1)).getBoolean());

       EXPECT_THROW(stringDation.pos(pearlrt::Fixed<31>(0)),
                    pearlrt::DationIndexBoundSignal); 
       EXPECT_NO_THROW(stringDation.pos(pearlrt::Fixed<31>(1)));
       EXPECT_NO_THROW(stringDation.pos(pearlrt::Fixed<31>(15)));
       EXPECT_THROW(stringDation.pos(pearlrt::Fixed<31>(16)),
                    pearlrt::DationIndexBoundSignal); 

       stringDation.sop(p);
       EXPECT_TRUE((p==pearlrt::Fixed<31>(15)).getBoolean());

       stringDation.pos(pearlrt::Fixed<31>(1));
       stringDation.fromA(text,3);

       EXPECT_TRUE((result==text).getBoolean());
       stringDation.adv(pearlrt::Fixed<31>(5));
       stringDation.fromF(x,2);
       EXPECT_TRUE((x==pearlrt::Fixed<31>(4)).getBoolean());

       stringDation.sop(p);
       EXPECT_TRUE((p==pearlrt::Fixed<31>(11)).getBoolean());

       stringDation.fromF(x,1);
       EXPECT_TRUE((x==pearlrt::Fixed<31>(2)).getBoolean());

       EXPECT_NO_THROW(stringDation.fromX(3));
       EXPECT_THROW(stringDation.fromX(1),pearlrt::CharacterTooLongSignal);
   }
}

