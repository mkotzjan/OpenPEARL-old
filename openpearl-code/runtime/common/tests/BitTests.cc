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

\brief Test routines for bit type operations

\page Testprograms

\section Bit Bit Tests
Test routines for the data type BIT(x).

There are several unit tests using the google test framework.

\cond TREAT_EXAMPLES
*/
#include <stdio.h>
#include <stdint.h>
#include "gtest.h"

#include "BitString.h"
#include "compare.h"
#include "Signals.h"

/**
test initializer
*/
TEST(BitTest, initializer) {
   pearlrt::BitString<1> b1(1);
   EXPECT_EQ(b1.x, 0x80);
   pearlrt::BitString<2> b2(1);
   EXPECT_EQ(b2.x, 0x40);
   pearlrt::BitString<16> b16(0x8000);
   EXPECT_EQ(b16.x, 0x08000);
   pearlrt::BitString<32> b32(0x80000000);
   EXPECT_EQ(b32.x, 0x80000000);
   pearlrt::BitString<64> b64a(0x0f0f0f0f0f0f0f0full);
   EXPECT_EQ(b64a.x, 0x0f0f0f0f0f0f0f0fULL);
   pearlrt::BitString<64> b64b(0xf0f0f0f0f0f0f0f0ull);
   EXPECT_EQ(b64b.x, 0xf0f0f0f0f0f0f0f0ULL);
   pearlrt::BitString<5> b53(pearlrt::BitString<3>(0x5));
   EXPECT_EQ(b53.x, 0x0a0);
   pearlrt::BitString<53> b533(pearlrt::BitString<3>(0x5));
   EXPECT_EQ(b533.x, 0x0a000000000000000LL);
}

/**
test assignment with assignments result
*/
TEST(BitTest, BitAssign) {
   pearlrt::BitString<1> tt(1);
   pearlrt::BitString<1> b1(1);
   pearlrt::BitString<3> b3;
   pearlrt::BitString<30> b30;
   pearlrt::Fixed<15> x(5);
   pearlrt::Fixed<15> y(6);

   b1 =  x < y;
   EXPECT_EQ(b1.x, 0x80); // true

   b1 = x > y;
   EXPECT_EQ(b1.x, 0x0); // false

   b3 = tt;
   EXPECT_EQ(b3.x, 0x80);

   b30 = tt;
   EXPECT_EQ(b30.x, 0x80000000);

   b3 = b3;
   EXPECT_THROW(b1 = b3,
                pearlrt::InternalDatatypeSignal);


}
/**
test bitNot
*/
TEST(BitTest, bitNot) {
   pearlrt::BitString<1> b1(1);
   b1 = b1.bitNot();
   EXPECT_EQ(b1.x, 0x00);
   pearlrt::BitString<2> b2(1);
   b2 = b2.bitNot();
   EXPECT_EQ(b2.x, 0x80);
   pearlrt::BitString<16> b16(0x5555);
   b16 = b16.bitNot();
   EXPECT_EQ(b16.x, 0x0AAAA);
}
/**
test the expected storage size
*/
TEST(BitTest, sizes) {
   pearlrt::BitString<3> y3;
   pearlrt::BitString<9> y9;
   pearlrt::BitString<17> y15;
   pearlrt::BitString<32> y32;
   pearlrt::BitString<33> y33;
   EXPECT_EQ(sizeof(y3), 1);
   EXPECT_EQ(sizeof(y9), 2);
   EXPECT_EQ(sizeof(y15), 4);
   EXPECT_EQ(sizeof(y32), 4);
   EXPECT_EQ(sizeof(y33), 8);
}

/**
 test the shift operations
*/
TEST(BitTest, Shift) {
   pearlrt::BitString<3> x3(1) , y3(4), z3, help;
   EXPECT_FALSE((x3 == y3).getBoolean());
   help = x3.bitShift(2);
   EXPECT_TRUE((help == y3).getBoolean());
   help = help.bitShift(1);
   EXPECT_TRUE((help == z3).getBoolean());
   help = pearlrt::BitString<3>(4);  // 100
   help = help.bitShift(-2);
   EXPECT_TRUE((help == x3).getBoolean());
   help = pearlrt::BitString<3>(4);  // 100
   help = help.bitShift(-3);
   EXPECT_TRUE((help == z3).getBoolean());
}

/**
 test the circular shift operations
*/
TEST(BitTest, cshifttests) {
   pearlrt::BitString<3> x3(1) , y3(4), z3, help;
   help = x3.bitCshift(2);
   EXPECT_TRUE((help == y3).getBoolean());
   help = x3.bitCshift(3);
   EXPECT_TRUE((help == x3).getBoolean());
   help = x3.bitCshift(-1);
   EXPECT_TRUE((help == y3).getBoolean());
   help = x3.bitCshift(-3);
   EXPECT_TRUE((help == x3).getBoolean());
}

/**
 test the logic operations
*/
TEST(BitTest, Logic) {
   pearlrt::BitString<3> x3(1);
   pearlrt::BitString<9> y9(0x01ff);
   pearlrt::BitString<9> z, zz;
   y9 = pearlrt::BitString<9>(0x01ff);
   x3 = pearlrt::BitString<3>(0x01);
   z = y9.bitAnd(x3); //   bitAnd(y9,x3);
   EXPECT_TRUE((z == (pearlrt::BitString<9>)(0x040)).getBoolean());
   y9 = pearlrt::BitString<9>(0x01ff);
   x3 = pearlrt::BitString<3>(0x01);
   z = x3.bitAnd(y9);
   EXPECT_TRUE((z == pearlrt::BitString<9>(0x040)).getBoolean());
   y9 = pearlrt::BitString<9>(0x0100);
   x3 = pearlrt::BitString<3>(0x01);
   z = y9.bitOr(x3);
   EXPECT_TRUE((z == (pearlrt::BitString<9>)(0x140)).getBoolean());
   y9 = pearlrt::BitString<9>(0x01);
   x3 = pearlrt::BitString<3>(0x01);
   z = y9.bitOr(x3);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x41))) .getBoolean());
   y9 = pearlrt::BitString<9>(0x01);
   x3 = pearlrt::BitString<3>(0x01);
   z = x3.bitOr(y9);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x41))).getBoolean());
   y9 = pearlrt::BitString<9>(0x0140);
   x3 = pearlrt::BitString<3>(0x01);
   z = y9.bitXor(x3);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x100))).getBoolean());
   y9 = pearlrt::BitString<9>(0x0140);
   x3 = pearlrt::BitString<3>(0x01);
   z = x3.bitXor(y9);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x100))).getBoolean());
   y9 = pearlrt::BitString<9>(0x01);
   x3 = pearlrt::BitString<3>(0x01);
   z = y9.bitXor(x3);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x041))).getBoolean());
   y9 = pearlrt::BitString<9>(0x01);
   x3 = pearlrt::BitString<3>(0x01);
   z = x3.bitXor(y9);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x041))).getBoolean());
}

/**
test concatenation
*/
TEST(BitTests, Cat) {
   pearlrt::BitString<3> x3;
   pearlrt::BitString<6> y6;
   pearlrt::BitString<9> z, zz;
   y6 = pearlrt::BitString<6>(0x03f);
   x3 = pearlrt::BitString<3>(0x01);
   z = x3.bitCat(y6);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x07f))).getBoolean());
   y6 = pearlrt::BitString<6>(0x03f);
   x3 = pearlrt::BitString<3>(0x01);
   z = y6.bitCat(x3);
   EXPECT_TRUE((z == (pearlrt::BitString<9>(0x1f9))).getBoolean());
}

/**
test toBit function
*/
TEST(BitTests, toBit) {
   pearlrt::BitString<3> x3;
   pearlrt::Fixed<3> f;
   pearlrt::Fixed<7> f7;
   f = 2;
   x3 = pearlrt::BitString<3>(f.x);
   EXPECT_TRUE((x3 == (pearlrt::BitString<3>(0x02))).getBoolean());
}

/**
test bit slice  functions
*/
TEST(BitTests, BitSlice) {
   pearlrt::BitString<16> x36;
   pearlrt::BitString<6> x;
   pearlrt::BitString<2> slice;
   x = pearlrt::BitString<6>(0x05);
   slice = pearlrt::BitString<2>(3);
   x.setSlice(1, slice);
   EXPECT_TRUE((x == (pearlrt::BitString<6>)0x35).getBoolean());
   x = pearlrt::BitString<6>(0x05);
   slice = pearlrt::BitString<2>(3);
   x.setSlice(2, slice);
   EXPECT_TRUE((x == (pearlrt::BitString<6>)0x1d).getBoolean());
   x36 = pearlrt::BitString<16>(0x05);
   slice = pearlrt::BitString<2>(3);
   x36.setSlice(2, slice);
   EXPECT_TRUE((x36 == (pearlrt::BitString<16>)0x06005).getBoolean());
   ASSERT_THROW(x.setSlice(0, slice), pearlrt::BitIndexOutOfRangeSignal);
   ASSERT_THROW(x.setSlice(5, slice), pearlrt::BitIndexOutOfRangeSignal);
   ASSERT_NO_THROW(x.setSlice(4, slice));
   x36 = pearlrt::BitString<16>(0x05);
   slice = x36.getSlice(15, slice);
   EXPECT_TRUE((slice == (pearlrt::BitString<2>)0x1).getBoolean());
   EXPECT_TRUE((x36.getSlice(14, slice) == (pearlrt::BitString<2>)0x2)
               .getBoolean());
   ASSERT_THROW(x36.getSlice(0, slice), pearlrt::BitIndexOutOfRangeSignal);
   ASSERT_THROW(x36.getSlice(16, slice), pearlrt::BitIndexOutOfRangeSignal);
   x36 = pearlrt::BitString<16>(0x05);
   ASSERT_THROW(x36.getBit(17) , pearlrt::BitIndexOutOfRangeSignal);
   ASSERT_THROW(x36.getBit(0) , pearlrt::BitIndexOutOfRangeSignal);
   ASSERT_THROW(x36.getBit(-1) , pearlrt::BitIndexOutOfRangeSignal);
   ASSERT_TRUE((x36.getBit(16) == (pearlrt::BitString<1>)1).getBoolean());
   ASSERT_TRUE((x36.getBit(15) == (pearlrt::BitString<1>)0).getBoolean());
   ASSERT_TRUE((x36.getBit(14) == (pearlrt::BitString<1>)1).getBoolean());
   x36.setBit(15, (pearlrt::BitString<1>)1);
   ASSERT_TRUE((x36.getBit(15) == (pearlrt::BitString<1>)1).getBoolean());
}

/**
test TOFIXED conversion
*/
TEST(BitTest, toFixed) {
   pearlrt::BitString<6> x6;
   pearlrt::Fixed<6> f6, h6;
   pearlrt::BitString<17> x17;
   pearlrt::Fixed<17> f17, h17;

   for (f6 = 0; (f6 < (pearlrt::Fixed<6>)63).getBoolean();
         f6 = f6 + (pearlrt::Fixed<6>)1) {
      x6 = (pearlrt::BitString<6>)f6.x;
      h6 = x6.toFixed();
      ASSERT_TRUE((f6 == h6).getBoolean());
   }

   for (f17 = 0;
         (f17 < (pearlrt::Fixed<17>)129072).getBoolean();
         f17 = f17 + (pearlrt::Fixed<17>)1) {
      x17 = (pearlrt::BitString<17>)f17.x;
      h17 = x17.toFixed();
      ASSERT_TRUE((f17 == h17).getBoolean());
   }
}

/**
test comparison
*/
TEST(BitTest, comparison) {
   pearlrt::BitString<16> a16(0x4000);
   pearlrt::BitString<16> b16(0x8000);
   EXPECT_FALSE((a16==b16).getBoolean());
   EXPECT_TRUE((a16!=b16).getBoolean());
}
/**
\endcond
*/
