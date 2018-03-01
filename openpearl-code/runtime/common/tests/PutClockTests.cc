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

\brief CLOCK test program

\page Testprograms

\section clock_test

This program checks whether the Clock class output formatting works
properly

\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "Character.h"
#include "RefChar.h"
#include "RefCharSink.h"
#include "Duration.h"
#include "Clock.h"
#include "PutClock.h"
#include "Signals.h"

using namespace std;

TEST(PutClock, Operations) {
   pearlrt::Clock c(3600 + 2 * 60 + 3);
   pearlrt::Character<100> wrk;
   pearlrt::RefCharacter rc;
   rc.setWork(wrk);
   pearlrt::RefCharSink sink(rc);
   rc.clear();
   ASSERT_THROW(
      pearlrt::PutClock::toT(c, 7, 0, sink),
      pearlrt::ClockFormatSignal);
   rc.clear();
   ASSERT_THROW(
      pearlrt::PutClock::toT(c, 18, -1, sink),
      pearlrt::ClockFormatSignal);
   rc.clear();
   ASSERT_THROW(
      pearlrt::PutClock::toT(c, 18, 7, sink),
      pearlrt::ClockFormatSignal);
   rc.clear();
   ASSERT_NO_THROW(pearlrt::PutClock::toT(c, 15, 6, sink));
   rc.clear();
   pearlrt::PutClock::toT(c, 15, 6, sink);
   ASSERT_STREQ(rc.getCstring(), " 1:02:03.000000") ;
   rc.clear();
   pearlrt::PutClock::toT(c, 15, 0, sink);
   ASSERT_STREQ(rc.getCstring(), "        1:02:03") ;
}

/**
\endcond
*/
