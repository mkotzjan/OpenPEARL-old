/*
 [A "BSD license"]
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

\section OctoputDigitalInOutTests tests/OctopusDigitaInOutTests.cc



There are several unit tests using the google test framework
for the OctopusDigitalIn-device


*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "OctopusDigitalOut.h"
#include "OctopusDigitalIn.h"
#include "BitString.h"
#include "Character.h"
#include <unistd.h>

using namespace std;

static pearlrt::Character<1> * idf= 0;

/**
  test input by looping output to input

  connect each bit of PortD  with the corresponding bit of Port F

  diffent bit strings are written to PortD an read from Port F
  the read data is written to Port C (LEDs)
  This is repeated 80 times.
*/
TEST(OctopusDigInOut, all) {
   pearlrt::OctopusDigitalOut leds('C', 7, 8);
   pearlrt::OctopusDigitalOut out('D', 7, 8);
   pearlrt::OctopusDigitalIn  inp('F', 7, 8);
   ASSERT_NO_THROW(leds.dationOpen(0,idf));
   ASSERT_NO_THROW(inp.dationOpen(0,idf));
   ASSERT_NO_THROW(out.dationOpen(0,idf));
   pearlrt::BitString<8> bimu(1);
   pearlrt::BitString<8> data;

   for (int j = 0; j < 80; j++) {
      out.dationWrite(&bimu, sizeof(bimu));
      inp.dationRead(&data, sizeof(data));
      leds.dationWrite(&data, sizeof(bimu));
      ASSERT_TRUE((bimu == data).getBoolean());
      bimu = bimu.bitCshift(1);
   }

   ASSERT_NO_THROW(leds.dationClose());
   ASSERT_NO_THROW(inp.dationClose());
   ASSERT_NO_THROW(out.dationClose());
}


