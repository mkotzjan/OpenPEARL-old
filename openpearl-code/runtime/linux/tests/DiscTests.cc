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

\brief Test routines Disc system device

\page Testprograms

\section DiscTests tests/DiscTests.cc

There are some tests to enshure proper operation of the Disc system dation
The tests  are implemented a gtest unit tests.
*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "Disc.h"
#include "Log.h"
#include "Fixed.h"
using namespace std;


#define HOME "/tmp/"    ///< the location of the files to create

/**
  path tests
*/
TEST(Disc, path) {
   pearlrt::Log::info("**** Disc path start ***");
   {
      ASSERT_NO_THROW(pearlrt::Disc disc(HOME, 10));
      ASSERT_THROW(
         pearlrt::Disc disc1("/home", 10),
         pearlrt::DationParamSignal);
      ASSERT_THROW(
         pearlrt::Disc disc2("/etc/motd", 10),
         pearlrt::DationParamSignal);
      ASSERT_THROW(
         pearlrt::Disc disc3("/xxx/", 10),
         pearlrt::DationParamSignal);
   }
}

/**
  open close
*/
TEST(Disc, openClose) {
   pearlrt::Log::info("**** Disc openClose start ***");
   //---------------------------------------------------
   // tests on non existing file
   // create+ delete file
   pearlrt::SystemDationNB* work;
   pearlrt::Disc disc(HOME, 10);
   work = disc.dationOpen("file1.tst",
                          pearlrt::Dation::ANY |
                          pearlrt::Dation::IDF |
                          pearlrt::Dation::OUT);
   work->dationClose(pearlrt::Dation::PRM);
   // file + OLD IN CAN
   // file exists now
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::IN));
   ASSERT_NO_THROW(work->dationClose(pearlrt::Dation::CAN));
   // file should be removed
   // if this test failes, the close above did not delete the file
   // no file + OLD + IN
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::IN),
      pearlrt::OpenFailedSignal);
   // no file + OLD + OUT --> IllParam
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::OUT),
      pearlrt::OpenFailedSignal);
   // no file + OLD + INOUT --> OpenFailed
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::INOUT),
      pearlrt::OpenFailedSignal);
   // no file + ANY + IN --> OpenFailed
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::ANY |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::IN),
      pearlrt::OpenFailedSignal);
   // no file + ANY + INOUT --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::ANY |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::INOUT));
   ASSERT_NO_THROW(
      work->dationClose(pearlrt::Dation::CAN));
   // no file + NEW + IN --> DationParamSignal
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::IN),
      pearlrt::DationParamSignal);
   // no file + NEW + OUT --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::OUT));
   ASSERT_NO_THROW(
      work->dationClose(pearlrt::Dation::CAN));
   // no file + NEW + INOUT --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::INOUT));
   ASSERT_NO_THROW(work->dationClose(pearlrt::Dation::CAN));
   //---------------------------------------------------
   // tests on existing file
   // create file for the following tests
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::INOUT));
   ASSERT_NO_THROW(
      work->dationClose(pearlrt::Dation::PRM));
   // file + OLD + IN --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::IN));
   ASSERT_NO_THROW(
      work->dationClose(pearlrt::Dation::PRM));
   // file + OLD + OUT --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::OUT));
   ASSERT_NO_THROW(work->dationClose(pearlrt::Dation::PRM));
   // file + OLD + INOUT --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::OLD |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::INOUT));
   ASSERT_NO_THROW(work->dationClose(pearlrt::Dation::PRM));
   // file + ANY + IN --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::ANY |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::IN));
   ASSERT_NO_THROW(work->dationClose(pearlrt::Dation::PRM));
   // file + ANY + OUT --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::ANY |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::OUT));
   ASSERT_NO_THROW(work->dationClose(pearlrt::Dation::PRM));
   // file + ANY + INOUT --> ok
   ASSERT_NO_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::ANY |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::INOUT));
   ASSERT_NO_THROW(work->dationClose(pearlrt::Dation::PRM));
   // file + NEW + IN --> DationParamSignal
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::IN),
      pearlrt::DationParamSignal);
   // file + NEW + OUT --> OpenFailed
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::OUT),
      pearlrt::OpenFailedSignal);
   // file + NEW + INOUT --> OpenFailed
   ASSERT_THROW(
      work = disc.dationOpen("file1.tst",
                             pearlrt::Dation::NEW |
                             pearlrt::Dation::IDF |
                             pearlrt::Dation::INOUT),
      pearlrt::OpenFailedSignal);
}
/**
  tests capacity tests
*/
TEST(Disc, capacity) {
   pearlrt::Log::info("**** Disc capacity start ***");
   int count = 0;
   bool noSignalGot;
   char filename[20];
   pearlrt::SystemDationNB* work[100];
   ASSERT_THROW(
      pearlrt::Disc disc(HOME, 0),
      pearlrt::DationParamSignal);
   ASSERT_THROW(
      pearlrt::Disc disc(HOME, -1),
      pearlrt::DationParamSignal);
   pearlrt::Disc disc(HOME, 10);
   noSignalGot = true;

   while (noSignalGot) {
      sprintf(filename, "testfile.%d", count);

      try {
         work[count] = disc.dationOpen(filename,
                                       pearlrt::Dation::ANY |
                                       pearlrt::Dation::IDF |
                                       pearlrt::Dation::OUT);
         count ++;
      } catch (pearlrt::OpenFailedSignal &s) {
         noSignalGot = false;
      }
   }

   EXPECT_EQ(count, 10);

   while (count > 0) {
      count --;
      work[count]->dationClose(pearlrt::Dation::CAN);
   }

   EXPECT_EQ(count, 0);
}

/**
test seek operation; just try seek(0)

This test does not verify the proper operation on the file.
*/
TEST(Disc, seek) {
   pearlrt::Log::info("**** Disc seek start ***");
   //---------------------------------------------------
   // tests on non existing file
   // create+ delete file
   pearlrt::SystemDationNB* work;
   pearlrt::Disc disc(HOME, 10);
   work = disc.dationOpen("file1.tst",
                          pearlrt::Dation::ANY |
                          pearlrt::Dation::IDF |
                          pearlrt::Dation::OUT);
   ASSERT_NO_THROW(
      work->dationSeek((pearlrt::Fixed<31>) 0,
                       pearlrt::Dation::DIRECT));
   work->dationClose(pearlrt::Dation::PRM);
}
