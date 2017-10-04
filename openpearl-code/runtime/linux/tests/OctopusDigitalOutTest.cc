/*
 [A "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014     Rainer Mueller
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

\brief Test routines basic systemdation (Octopus at90 LED)

\page Testprograms

\section OctoputDigitalOutTests tests/OctopusDigitalOutTests.cc

Dation SystemDationB especially at an Octopus at90 LED-device


There are several unit tests using the google test framework.


*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "OctopusDigitalOut.h"
#include "BitString.h"
#include "Character.h"
#include <unistd.h>

using namespace std;

static pearlrt::Character<1>* idf = 0;
/**
  test with led7
*/
TEST(OctopusDigout, Led7) {
   pearlrt::OctopusDigitalOut led7('C', 6, 1);
   ASSERT_NO_THROW(led7.dationOpen(0,idf));
   pearlrt::BitString<1> on;

   for (int j = 0; j < 10; j++) {
      led7.dationWrite(&on, sizeof(on));
      sleep(1);
      on = on.bitNot();
   }

   ASSERT_NO_THROW(led7.dationClose());
}


/**
  test with all leds
*/
TEST(OctopusDigout, allLeds) {
   pearlrt::OctopusDigitalOut leds('C', 5, 6);
   ASSERT_NO_THROW(leds.dationOpen(0,idf));
   pearlrt::BitString<6> on(1);

   for (int j = 0; j < 10; j++) {
      leds.dationWrite(&on, sizeof(on));
      sleep(1);
      on = on.bitCshift(1);
   }

   ASSERT_NO_THROW(leds.dationClose());
}
