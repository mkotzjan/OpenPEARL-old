/*
 [The "BSD license"]
 Copyright (c) 2018 Rainer Mueller
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

\brief Test routines for bit slice operations

\page Testprograms

\section Bit Bit Slice Tests
Test routines for slices of the data type BIT(x).

There are several unit tests using the google test framework.

\cond TREAT_EXAMPLES
*/
#include <stdio.h>
#include <stdint.h>
#include "gtest.h"

#include "BitString.h"
#include "BitSlice.h"
#include "compare.h"
#include "Signals.h"

   // reminder how the test 64 bit values
   //pearlrt::BitString<53> b533(pearlrt::BitString<3>(0x5));
   //EXPECT_EQ(b533.x, 0x0a000000000000000LL);

/**
Test getSlice exceptions for BitStrings
*/
TEST(BitSliceTest, getSlice) {
   pearlrt::Fixed<15> zwei(2);
   {
   pearlrt::Fixed<15> x(0);
   pearlrt::BitString<16> b16(0x0ffff);
   EXPECT_THROW(
   pearlrt::BitSlice(b16).getSlice(x,zwei),
                             //    ^ too small
      pearlrt::BitIndexOutOfRangeSignal);
   }

   {
   pearlrt::Fixed<15> x(1);
   pearlrt::BitString<16> b16(0x0ffff);
   EXPECT_THROW(
   pearlrt::BitSlice(b16).getSlice(zwei,x),
                                    //  ^ too small
      pearlrt::BitIndexOutOfRangeSignal);
   }

   {
   pearlrt::BitString<16> b16(0x0ffff);
   pearlrt::Fixed<15> x(17);
   pearlrt::Fixed<15> y(18);
   EXPECT_THROW(
   pearlrt::BitSlice(b16).getSlice(x,y),
                              //   ^ ^ too large
      pearlrt::BitIndexOutOfRangeSignal);
   }
   {
   pearlrt::BitString<16> b16(0x0ffff);
   pearlrt::Fixed<15> x(17);
   EXPECT_THROW(
   pearlrt::BitSlice(b16).getSlice(zwei,x),
                                     //   ^ too large
      pearlrt::BitIndexOutOfRangeSignal);
   }
}

/**
test setSlice in a BitSlice
*/
TEST(BitSliceTest, setSlice) {
   pearlrt::Fixed<15> zwei(2), drei(3), vier(4);
   pearlrt::BitString<2> b2(2);
   pearlrt::BitString<3> b3;

   {
   pearlrt::BitString<16> b16(0x0ffff);
   pearlrt::BitSlice(b16).getSlice(zwei,vier).setSlice(b2);
   EXPECT_EQ(b16.x,0xcfff);
   }

   {
   pearlrt::BitString<16> b16(0x0ffff);
   pearlrt::BitSlice(b16).getSlice(zwei,drei).setSlice(b2);
   EXPECT_EQ(b16.x,0xdfff);
   }

   {
   pearlrt::BitString<16> b16(0x0ffff);
   EXPECT_THROW(
   pearlrt::BitSlice(b16).getSlice(zwei,drei).setSlice(b3),
                                                      // ^^ too large
      pearlrt::BitIndexOutOfRangeSignal);
   }
}


/**
test mkBitString  from BitSlice
*/
TEST(BitSliceTest, mkBitString) {
   pearlrt::Fixed<15> eins(1),zwei(2), drei(3), vier(4), sechs(6);
   pearlrt::BitString<16> b16(0x05f5f);
   pearlrt::BitString<2> b2;
   pearlrt::BitString<5> b5;
  
   b2 = pearlrt::BitSlice(b16)
           .getSlice(zwei,drei)
           .mkBitString((pearlrt::BitString<2>*)0);
   EXPECT_EQ(b2.x,(uint8_t)0x80);
  
   b2 = pearlrt::BitSlice(b16)
           .getSlice(eins,zwei)
           .mkBitString((pearlrt::BitString<2>*)0);
   EXPECT_EQ(b2.x,(uint8_t)0x40);
  
   b5 = pearlrt::BitSlice(b16)
           .getSlice(eins,vier)
           .mkBitString((pearlrt::BitString<5>*)0);
   EXPECT_EQ(b5.x,(uint8_t)0x50);

   EXPECT_THROW(
      pearlrt::BitSlice(b16)
           .getSlice(eins,vier)
           .mkBitString((pearlrt::BitString<3>*)0),
       pearlrt::InternalDatatypeSignal);
}

/**
\endcond
*/
