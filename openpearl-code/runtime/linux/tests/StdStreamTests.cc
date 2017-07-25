/*
 [A "BSD license"]
 Copyright (c) 2014-2016 Rainer Mueller
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

\brief Test routines StdStream system device

\page Testprograms

\section StdStreamTests tests/StdStreamTests.cc
There are some tests to enshure proper operation of the StdStream system dation

*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "StdIn.h"
#include "StdOut.h"
#include "StdError.h"
#include "Log.h"
#include "Fixed.h"
#include "Character.h"
#include "Dation.h"
#include "DationPG.h"
#include "DationDim2.h"

using namespace std;

/**
Test ctor parameters for StdStream device
*/
TEST(StdStream, path) {
   pearlrt::Log::info("**** StdStream stream select tests ***");
   {
      ASSERT_NO_THROW(pearlrt::StdIn stdIn);
      ASSERT_NO_THROW(pearlrt::StdOut stdOut);
      ASSERT_NO_THROW(pearlrt::StdError stdErr);

   }
}

/**
  open close
*/
TEST(StdStream, openClose) {
   pearlrt::Log::info("**** StdStream openClose start ***");
   //---------------------------------------------------
   // tests legal operations
   pearlrt::SystemDationNB* work;
   pearlrt::StdIn stdIn;
   work = stdIn.dationOpen(NULL,
                          pearlrt::Dation::OLD |
                          pearlrt::Dation::IN);
   work->dationClose(pearlrt::Dation::PRM);

   pearlrt::StdOut stdOut;
   work = stdOut.dationOpen(NULL,
                          pearlrt::Dation::OLD |
                          pearlrt::Dation::OUT);
   work->dationClose(pearlrt::Dation::PRM);

   pearlrt::StdError stdErr;
   work = stdErr.dationOpen(NULL,
                          pearlrt::Dation::OLD |
                          pearlrt::Dation::OUT);
   work->dationClose(pearlrt::Dation::PRM);

   ASSERT_THROW(
      work = stdIn.dationOpen("x.x",
                          pearlrt::Dation::OLD |
                          pearlrt::Dation::IDF |
                          pearlrt::Dation::IN),
      pearlrt::DationParamSignal);
}

/**
UserDations tests on stdOut
*/
TEST(StdStream, userDationStdOut) {
   pearlrt::Log::info("**** StdStream UserDations  start (StdOut) ***");

   pearlrt::StdOut stdOut;
   pearlrt::DationDim2 dim(80);
   pearlrt::Character<1> fn;
   ASSERT_THROW(
      pearlrt::DationPG console(&stdOut,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim),
     pearlrt::InternalDationSignal);

   ASSERT_THROW(
      pearlrt::DationPG console(&stdOut,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::DIRECT |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim),
     pearlrt::InternalDationSignal);

   ASSERT_THROW(
      pearlrt::DationPG console(&stdOut,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORBACK |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim),
     pearlrt::InternalDationSignal);

   pearlrt::DationPG console(&stdOut,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);

   ASSERT_THROW(
      console.dationOpen(
         pearlrt::Dation::IDF ,
         &fn, (pearlrt::Fixed<15>*)0),
     pearlrt::DationParamSignal);
   pearlrt::Character<8> text("PEARL");

   ASSERT_THROW(
      console.dationOpen(
         pearlrt::Dation::OLD | pearlrt::Dation::IDF,
         &fn, (pearlrt::Fixed<15>*)0),
     pearlrt::DationParamSignal);

   ASSERT_THROW(
      console.dationOpen(
         pearlrt::Dation::CAN,
         (pearlrt::Character<1>*) 0, //&fn,
         (pearlrt::Fixed<15>*)0),
     pearlrt::DationParamSignal);
}

/**
UserDations tests on stdIn
*/
TEST(StdStream, userDationStdIn) {
   pearlrt::Log::info("**** StdStream UserDations  start (StdIn) ***");

   pearlrt::StdIn stdIn;
   pearlrt::DationDim2 dim(80);
   pearlrt::Character<1> fn;
   ASSERT_THROW(
      pearlrt::DationPG console(&stdIn,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim),
     pearlrt::InternalDationSignal);

   ASSERT_THROW(
      pearlrt::DationPG console(&stdIn,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::DIRECT |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim),
     pearlrt::InternalDationSignal);

   ASSERT_THROW(
      pearlrt::DationPG console(&stdIn,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORBACK |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim),
     pearlrt::InternalDationSignal);

   pearlrt::DationPG console(&stdIn,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);

   ASSERT_THROW(
      console.dationOpen(
         pearlrt::Dation::IDF ,
         &fn, (pearlrt::Fixed<15>*)0),
     pearlrt::DationParamSignal);
   pearlrt::Character<8> text("PEARL");

   ASSERT_THROW(
      console.dationOpen(
         pearlrt::Dation::OLD | pearlrt::Dation::IDF,
         &fn, (pearlrt::Fixed<15>*)0),
     pearlrt::DationParamSignal);

   ASSERT_THROW(
      console.dationOpen(
         pearlrt::Dation::CAN,
         (pearlrt::Character<1>*) 0, //&fn,
         (pearlrt::Fixed<15>*)0),
     pearlrt::DationParamSignal);
}
/**
Put/Get as  visibility test only
*/
TEST(StdStream, putGet) {
   pearlrt::Log::info("**** StdStream put get  start ***");
   pearlrt::Character<37> prompt1("give String A(8)-format and enter    ");
   pearlrt::Character<37> prompt2("and give Fixed F(3)-format and enter ");
   pearlrt::StdOut stdOut;
   pearlrt::DationDim2 dim(80);
   pearlrt::Character<1> fn;
   pearlrt::DationPG console(&stdOut,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   ASSERT_NO_THROW(
      console.dationOpen(
         0, //pearlrt::Dation::ANY ,
         (pearlrt::Character<1>*) 0, //&fn,
         (pearlrt::Fixed<15>*)0));
   pearlrt::Character<8> text("PEARL");
   pearlrt::Fixed<31>  x(42);
   console.toA(text);
   console.toSkip(1);
   console.toF(x, (pearlrt::Fixed<31>)5);
   console.toSkip(1);
   console.toF(x, (pearlrt::Fixed<31>)5, (pearlrt::Fixed<31>)1);
   console.toSkip(1);
   console.toF(x, (pearlrt::Fixed<31>)5, (pearlrt::Fixed<31>)2);
   console.toSkip(1);

   pearlrt::StdIn stdIn;
   pearlrt::DationPG keyboard(&stdIn,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   ASSERT_NO_THROW(
      keyboard.dationOpen(
         0, //pearlrt::Dation::ANY ,
         (pearlrt::Character<1>*) 0, //&fn,
         (pearlrt::Fixed<15>*)0));
   console.toA(prompt1);
   console.toSkip(1);
   console.toA(prompt2);
   console.toSkip(1);
   keyboard.fromA(text);
   keyboard.fromSkip(1);
   keyboard.fromF(x, (pearlrt::Fixed<31>)3);
   keyboard.fromSkip(1);
   keyboard.dationClose(0, (pearlrt::Fixed<15>*)0);

   pearlrt::Character<8> result("Result: ");
   console.toA(result);
   console.toA(text);
   console.toX(5);
   console.toF(x, (pearlrt::Fixed<31>)3);
   console.toSkip(1);
   console.dationClose(0, (pearlrt::Fixed<15>*)0);
}
