/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
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

\brief Test routines generic non-basic systemdations (RW-Dation)

\page Testprograms

\section DationRW


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
#include "DationRW.h"
#include "Device.h"
#include "Fixed.h"
#include "myTests.h"
#include "compare.h"

using namespace std;

static pearlrt::Disc disc("/tmp/", 5);
static pearlrt::Device* _disc = &disc;


/**
  Writing three different fixed<31> values to testfile
*/

TEST(DationRW, read_write_FORWARD) {
   pearlrt::Log::info("*** DationRW: read_write_to_file start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(20);
   /* -------------------------------------------- */
   pearlrt::DationRW logbuch(disc_,
                             pearlrt::Dation::INOUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             pearlrt::Fixed<15>(1));
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY ,
         & filename,
         NULL));
   pearlrt::Character<16> text("PEARL");
   pearlrt::Fixed<31>  x(42);
   logbuch.dationWrite(&text, sizeof(text));
   logbuch.dationWrite(&x, sizeof(x));
   logbuch.dationClose(pearlrt::Dation::PRM);
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::OLD ,
         & filename,
         NULL));
   pearlrt::Character<16> text1;
   pearlrt::Fixed<31>  x1;
   logbuch.dationRead(&text1, sizeof(text1));
   logbuch.dationRead(&x1, sizeof(x1));
   logbuch.dationClose(pearlrt::Dation::CAN);
   ASSERT_TRUE((text == text1).getBoolean());
   ASSERT_TRUE((x == x1).getBoolean());
}

/* \test
  READ/Write 2 dimenional checkerfield
*/
TEST(DationRW, Read_Write_checkerfield) {
   pearlrt::Log::info("*** DationRW: read_write_checkerfield start ***");
   pearlrt::Character<9> filename("chck1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(20, 20);
   pearlrt::Fixed<31> testValue;
   /* -------------------------------------------- */
   {
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));

      for (int i = 1; i <= 20; i++) {
         for (int j = 1; j <= 20; j++) {
            testValue = pearlrt::Fixed<31>(i * 100 + j);
            logbuch.pos(i, j);
            logbuch.dationWrite(&testValue, sizeof(testValue));
         }
      }

      // read in other order
      for (int j = 1; j <= 20; j++) {
         for (int i = 1; i <= 20; i++) {
            logbuch.pos(i, j);
            logbuch.dationRead(&testValue, sizeof(testValue));
            ASSERT_EQ((testValue == pearlrt::Fixed<31>(i * 100 + j))
                      .getBoolean(), true);
         }
      }

      logbuch.dationClose(pearlrt::Dation::PRM);
   }
   {
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::IN |
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      // read so values by postioning with x, skip and adv
      logbuch.fromAdv(1, 2);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_EQ((testValue == pearlrt::Fixed<31>(2 * 100 + 3)).getBoolean(),
                true);
      logbuch.fromX(2);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_EQ((testValue == pearlrt::Fixed<31>(2 * 100 + 6)).getBoolean(),
                true);
      logbuch.fromSkip(2);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_EQ((testValue == pearlrt::Fixed<31>(4 * 100 + 1)).getBoolean(),
                true);
      logbuch.dationClose(pearlrt::Dation::PRM);
   }
}


/**
Test write X,SKIP,PAGE  on 1 dim dation
*/
TEST(DationRW, toSkip1Dim) {
   pearlrt::Log::info("*** DationRW: toSkip1Dim start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(20);
   pearlrt::Fixed<31> testValue[20];
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      ASSERT_THROW(file.toSkip(1), pearlrt::InternalDationSignal);
      ASSERT_THROW(file.toPage(1), pearlrt::InternalDationSignal);
      ASSERT_NO_THROW(file.toX(20));
      ASSERT_THROW(
         file.toX(1),
         pearlrt::DationIndexBoundSignal);
      file.dationClose();
   }
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::OLD ,
            & filename,
            NULL));
      ASSERT_THROW(file.fromSkip(1), pearlrt::InternalDationSignal);
      ASSERT_THROW(file.fromPage(1), pearlrt::InternalDationSignal);
      file.dationRead(testValue, sizeof(testValue));
      file.dationClose();
      pearlrt::Fixed<31> cmpTestValue[20] = {(pearlrt::Fixed<31>)0};
      ASSERT_TRUE(
         ARRAY_EQUAL(20, cmpTestValue, testValue));
   }
   pearlrt::Log::info("*** DationRW: toSkip1Dim end ***");
}


/**
Test write X,SKIP,PAGE  on 2 dim dation
*/
TEST(DationRW, toSkip2Dim) {
   pearlrt::Log::info("*** DationRW: toSkip2Dim start ***");
   pearlrt::Character<9> filename("file2.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(10, 20);
   pearlrt::Fixed<31> testValue[200];
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      ASSERT_NO_THROW(file.toSkip(1));
      ASSERT_THROW(file.toPage(1), pearlrt::InternalDationSignal);
      ASSERT_NO_THROW(file.toX(20));
      ASSERT_NO_THROW(
         file.toX(1));
      ASSERT_NO_THROW(file.toSkip(8));
      ASSERT_THROW(
         file.toX(1),
         pearlrt::DationIndexBoundSignal);
      file.dationClose();
   }
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::OLD ,
            & filename,
            NULL));
      ASSERT_THROW(file.fromPage(1), pearlrt::InternalDationSignal);
      file.dationRead(testValue, sizeof(testValue));
      file.dationClose();
      pearlrt::Fixed<31> cmpTestValue[200] = {(pearlrt::Fixed<31>)0};
      ASSERT_TRUE(
         ARRAY_EQUAL(200, cmpTestValue, testValue));
   }
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      pearlrt::Fixed<31> testVal = (pearlrt::Fixed<31>)42;
      ASSERT_NO_THROW(file.toSkip(1));
      ASSERT_THROW(file.toPage(1), pearlrt::InternalDationSignal);
//   ASSERT_NO_THROW(file.toX(20));
      file.dationWrite(&testVal, sizeof(testVal));
      testVal = testVal + (pearlrt::Fixed<31>)(1);
      file.dationWrite(&testVal, sizeof(testVal));
      ASSERT_NO_THROW(file.toSkip(9));
      ASSERT_THROW(
         file.toX(1),
         pearlrt::DationIndexBoundSignal);
      file.dationClose();
   }
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::OLD ,
            & filename,
            NULL));
      file.dationRead(testValue, sizeof(testValue));
      file.dationClose();
      pearlrt::Fixed<31> cmpTestValue[200];

      for (int i = 0; i < 200; i++) {
         cmpTestValue[i] = (pearlrt::Fixed<31>)0;
      }

      cmpTestValue[20] = (pearlrt::Fixed<31>)42;
      cmpTestValue[21] = (pearlrt::Fixed<31>)43;
      ASSERT_TRUE(
         ARRAY_EQUAL(200, cmpTestValue, testValue));
   }
   pearlrt::Log::info("*** DationRW: toSkip1Dim end ***");
}


/**
Test write X,SKIP,PAGE  on 3 dim dation
*/
TEST(DationRW, toSkip3Dim) {
   pearlrt::Log::info("*** DationRW: toSkip3Dim start ***");
   pearlrt::Character<9> filename("file3.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim3 dim(2, 10, 10);
   pearlrt::Fixed<31> testValue[200];
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      ASSERT_NO_THROW(file.toSkip(1));
      ASSERT_NO_THROW(file.toPage(1));
      ASSERT_NO_THROW(file.toX(20));
      ASSERT_NO_THROW(
         file.toX(1));
      ASSERT_NO_THROW(file.toSkip(8));
      ASSERT_THROW(
         file.toX(1),
         pearlrt::DationIndexBoundSignal);
      file.dationClose();
   }
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::OLD ,
            & filename,
            NULL));
      file.dationRead(testValue, sizeof(testValue));
      file.dationClose();
      pearlrt::Fixed<31> cmpTestValue[200] = {(pearlrt::Fixed<31>)0};
      ASSERT_TRUE(
         ARRAY_EQUAL(200, cmpTestValue, testValue));
   }
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      pearlrt::Fixed<31> testVal = (pearlrt::Fixed<31>)42;
      ASSERT_NO_THROW(file.toPage(1));
      ASSERT_NO_THROW(file.toSkip(1));
      ASSERT_NO_THROW(file.toX(1));
      file.dationWrite(&testVal, sizeof(testVal));
      testVal = testVal + (pearlrt::Fixed<31>)(1);
      file.dationWrite(&testVal, sizeof(testVal));
      ASSERT_NO_THROW(file.toSkip(9));
      ASSERT_THROW(
         file.toX(1),
         pearlrt::DationIndexBoundSignal);
      file.dationClose();
   }
   {
      pearlrt::DationRW file(disc_,
                             pearlrt::Dation::IN |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         file.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::OLD ,
            & filename,
            NULL));
      file.dationRead(testValue, sizeof(testValue));
      pearlrt::Fixed<31> testVal;  // read over EOF
      ASSERT_THROW(
         file.dationRead(&testVal, sizeof(testVal)),
         pearlrt::DationIndexBoundSignal);
      file.dationClose();
      pearlrt::Fixed<31> cmpTestValue[200];

      for (int i = 0; i < 200; i++) {
         cmpTestValue[i] = (pearlrt::Fixed<31>)0;
      }

      cmpTestValue[110] = (pearlrt::Fixed<31>)42;
      cmpTestValue[111] = (pearlrt::Fixed<31>)43;
      ASSERT_TRUE(
         ARRAY_EQUAL(200, cmpTestValue, testValue));
   }
   pearlrt::Log::info("*** DationRW: toSkip3Dim end ***");
}

/* \test
  READ/Write 3 dimensional checkerfield
*/
TEST(DationRW, Read_Write_checkerfield3) {
   pearlrt::Log::info("*** DationRW: read_write_checkerfield3 start ***");
   pearlrt::Character<9> filename("chck1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim3 dim(10, 10, 10);
   pearlrt::Fixed<31> testValue;
   /* -------------------------------------------- */
   {
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));

      for (int i = 1; i <= 10; i++) {
         for (int j = 1; j <= 10; j++) {
            for (int k = 1; k <= 10; k++) {
               testValue = pearlrt::Fixed<31>(i * 100 + j * 10 + k);
               logbuch.pos(i, j, k);
               logbuch.dationWrite(&testValue, sizeof(testValue));
            }
         }
      }

      // read in other order
      for (int k = 1; k <= 10; k++) {
         for (int j = 1; j <= 10; j++) {
            for (int i = 1; i <= 10; i++) {
               logbuch.pos(i, j, k);
               logbuch.dationRead(&testValue, sizeof(testValue));
               ASSERT_TRUE((testValue ==
                            pearlrt::Fixed<31>(i * 100 + j * 10 + k)).
                           getBoolean());
            }
         }
      }

      logbuch.dationClose(pearlrt::Dation::PRM);
   }
   {
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::IN |
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                sizeof(pearlrt::Fixed<31>));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      // read so values by postioning with x, skip and adv
      logbuch.fromAdv(1, 2);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_TRUE((testValue == pearlrt::Fixed<31>(1 * 100 + 2 * 10 + 3))
                  .getBoolean());
      logbuch.fromX(2);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_TRUE((testValue == pearlrt::Fixed<31>(1 * 100 + 2 * 10 + 6))
                  .getBoolean());
      logbuch.fromSkip(2);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_TRUE((testValue == pearlrt::Fixed<31>(1 * 100 + 4 * 10 + 1))
                  .getBoolean());
      logbuch.fromPage(1);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_TRUE((testValue == pearlrt::Fixed<31>(2 * 100 + 1 * 10 + 1))
                  .getBoolean());
      logbuch.fromAdv(1, 2, 3);
      logbuch.dationRead(&testValue, sizeof(testValue));
      ASSERT_TRUE((testValue == pearlrt::Fixed<31>(3 * 100 + 3 * 10 + 5))
                  .getBoolean());
      logbuch.dationClose(pearlrt::Dation::PRM);
   }
}


/**
  read cyclic 1-dim
*/
TEST(DationRW, read_CYCLIC) {
   pearlrt::Log::info("*** DationRW: read_cyclic start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(3);
   /* -------------------------------------------- */
   {
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(4));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      pearlrt::Fixed<31>  x(42);
      logbuch.dationWrite(&x, sizeof(x));
      x = pearlrt::Fixed<31>(8);
      logbuch.dationWrite(&x, sizeof(x));
      x = pearlrt::Fixed<31>(15);
      logbuch.dationWrite(&x, sizeof(x));
      logbuch.dationClose(pearlrt::Dation::PRM);
   }{
      ASSERT_THROW(
         pearlrt::DationRW logbuch(disc_,
                                   pearlrt::Dation::IN |
                                   pearlrt::Dation::FORWARD |
                                   pearlrt::Dation::STREAM |
                                   pearlrt::Dation::CYCLIC,
                                   &dim,
                                   pearlrt::Fixed<15>(4)),
         pearlrt::IllegalParamSignal);
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::IN |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(4));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IN |
            pearlrt::Dation::IDF |
            pearlrt::Dation::OLD ,
            & filename,
            NULL));
      pearlrt::Fixed<31>  x1;
      pearlrt::Fixed<31>  y[4];
      logbuch.dationRead(&x1, sizeof(x1));
      logbuch.dationRead(&y, sizeof(y));
      logbuch.dationClose(pearlrt::Dation::CAN);
      ASSERT_TRUE((pearlrt::Fixed<31>(42) == x1).getBoolean());
      ASSERT_TRUE((pearlrt::Fixed<31>(8) == y[0]).getBoolean());
      ASSERT_TRUE((pearlrt::Fixed<31>(15) == y[1]).getBoolean());
      ASSERT_TRUE((pearlrt::Fixed<31>(42) == y[2]).getBoolean());
   }
}

/**
  write cyclic 1-dim
*/
TEST(DationRW, write_CYCLIC) {
   pearlrt::Log::info("*** DationRW: write_cyclic start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(3);
   /* -------------------------------------------- */
   {
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(4));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            NULL));
      pearlrt::Fixed<31>  y[4];
      y[0] = pearlrt::Fixed<31>(8);
      y[1] = pearlrt::Fixed<31>(15);
      y[2] = pearlrt::Fixed<31>(47);
      y[3] = pearlrt::Fixed<31>(11);
      pearlrt::Fixed<31>  x(42);
      logbuch.dationWrite(&x, sizeof(x));
      logbuch.dationWrite(&y, sizeof(y));
      logbuch.dationClose(pearlrt::Dation::PRM);
   }{
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::IN |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(4));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IN |
            pearlrt::Dation::IDF |
            pearlrt::Dation::OLD ,
            & filename,
            NULL));
      pearlrt::Fixed<31>  y[3];
      logbuch.dationRead(&y, sizeof(y));
      logbuch.dationClose(pearlrt::Dation::CAN);
      ASSERT_TRUE((pearlrt::Fixed<31>(47) == y[0]).getBoolean());
      ASSERT_TRUE((pearlrt::Fixed<31>(11) == y[1]).getBoolean());
      ASSERT_TRUE((pearlrt::Fixed<31>(15) == y[2]).getBoolean());
   }
}
