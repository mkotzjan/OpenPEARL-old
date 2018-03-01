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

\brief test of size of data structures

\page Testprograms

\section sizetests Size Tests

This test checks if the data elements use the expected amount of memory.
This test is only done for types which may be used inside read/write
statements, where the pure data content must be accessed.

Other language elements are listed for information purpose.

Expected Result:
\verbatim
 --- I/O data types --
size of Fixed<7>                       =    1:  ok
size of Fixed<7> Array 3x4             =   12:  ok
size of Fixed<15>                      =    2:  ok
size of Fixed<15> Array 3x4            =   24:  ok
size of Fixed<31>                      =    4:  ok
size of Fixed<31> Array 3x4            =   48:  ok
size of Fixed<63>                      =    8:  ok
size of Fixed<63> Array 3x4            =   96:  ok
size of Duration                       =    8:  ok
size of Clock                          =    8:  ok
size of Character(30)                  =   30:  ok
size of Character(30) Array[5]         =  150:  ok

 --- Informative Data --
sizeof(CSema) = 20
sizeof(Mutex) = 28
sizeof(Semaphore) = 8
sizeof(Task TCB) = 196
\endverbatim

\cond TREAT_EXAMPLES
*/
#include "gtest.h"
#include <stdio.h>
#include <stdlib.h>

#include "PearlIncludes.h"

SPCTASK(t1);

DCLTASK(t1, pearlrt::Prio(1), pearlrt::BitString<1>(0)) {
}


static int print_ok_nok(const char * test, int current, int expected) {
   int ok = 1;

   printf("size of %-30.30s = %4d", test, current);

   if (current == expected) {
      printf(":  ok\n");
   } else {
      printf(": fail (%d instead of %d)\n", current, expected);
      ok = 0;
   }

   return ok;
}


TEST(SizeTests, Test) {
   int siz;
   pearlrt::Fixed<7> f7, f734[3][4];
   pearlrt::Fixed<15> f15, f1534[3][4];
   pearlrt::Fixed<31> f31, f3134[3][4];
   pearlrt::Fixed<63> f63, f6334[3][4];
   pearlrt::Duration d;
   pearlrt::Clock c;
   pearlrt::Character<30> c30, c31[5];

   siz = sizeof(f7);
   print_ok_nok("Fixed7", siz, 1);
   EXPECT_EQ(sizeof(f7), 1);
   siz = sizeof(f734);
   print_ok_nok("Fixed7 Array 3x4", siz, 12);
   EXPECT_EQ(sizeof(f734), 12);
   print_ok_nok("Fixed15", sizeof(f15), 2);
   EXPECT_EQ(sizeof(f15), 2);
   print_ok_nok("Fixed15 Array 3x4", sizeof(f1534), 24);
   EXPECT_EQ(sizeof(f1534), 24);
   print_ok_nok("Fixed31", sizeof(f31), 4);
   EXPECT_EQ(sizeof(f31), 4);
   print_ok_nok("Fixed31 Array 3x4", sizeof(f3134), 48);
   EXPECT_EQ(sizeof(f3134), 48);
   print_ok_nok("Fixed63", sizeof(f63), 8);
   EXPECT_EQ(sizeof(f63), 8);
   print_ok_nok("Fixed63 Array 3x4", sizeof(f6334), 96);
   EXPECT_EQ(sizeof(f6334), 96);
   print_ok_nok("Duration", sizeof(d), 8);
   EXPECT_EQ(sizeof(d), 8);
   print_ok_nok("Clock", sizeof(c), 8);
   EXPECT_EQ(sizeof(c), 8);
   print_ok_nok("Character(30)", sizeof(c30), 30);
   EXPECT_EQ(sizeof(c30), 30);
   print_ok_nok("Character(30) Array[5]", sizeof(c31), 150);
   EXPECT_EQ(sizeof(c31), 150);

   printf("\n --- Informative Data --\n");
   {
      pearlrt::CSema s;
      printf("sizeof(CSema) = %d\n", (int)sizeof(s));
   }
   {
      pearlrt::Mutex s;
      printf("sizeof(Mutex) = %d\n", (int)sizeof(s));
   }
   {
      pearlrt::Semaphore s(0, "");
      printf("sizeof(PEARL Semaphore) = %d\n", (int)sizeof(s));
   }
   {
      printf("sizeof(PEARL Task TCB) = %d\n", (int)sizeof(t1));
   }
}

/**
\endcond
*/

