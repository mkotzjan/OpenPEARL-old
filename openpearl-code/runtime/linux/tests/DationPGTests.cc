/*
 [The "BSD license"]
 Copyright (c) 2014-2014 Rainer Mueller
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

\brief Test routines generic non-basic systemdations (PG-Dation)

\page Testprograms

\section DationPG


*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "SystemDationNB.h"
#include "Dation.h"
#include "Disc.h"
#include "DationDim.h"
#include "DationDim1.h"
#include "DationDim2.h"
#include "DationDim3.h"
#include "DationPG.h"
#include "DationRW.h"
#include "Device.h"
#include "Fixed.h"
#include "Character.h"
#include "Log.h"
#include "myTests.h"

using namespace std;

static pearlrt::Disc disc("/tmp/", 5);
static pearlrt::Device* _disc = &disc;


/**
  Writing simple text to  testfile and verify by binary read with DationRW
*/

TEST(DationPG, simple_put) {
   pearlrt::Log::info("*** DationPG: simple_put start ***");
   pearlrt::Character<9> filename("put_1.txt");
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(80);
   /* -------------------------------------------- */
   pearlrt::Log::info("      DationPG: simple_out start   ");
   pearlrt::DationPG logbuch(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY ,
         & filename,
         NULL));
   pearlrt::Character<8> text("PEARL");
   pearlrt::Fixed<31>  x(42);
   logbuch.toA(text);
   logbuch.toSkip(1);
   logbuch.toF(x, (pearlrt::Fixed<15>)3);
   logbuch.dationClose(pearlrt::Dation::PRM);
   /* read binary and compare */
   pearlrt::Character<1> data[12];
   pearlrt::Character<1> rdata[12];
   data[0] = pearlrt::toChar('P');
   data[1] = pearlrt::toChar('E');
   data[2] = pearlrt::toChar('A');
   data[3] = pearlrt::toChar('R');
   data[4] = pearlrt::toChar('L');
   data[5] = pearlrt::toChar(' ');
   data[6] = pearlrt::toChar(' ');
   data[7] = pearlrt::toChar(' ');
   data[8] = pearlrt::toChar('\n');
   data[9] = pearlrt::toChar(' ');
   data[10] = pearlrt::toChar('4');
   data[11] = pearlrt::toChar('2');
   pearlrt::DationRW log_bin(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             (pearlrt::Fixed<15>)1);
   ASSERT_NO_THROW(
      log_bin.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::OLD ,
         & filename,
         NULL));
   log_bin.dationRead(&rdata, sizeof(rdata));
   log_bin.dationClose();
   ASSERT_TRUE(
      ARRAY_EQUAL(12, data, rdata));
}

/**
  Writing simple using binary write to testfile
  and verify by GET read with DationPG
*/

TEST(DationPG, simple_get) {
   pearlrt::Log::info("*** DationPG: simple_get start ***");
   pearlrt::Character<9> filename("get_1.txt");
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(80);
   /* -------------------------------------------- */
   pearlrt::Log::info("      DationPG: simple_get start   ");
   /* write binary */
   pearlrt::Character<1> data[12];
   data[0] = pearlrt::toChar('P');
   data[1] = pearlrt::toChar('E');
   data[2] = pearlrt::toChar('A');
   data[3] = pearlrt::toChar('R');
   data[4] = pearlrt::toChar('L');
   data[5] = pearlrt::toChar(' ');
   data[6] = pearlrt::toChar(' ');
   data[7] = pearlrt::toChar(' ');
   data[8] = pearlrt::toChar('\n');
   data[9] = pearlrt::toChar(' ');
   data[10] = pearlrt::toChar('4');
   data[11] = pearlrt::toChar('2');
   pearlrt::DationRW log_bin(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             (pearlrt::Fixed<15>)1);
   ASSERT_NO_THROW(
      log_bin.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY ,
         & filename,
         NULL));
   log_bin.dationWrite(&data, sizeof(data));
   log_bin.dationClose();
   /* read with GET and compare */
   pearlrt::Character<8> text;
   pearlrt::Character<8> expect("PEARL");
   pearlrt::DationPG logbuch(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::OLD ,
         & filename,
         NULL));
   pearlrt::Fixed<31>  x;
   logbuch.fromA(text);
   EXPECT_TRUE((text == expect).getBoolean());
   // the value 42 is in the next line; read now should deliver 0
   logbuch.fromF(x, (pearlrt::Fixed<15>)3);
   EXPECT_EQ((x == (pearlrt::Fixed<31>)0).getBoolean(), true);
   logbuch.fromSkip(1);
   logbuch.fromF(x, (pearlrt::Fixed<15>)3);
   EXPECT_EQ((x == (pearlrt::Fixed<31>)42).getBoolean(), true);
   logbuch.dationClose();
}


/**
  test signals on CYCLIC and DationPG
*/

TEST(DationPG, cyclicFails) {
   pearlrt::Log::info("*** DationPG: cyclic fails ***");
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(2, 80);
   ASSERT_THROW(
      pearlrt::DationPG logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim),
      pearlrt::IllegalParamSignal);
}

/**
  test signal handling with and without RST
*/

TEST(DationPG, signal_get) {
   pearlrt::Log::info("*** DationPG: signal_get start ***");
   pearlrt::Character<9> filename("get_1.txt");
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(80);
   /* -------------------------------------------- */
   pearlrt::Log::info("      DationPG: simple_get start   ");
   /* write binary */
   pearlrt::Character<1> data[12];
   data[0] = pearlrt::toChar('P');
   data[1] = pearlrt::toChar('E');
   data[2] = pearlrt::toChar('A');
   data[3] = pearlrt::toChar('R');
   data[4] = pearlrt::toChar('L');
   data[5] = pearlrt::toChar(' ');
   data[6] = pearlrt::toChar(' ');
   data[7] = pearlrt::toChar(' ');
   data[8] = pearlrt::toChar('\n');
   data[9] = pearlrt::toChar(' ');
   data[10] = pearlrt::toChar('4');
   data[11] = pearlrt::toChar('2');
   pearlrt::DationRW log_bin(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             (pearlrt::Fixed<15>)1);
   ASSERT_NO_THROW(
      log_bin.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY ,
         & filename,
         NULL));
   log_bin.dationWrite(&data, sizeof(data));
   log_bin.dationClose();
   /* read with GET and compare */
   pearlrt::Character<8> text;
   pearlrt::Character<8> expect("PEARL");
   pearlrt::DationPG logbuch(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim);
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::OLD ,
         & filename,
         NULL));
   pearlrt::Fixed<31>  x;
   pearlrt::Fixed<15>  rst;
   // try to read without rst-value --> signal should be induced
   ASSERT_THROW(

   try {
      logbuch.beginSequence(NULL);
      logbuch.fromF(x, (pearlrt::Fixed<15>)3);
      logbuch.endSequence();
   } catch (pearlrt::Signal & s) {
      if (!logbuch.updateRst(&s)) {
         logbuch.endSequence();
         throw;
      }

      logbuch.endSequence();
   },
   pearlrt::FixedValueSignal);
   ASSERT_NO_THROW(

   try {
      logbuch.beginSequence(NULL);
      logbuch.rst(rst);
      logbuch.fromF(x, (pearlrt::Fixed<15>)3);
      logbuch.endSequence();
   } catch (pearlrt::Signal & s) {
      if (!logbuch.updateRst(&s)) {
         logbuch.endSequence();
         throw;
      }

      logbuch.endSequence();
   }
   );
   ASSERT_EQ(rst.x , pearlrt::theFixedValueSignal.whichRST());
   logbuch.dationClose();
}
