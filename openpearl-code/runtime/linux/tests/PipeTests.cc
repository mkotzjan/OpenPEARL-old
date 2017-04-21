/*
 [A "BSD license"]
 Copyright (c) 2014 Rainer Mueller
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

\brief Test routines Pipe system device

\page Testprograms

\section PipeTests tests/PipeTests.cc
Dation System Device Pipe
There are some tests to enshure proper operation of the Pipe system dation

*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "Pipe.h"
#include "Log.h"
#include "Fixed.h"
using namespace std;

/**
working directory for the tests
*/
#define HOME "/tmp/pipe1"

/**
  path tests
*/
TEST(Pipe, path) {
   pearlrt::Log::info("**** Pipe path start ***");
   {
      ASSERT_NO_THROW(pearlrt::Pipe pipe(HOME, 10, "ANY"));
      ASSERT_THROW(
         pearlrt::Pipe pipe1("/home/", 10, "ANY"),
         pearlrt::IllegalPathSignal);
      ASSERT_THROW(
         pearlrt::Pipe pipe2("/tmp/pipes/", 10, "ANY"),
         pearlrt::IllegalPathSignal);
      ASSERT_THROW(
         pearlrt::Pipe pipe2("/etc/motd", 10, "ANY"),
         pearlrt::IllegalPathSignal);
   }
}

/**
  param tests
*/
TEST(Pipe, param) {
   pearlrt::Log::info("**** Pipe param start ***");
   {
      ASSERT_NO_THROW(pearlrt::Pipe pipe(HOME, 10, "ANY CAN"));
   }
   // HOME should be removed now
   {
      ASSERT_NO_THROW(pearlrt::Pipe pipe(HOME, 10, "NEW CAN"));
   }
   {
      ASSERT_THROW(
         pearlrt::Pipe pipe1(HOME, 10, "OLD"),
         pearlrt::IllegalPathSignal);
   }
   {
      ASSERT_NO_THROW(pearlrt::Pipe pipe(HOME, 10, "ANY"));
   }
   // exists
   {
      ASSERT_NO_THROW(pearlrt::Pipe pipe(HOME, 10, "OLD"));
   }
   // should work twice
   {
      ASSERT_NO_THROW(pearlrt::Pipe pipe(HOME, 10, "OLD"));
   }
}

/**
  open close
*/
TEST(Pipe, openClose) {
   pearlrt::Log::info("**** Pipe openClose start ***");
   //---------------------------------------------------
   // tests on non existing file
   // create+ delete file
   pearlrt::SystemDationNB* work;
   pearlrt::Pipe pipe(HOME, 10, "OPEN1 CAN");
   // should throw due to given IDF
   ASSERT_THROW(
      pipe.dationOpen("file1.tst",
                      pearlrt::Dation::ANY |
                      pearlrt::Dation::IDF |
                      pearlrt::Dation::OUT),
      pearlrt::IllegalParamSignal);

   //
   ASSERT_NO_THROW(
      work = pipe.dationOpen(NULL,
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IN));
   ASSERT_THROW(
      work->dationClose(pearlrt::Dation::CAN),
      pearlrt::IllegalParamSignal
   );
   ASSERT_THROW(
      work = pipe.dationOpen(NULL,
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::INOUT),
      pearlrt::IllegalParamSignal
   );
   ASSERT_THROW(
      work = pipe.dationOpen(NULL,
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::OUT),
      pearlrt::IllegalParamSignal
   );
}
/**
  tests capacity tests
*/
TEST(Pipe, capacity) {
   pearlrt::Log::info("**** Pipe capacity start ***");
   int count = 0;
   bool noSignalGot;
   pearlrt::SystemDationNB* work[100];
   ASSERT_THROW(
      pearlrt::Pipe pipe(HOME, 0),
      pearlrt::IllegalParamSignal);
   ASSERT_THROW(
      pearlrt::Pipe pipe1(HOME, -1),
      pearlrt::IllegalParamSignal);
   pearlrt::Pipe pipe2(HOME, 10, "OPEN1 CAN");
   noSignalGot = true;

   while (noSignalGot) {
      try {
         work[count] = pipe2.dationOpen(NULL,
                                        pearlrt::Dation::ANY |
                                        pearlrt::Dation::OUT);
         count ++;
      } catch (pearlrt::OpenFailedSignal &s) {
         noSignalGot = false;
      }
   }

   EXPECT_EQ(count, 10);

   while (count > 0) {
      count --;
      work[count]->dationClose(pearlrt::Dation::PRM);
   }

   EXPECT_EQ(count, 0);
   pearlrt::Log::info("**** Pipe capacity end ***");
}

/**
  write/read
*/
TEST(Pipe, writeRead) {
   pearlrt::Log::info("**** Pipe write/read start ***");
   //---------------------------------------------------
   pearlrt::SystemDationNB* wrt;
   pearlrt::SystemDationNB* rd;
   pearlrt::Pipe pipe(HOME, 10, "OPEN1 CAN");
   int x;
   ASSERT_NO_THROW(
      rd = pipe.dationOpen(NULL,
                           pearlrt::Dation::OLD |
                           pearlrt::Dation::IN));
   ASSERT_NO_THROW(
      wrt = pipe.dationOpen(NULL,
                            pearlrt::Dation::OLD |
                            pearlrt::Dation::OUT));
   x = 42;
   wrt->dationWrite(&x, sizeof(x));
   x = 13;
   wrt->dationWrite(&x, sizeof(x));
   x = 0;
   wrt->dationWrite(&x, sizeof(x));
   x = 4711;
   wrt->dationWrite(&x, sizeof(x));
   x = 0;
   wrt->dationWrite(&x, sizeof(x));
   rd->dationRead(&x, sizeof(x));
   ASSERT_EQ(x, 42);
   rd->dationRead(&x, sizeof(x));
   ASSERT_EQ(x, 13);
   rd->dationRead(&x, sizeof(x));
   ASSERT_EQ(x, 0);
   rd->dationRead(&x, sizeof(x));
   ASSERT_EQ(x, 4711);
   rd->dationRead(&x, sizeof(x));
   ASSERT_EQ(x, 0);
   wrt->dationClose(pearlrt::Dation::PRM);
   rd->dationClose(pearlrt::Dation::PRM);
}
