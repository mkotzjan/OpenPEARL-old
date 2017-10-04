/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Holger Koelle
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

\brief Test routines basic non-basic systemdations (UserDation)

\page Testprograms

\section UserDationNBTests tests/UserDationNBTests.cc
Test routines generic non-basic systemdations

There are several unit tests using the google test framework.
Note that the position tests do all combinations of STREAM and
CYCLIC on DIRECT dations.

<ul>
<li>Dim1: tests creation of simple Dim object
<li>DCL: tests dation declarations
<li>OPEN: tests dation open scenarios
<li>OpenClose: tests closing with PRM, CAN
<li>PosBasic: basic positioning
<li>Dim1_20_Pos: positioning on DIM(20) Dation
<li>Dim1_unbounded_Pos: positioning on DIM(*) Dation
<li>Dim2_10x20_Pos: positioning on DIM(10,20) dation
<li>Dim2_20_Pos: positioning on DIM(*,20) DATION
<li>Dim3_5x10x20_Pos: positioning on DIM(5,10,20) DATION
<li>Dim3_10x20_Pos: positioning on DIM(*,10,20) DATION
<li>RST: rst-attribute operations
<li>Dim3_10x20_PosForward: forward limitations

</ul>



*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "SystemDationNB.h"
#include "UserDation.h"
#include "DationRW.h"
#include "Dation.h"
#include "DationDim1.h"
#include "DationDim2.h"
#include "DationDim3.h"
#include "Disc.h"
#include "Pipe.h"
#include "DationRW.h"
#include "Device.h"
#include "Fixed.h"
#include "Character.h"
#include "Log.h"

using namespace std;

static pearlrt::Disc disc("/tmp/", 5);
static pearlrt::Device* _disc = &disc;
static pearlrt::Pipe myPipe("/tmp/pipe", 5, "ANY CAN OPEN1");
static pearlrt::Device* _myPipe = &myPipe;

/**
Test exception creation for 1 dimensional objects (number <=0)
*/
TEST(UserDationNB, Dim1) {
   pearlrt::Log::info("*** Dim1 start ***");
   ASSERT_THROW(
      pearlrt::DationDim1 dim(-1),
      pearlrt::InternalDationSignal);
   ASSERT_THROW(
      pearlrt::DationDim1 dim(0),
      pearlrt::InternalDationSignal);
   ASSERT_NO_THROW(
      pearlrt::DationDim1 dim(20));
   pearlrt::Log::info("*** Dim1 end ***");
}

/**
test Dation DCL statements
<ul>
<li>missing dimensions
<li>wrong combinations (FORWARD + CYCLIC)
<li>incompatible capabilities (FORBACK)
<li>..
</ul>
*/
TEST(UserDationNB, DCL) {
   pearlrt::Log::info("*** DCL start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(20);
   // no DIM given
   ASSERT_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                NULL,
                                pearlrt::Fixed<15>(1)),
      pearlrt::InternalDationSignal);
   // attribute defaulting
   pearlrt::Log::info("*** attribute defaulting ***");
   ASSERT_NO_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1)));
   ASSERT_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(1)),
      pearlrt::InternalDationSignal);
   ASSERT_NO_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::NOSTREAM,
                                &dim,
                                pearlrt::Fixed<15>(1)));
   ASSERT_NO_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::FORWARD,
                                &dim,
                                pearlrt::Fixed<15>(1)));
   // missing direction
   pearlrt::Log::info("*** missing direction ***");
   ASSERT_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::FORWARD,
                                &dim,
                                pearlrt::Fixed<15>(1)),
      pearlrt::InternalDationSignal);
   // missing positioning
   ASSERT_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT,
                                &dim,
                                pearlrt::Fixed<15>(1)),
      pearlrt::InternalDationSignal);
   // multiple direction
   ASSERT_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::IN |
                                pearlrt::Dation::OUT,
                                &dim,
                                pearlrt::Fixed<15>(1)),
      pearlrt::InternalDationSignal);
   // multiple positioning
   ASSERT_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::FORWARD |
                                pearlrt::Dation::DIRECT,
                                &dim,
                                pearlrt::Fixed<15>(1)),
      pearlrt::InternalDationSignal);
   // incompatible capabilities (Disc supports FORWARD and DIRECT)
   pearlrt::Log::info("test incomp. caps");
   ASSERT_THROW(
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::INOUT |
                                pearlrt::Dation::FORBACK,
                                &dim,
                                pearlrt::Fixed<15>(1)),
      pearlrt::InternalDationSignal);
   pearlrt::Log::info("*** DCL start ***");
}

/**
Test dation open with idf and missing idf behaves correct.
Tests IDF in combinations with OLD,NEW,ANY

Close(CAN) is expected to work
*/
TEST(UserDationNB, OPEN) {
   pearlrt::Log::info("*** OPEN start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _myPipe;
   pearlrt::SystemDationNB* myPipe_ =
      static_cast<pearlrt::SystemDationNB*>(_myPipe);
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(20);
   /* -------------------------------------------- */
   pearlrt::DationRW logpipe(myPipe_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             pearlrt::Fixed<15>(1));

   pearlrt::DationRW logbuch(disc_,
                             pearlrt::Dation::INOUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             pearlrt::Fixed<15>(1));
   pearlrt::Log::info("*** idf + filename test ***");
   pearlrt::Fixed<15> error;
   ASSERT_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::ANY ,
         &filename,
         (pearlrt::Fixed<15>*)NULL),
      pearlrt::InternalDationSignal);
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::ANY | pearlrt::Dation::RST ,
         &filename,&error));
   ASSERT_EQ(error.x, pearlrt::theInternalDationSignal.whichRST());
   pearlrt::Log::info("*** idf +  no old/new/any  ***");
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF ,
         &filename,
         (pearlrt::Fixed<15>*)NULL));
   logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);

   // should open file with system defaulted name
   pearlrt::Log::info("*** no idf +  no old/new/any --> system file name ***");
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         0 ,
         (pearlrt::Character<1>*)0,
         (pearlrt::Fixed<15>*)NULL));
   logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);

   pearlrt::Log::info("*** CAN+PRM  ***");
   ASSERT_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY |
         pearlrt::Dation::PRM |
         pearlrt::Dation::CAN ,
         &filename,
         (pearlrt::Fixed<15>*)NULL),
      pearlrt::InternalDationSignal);
   pearlrt::Log::info("*** successful open  ***");
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY ,
         & filename,
         (pearlrt::Fixed<15>*)NULL));
   logbuch.dationClose(pearlrt::Dation::CAN, (pearlrt::Fixed<15>*)0);

   // tests with idf in userdation and different system dations
   pearlrt::Log::info("*** OPEN no IDF on Disc");
   ASSERT_THROW(
      logbuch.dationOpen(pearlrt::Dation::NEW ,
                         (pearlrt::Character<1>*)(NULL), (pearlrt::Fixed<15>*)NULL),
      pearlrt::DationParamSignal);
   ASSERT_NO_THROW(
      logbuch.dationOpen(0 , (pearlrt::Character<1>*)(NULL), (pearlrt::Fixed<15>*)NULL));
   logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);

   pearlrt::Log::info("*** OPEN  IDF on Pipe");
   ASSERT_THROW(
      logpipe.dationOpen(
         pearlrt::Dation::IDF ,
         & filename,
         (pearlrt::Fixed<15>*)NULL),
      pearlrt::DationParamSignal);

   pearlrt::Log::info("*** OPEN no IDF on Pipe");
   ASSERT_NO_THROW(
      logpipe.dationOpen(
         0 ,
         (pearlrt::Character<1>*)(NULL),
         (pearlrt::Fixed<15>*)NULL));
   logpipe.dationClose(0, (pearlrt::Fixed<15>*)0);


   pearlrt::Log::info("*** OPEN end ***");
}

/**
Test CAN + PRM option in OPEN and CLOSE statements
*/
TEST(UserDationNB, OpenClose) {
   pearlrt::Log::info("*** OpenClose start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(20);
   /* -------------------------------------------- */
   pearlrt::DationRW logbuch(disc_,
                             pearlrt::Dation::OUT |
                             pearlrt::Dation::FORWARD |
                             pearlrt::Dation::STREAM |
                             pearlrt::Dation::NOCYCL,
                             &dim,
                             pearlrt::Fixed<15>(1));
   // make file to be not present
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY ,
         & filename,
         (pearlrt::Fixed<15>*)NULL));
   logbuch.dationClose(pearlrt::Dation::CAN, (pearlrt::Fixed<15>*)0);
   ASSERT_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::OLD ,
         & filename,
         (pearlrt::Fixed<15>*)NULL),
      pearlrt::OpenFailedSignal);
   pearlrt::Log::info("*** check if CAN works in OPEN ***");
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY |
         pearlrt::Dation::CAN,
         & filename,
         (pearlrt::Fixed<15>*)NULL));
   logbuch.dationClose(0,(pearlrt::Fixed<15>*)0);
   ASSERT_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::OLD ,
         & filename,
         (pearlrt::Fixed<15>*)NULL),
      pearlrt::OpenFailedSignal);
   pearlrt::Log::info("*** check if CAN superseeds in OPEN ***");
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY |
         pearlrt::Dation::PRM,
         & filename,
         (pearlrt::Fixed<15>*)NULL));
   logbuch.dationClose(pearlrt::Dation::CAN,(pearlrt::Fixed<15>*)0);
   ASSERT_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::OLD ,
         & filename,
         (pearlrt::Fixed<15>*)NULL),
      pearlrt::OpenFailedSignal);
   pearlrt::Log::info("*** check close params ***");
   ASSERT_NO_THROW(
      logbuch.dationOpen(
         pearlrt::Dation::IDF |
         pearlrt::Dation::ANY |
         pearlrt::Dation::PRM,
         & filename,
         (pearlrt::Fixed<15>*)NULL));
   ASSERT_THROW(
      logbuch.dationClose(pearlrt::Dation::CAN |
                          pearlrt::Dation::PRM, (pearlrt::Fixed<15>*)0),
      pearlrt::InternalDationSignal);
   logbuch.dationClose(pearlrt::Dation::CAN, (pearlrt::Fixed<15>*)0);
   pearlrt::Log::info("*** OpenClose end ***");
}

/**
test basic positioning operations on DIRECT dation
*/
TEST(UserDationNB, PosBasic) {
   pearlrt::Log::info("*** PosBasic start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS on non DIRECT ***");
   {
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
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
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_THROW(
         logbuch.pos(1),
         pearlrt::InternalDationSignal);
      logbuch.dationClose(pearlrt::Dation::CAN, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** invalid POS on DIRECT ***");
   {
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1));
      ASSERT_NO_THROW(logbuch.sop(&c));
      ASSERT_EQ(c.x, 1);
      ASSERT_THROW(
         logbuch.pos(-1),
         pearlrt::DationIndexBoundSignal);
      ASSERT_THROW(
         logbuch.pos(21),
         pearlrt::DationIndexBoundSignal);
      logbuch.dationClose(pearlrt::Dation::CAN, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** PosBasic end ***");
}

/**
test positioning on DIRECT DIM(10)
with CYCIC,NOCYCL and STREAM
*/
TEST(UserDationNB, Dim1_20_Pos) {
   pearlrt::Log::info("*** Dim1Pos DIM(20) start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim(20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL DIRECT DIM(20)  ***");
   {
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(logbuch.pos(20));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.pos(10));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.adv(10));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(1)); // skip over last element
      ASSERT_THROW(
         logbuch.adv(1),		// skip after last element
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 21);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS STREAM CYCLIC DIRECT DIM(20)  ***");
   {
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(logbuch.pos(20));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.pos(10));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.adv(10));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(1));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 1);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** Dim1Pos end ***");
}


/**
test positioning on DIRECT DIM()
with CYCIC,NOCYCL and STREAM
*/
TEST(UserDationNB, Dim1_unbound_Pos) {
   pearlrt::Log::info("*** Dim1Pos DIM(*) start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim1 dim;
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL DIRECT DIM(*)  ***");
   {
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(logbuch.pos(20));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(1));
      logbuch.sop(&c);
      ASSERT_EQ(c.x, 21);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS STREAM CYCLIC DIRECT DIM(*)  ***");
   {
      pearlrt::Fixed<31> c;
      ASSERT_THROW(
         pearlrt::DationRW logbuch(disc_,
                                   pearlrt::Dation::OUT |
                                   pearlrt::Dation::DIRECT |
                                   pearlrt::Dation::STREAM |
                                   pearlrt::Dation::CYCLIC,
                                   &dim,
                                   pearlrt::Fixed<15>(1)),
         pearlrt::InternalDationSignal);
   }
   pearlrt::Log::info("*** Dim1Pos end ***");
}


/**
test positioning on DIRECT DIM(10,20)
with CYCIC,NOCYCL and STREAM, NOSTREAM
*/
TEST(UserDationNB, Dim2_10x20_Pos) {
   pearlrt::Log::info("*** Dim2Pos DIM(10,20) start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(10, 20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL DIRECT"
                      " DIM(10,20)  ***");
   {
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(5, 10));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(20));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(10));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 6);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(10, 20));
      ASSERT_NO_THROW(logbuch.adv(1));  // read/write last element
      ASSERT_THROW(
         logbuch.adv(1),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 11);
      ASSERT_EQ(c.x, 1);
      ASSERT_THROW(
         logbuch.adv(0, 1),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 11);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(
         logbuch.adv(-5, -30));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 4);
      ASSERT_EQ(c.x, 11);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS STREAM CYCLIC DIRECT DIM(10,20)  ***");
   {
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1, 20));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(0, 1));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 2);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(logbuch.pos(10, 20));
      ASSERT_NO_THROW(logbuch.adv(0, 1));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 1);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS NOSTREAM CYCLIC DIRECT DIM(10,20)  ***");
   {
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::NOSTREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1, 20));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(0, 1));  // skip over last element
      ASSERT_THROW(
         logbuch.adv(0, 1),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 21);
      ASSERT_NO_THROW(logbuch.pos(10, 20));
   }
   pearlrt::Log::info("*** Dim2Pos end ***");
}
// ++++++++++++++++++

/**
test positioning on DIRECT DIM(*,20)
with CYCIC,NOCYCL and STREAM, NOSTREAM
*/
TEST(UserDationNB, Dim2_20_Pos) {
   pearlrt::Log::info("*** Dim2Pos DIM(*,20) start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim2 dim(20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL DIRECT"
                      " DIM(*,20)  ***");
   {
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(5, 10));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(20));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(10));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 6);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(10, 20));
      ASSERT_NO_THROW(
         logbuch.adv(1));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 11);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(
         logbuch.adv(-5, -30));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 4);
      ASSERT_EQ(c.x, 11);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS STREAM CYCLIC DIRECT DIM(*,20)  ***");
   {
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      ASSERT_THROW(
         pearlrt::DationRW logbuch(disc_,
                                   pearlrt::Dation::OUT |
                                   pearlrt::Dation::DIRECT |
                                   pearlrt::Dation::STREAM |
                                   pearlrt::Dation::CYCLIC,
                                   &dim,
                                   pearlrt::Fixed<15>(1)),
         pearlrt::InternalDationSignal);
   }
   pearlrt::Log::info("*** Dim2Pos end ***");
}

/**
test positioning on DIRECT DIM(5,10,20)
with CYCIC,NOCYCL and STREAM, NOSTREAM
*/
TEST(UserDationNB, Dim3_5x10x20_Pos) {
   pearlrt::Log::info("*** Dim3Pos DIM(5,10,20) start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim3 dim(5, 10, 20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL DIRECT"
                      " DIM(5,10,20)  ***");
   {
      pearlrt::Fixed<31> p;
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(2, 5, 10));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(20));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(10));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 6);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(5, 10, 20));
      ASSERT_NO_THROW(logbuch.adv(1)); //read/write last element
      ASSERT_THROW(
         logbuch.adv(1),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 6);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 1);
      ASSERT_THROW(
         logbuch.adv(0, 1),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 6);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(
         logbuch.adv(-2, -5, -30));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 3);
      ASSERT_EQ(r.x, 4);
      ASSERT_EQ(c.x, 11);
      ASSERT_NO_THROW(logbuch.pos(1, 1, 1));
      ASSERT_THROW(
         logbuch.adv(-1),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 1);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 1);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS STREAM CYCLIC DIRECT DIM(5,10,20)"
                      "  ***");
   {
      pearlrt::Fixed<31> p;
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1, 20));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 1);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(0, 1));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 2);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(logbuch.pos(5, 10, 20));
      ASSERT_NO_THROW(logbuch.adv(0, 1));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 1);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 1);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS NOSTREAM CYCLIC DIRECT"
                      " DIM(5,10,20)  ***");
   {
      pearlrt::Fixed<31> p;
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::NOSTREAM |
                                pearlrt::Dation::CYCLIC,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(1, 20));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 1);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(0, 1));   // skip past last element
      ASSERT_THROW(
         logbuch.adv(0, 1),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 1);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 21);
      ASSERT_NO_THROW(logbuch.pos(10, 20));
      ASSERT_NO_THROW(logbuch.pos(2, 10, 2));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 10);
      ASSERT_EQ(c.x, 2);
      ASSERT_THROW(
         logbuch.adv(1, 0),
         pearlrt::DationIndexBoundSignal);
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 10);
      ASSERT_EQ(c.x, 2);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** Dim2Pos end ***");
}
// ++++++++++++++++++

/**
test positioning on DIRECT DIM(*,10,20)
with CYCIC,NOCYCL and STREAM, NOSTREAM
*/
TEST(UserDationNB, Dim3_10x20_Pos) {
   pearlrt::Log::info("*** Dim3Pos DIM(*,10,20) start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim3 dim(10, 20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL DIRECT"
                      " DIM(*,10,20)  ***");
   {
      pearlrt::Fixed<31> p;
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.pos(2, 5, 10));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(20));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(logbuch.adv(10));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 2);
      ASSERT_EQ(r.x, 6);
      ASSERT_EQ(c.x, 10);
      ASSERT_NO_THROW(logbuch.pos(10, 20));
      ASSERT_NO_THROW(
         logbuch.adv(1));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 3);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 1);
      ASSERT_NO_THROW(
         logbuch.adv(-5, -30));
      logbuch.sop(&r, &c);
      ASSERT_EQ(r.x, 4);
      ASSERT_EQ(c.x, 11);
      ASSERT_NO_THROW(logbuch.pos(2, 10, 20));
      ASSERT_NO_THROW(
         logbuch.adv(1, 0));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 3);
      ASSERT_EQ(r.x, 1);
      ASSERT_EQ(c.x, 20);
      ASSERT_NO_THROW(
         logbuch.adv(-1, -5, -30));
      logbuch.sop(&p, &r, &c);
      ASSERT_EQ(p.x, 1);
      ASSERT_EQ(r.x, 5);
      ASSERT_EQ(c.x, 10);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
   pearlrt::Log::info("*** POS STREAM CYCLIC DIRECT"
                      " DIM(*,10,20)  ***");
   {
      pearlrt::Fixed<31> p;
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      ASSERT_THROW(
         pearlrt::DationRW logbuch(disc_,
                                   pearlrt::Dation::OUT |
                                   pearlrt::Dation::DIRECT |
                                   pearlrt::Dation::STREAM |
                                   pearlrt::Dation::CYCLIC,
                                   &dim,
                                   pearlrt::Fixed<15>(1)),
         pearlrt::InternalDationSignal);
   }
   pearlrt::Log::info("*** Dim3Pos end ***");
}

/**
test RST-variable treatment
*/
TEST(Userdation, RST) {
   pearlrt::Log::info("*** Dim3Pos DIM(*,10,20) start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim3 dim(10, 20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL DIRECT"
                      " DIM(*,10,20)  ***");
   {
      pearlrt::Fixed<31> p;
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::Fixed<15> rst;
      pearlrt::Fixed<5> rst5;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
                                pearlrt::Dation::DIRECT |
                                pearlrt::Dation::STREAM |
                                pearlrt::Dation::NOCYCL,
                                &dim,
                                pearlrt::Fixed<15>(1));
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::RST |
            pearlrt::Dation::OLD ,
            (pearlrt::Character<1>*)0,  // missing filename
            &rst));
      ASSERT_EQ(rst.x, pearlrt::theDationParamSignal.whichRST());

      ASSERT_THROW(
         logbuch.dationClose(0, (pearlrt::Fixed<15>*)0),
         pearlrt::DationNotOpenSignal);
      logbuch.dationClose(pearlrt::Dation::RST, &rst);
      ASSERT_EQ(rst.x, pearlrt::theDationNotOpenSignal.whichRST());

      ASSERT_THROW(
         logbuch.dationClose(pearlrt::Dation::RST, &rst5),
         pearlrt::FixedRangeSignal);
 
      ASSERT_NO_THROW(
         logbuch.dationOpen(
            pearlrt::Dation::IDF |
            pearlrt::Dation::ANY ,
            & filename,
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_THROW(
         logbuch.pos(0, 5, 10),
         pearlrt::DationIndexBoundSignal);
      logbuch.beginSequence(NULL);
      ASSERT_THROW(
         logbuch.pos(0, 5, 10),
         pearlrt::DationIndexBoundSignal);
      logbuch.endSequence();
      logbuch.beginSequence(NULL);
      logbuch.rst(rst);
      ASSERT_THROW(logbuch.pos(0, 5, 10),
                   pearlrt::DationIndexBoundSignal);
      logbuch.endSequence();

      // simulate the compiler output with rst-specified
      // not throwing
      try {
         logbuch.beginSequence(NULL);
         logbuch.rst(rst);
         logbuch.pos(0, 5, 10),
                     logbuch.endSequence();
      } catch (pearlrt::Signal & s) {
         logbuch.endSequence();

         if (!logbuch.updateRst(&s)) {
            // this should not occur.
            ASSERT_NO_THROW(
               throw;
            );
         }
      }

      // simulate the compiler output with rst-specified
      // throwing
      try {
         logbuch.beginSequence(NULL);
         logbuch.pos(0, 5, 10),
                     logbuch.endSequence();
         logbuch.rst(rst);
      } catch (pearlrt::Signal & s) {
         logbuch.endSequence();

         if (!logbuch.updateRst(&s)) {
            // this should not occur.
            ASSERT_THROW(
               throw,
               pearlrt::DationIndexBoundSignal);
         }
      }

      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
}

/**
test positioning on FORWARD DIM(10,20)
with CYCIC,NOCYCL and STREAM, NOSTREAM
*/
TEST(UserDationNB, Dim3_10x20_PosForward) {
   pearlrt::Log::info("*** Dim3Pos DIM(*,10,20) FORWARD start ***");
   pearlrt::Character<9> filename("file1.txt");
   extern pearlrt::Device* _disc;
   pearlrt::SystemDationNB* disc_ =
      static_cast<pearlrt::SystemDationNB*>(_disc);
   pearlrt::DationDim3 dim(10, 20);
   /* -------------------------------------------- */
   pearlrt::Log::info("*** POS STREAM NOCYCL FORWARD"
                      " DIM(10,20)  ***");
   {
      pearlrt::Fixed<31> p;
      pearlrt::Fixed<31> r;
      pearlrt::Fixed<31> c;
      pearlrt::DationRW logbuch(disc_,
                                pearlrt::Dation::OUT |
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
            (pearlrt::Fixed<15>*)NULL));
      ASSERT_NO_THROW(logbuch.adv(1, 4, 9));
      ASSERT_THROW(
         logbuch.sop(&p, &r, &c),
         pearlrt::InternalDationSignal);
      ASSERT_THROW(
         logbuch.adv(-1),
         pearlrt::InvalidPositioningSignal);
      logbuch.dationClose(0, (pearlrt::Fixed<15>*)0);
   }
}

