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

\brief module tests for Fixed<S> class

These tests use google test frame to verify the proper operation of the methode
implemented in Fixed.h
*/

#include <inttypes.h>  // PRIx64
#include "gtest.h"

#include "Signals.h"
#include "Fixed.h"
#include "compare.h"


/**
Unit tests for fixed sizes
*/
TEST(Fixed, Sizes) {
   EXPECT_EQ(sizeof((pearlrt::Fixed<0>)0), 1);
   EXPECT_EQ(sizeof((pearlrt::Fixed<1>)1), 1);
   EXPECT_EQ(sizeof((pearlrt::Fixed<8>)1), 2);
   EXPECT_EQ(sizeof((pearlrt::Fixed<16>)1), 4);
   EXPECT_EQ(sizeof((pearlrt::Fixed<32>)1), 8);
}

/**
Unit tests for fixed value ranges at the constructor
*/
TEST(Fixed, RangesCreation) {
   ASSERT_NO_THROW((pearlrt::Fixed<0>)0);
   ASSERT_NO_THROW((pearlrt::Fixed<0>)-1);
   ASSERT_THROW((pearlrt::Fixed<0>)1, pearlrt::FixedRangeSignal);
   ASSERT_NO_THROW((pearlrt::Fixed<5>)31);
   ASSERT_THROW((pearlrt::Fixed<5>)32, pearlrt::FixedRangeSignal);
   ASSERT_NO_THROW((pearlrt::Fixed<15>)32767);
   ASSERT_THROW((pearlrt::Fixed<15>)32768, pearlrt::FixedRangeSignal);
   ASSERT_NO_THROW((pearlrt::Fixed<31>)(1LL << 30));
   ASSERT_THROW((pearlrt::Fixed<31>)(1LL << 31), pearlrt::FixedRangeSignal);
   ASSERT_NO_THROW((pearlrt::Fixed<32>)(1LL << 31));
   ASSERT_THROW((pearlrt::Fixed<32>)(1LL << 32), pearlrt::FixedRangeSignal);
   ASSERT_NO_THROW((pearlrt::Fixed<63>)(1LL << 62));
}


/**
Unit tests for  operations with Fixed(3) and Fixed(5) operands
*/
TEST(Fixed, Fixed_3_5_Operations) {
   pearlrt::Fixed<3> x(1);
   pearlrt::Fixed<5> y, z;
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)6;
   y = y + x;
   EXPECT_EQ(y.x, 8);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)6;
   y = x + y;
   EXPECT_EQ(y.x, 8);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)31;
   ASSERT_THROW(y + x, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)31;
   ASSERT_NO_THROW(y = x);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)31;
   ASSERT_THROW(x = y, pearlrt::FixedRangeSignal);
   // substraction
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)31;
   ASSERT_NO_THROW(y - x);
   z = y - x;
   EXPECT_EQ(z.x, 29);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>) - 31;
   ASSERT_THROW(y - x, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>) - 29;
   ASSERT_NO_THROW(y - x);
   z = y - x;
   EXPECT_EQ(z.x, -31);
   // monadic minus
   y = (pearlrt::Fixed<5>)2;
   ASSERT_NO_THROW(-y);
   z = -y;
   EXPECT_EQ(z.x, -2);
   y = (pearlrt::Fixed<5>)(-32);
   ASSERT_THROW(-y, pearlrt::FixedRangeSignal);
   // multiply
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)6;
   y = x * y;
   EXPECT_EQ(y.x, 12);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)16;
   ASSERT_THROW(y * x, pearlrt::FixedRangeSignal);
   ASSERT_THROW(x * y, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<3>) - 1;
   y = (pearlrt::Fixed<5>) - 32;
   ASSERT_THROW(y * x, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<3>) - 2;
   y = (pearlrt::Fixed<5>)16;
   ASSERT_NO_THROW(y * x);
   y = y * x;
   EXPECT_EQ(y.x, -32);
   // divide
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)6;
   y = y / x;
   EXPECT_EQ(y.x, 3);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)6;
   y = x / y;
   EXPECT_EQ(y.x, 0);
   x = (pearlrt::Fixed<3>) - 1;
   y = (pearlrt::Fixed<5>)(-32);
   ASSERT_THROW(y / x, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<3>)0;
   y = (pearlrt::Fixed<5>)6;
   ASSERT_THROW(y / x, pearlrt::FixedDivideByZeroSignal);
   // modulo operation
   x = (pearlrt::Fixed<3>)3;
   y = (pearlrt::Fixed<5>)5;
   z = y % x;
   EXPECT_EQ(z.x, 2);
   x = (pearlrt::Fixed<3>)3;
   y = (pearlrt::Fixed<5>) - 5;
   z = y % x;
   EXPECT_EQ(z.x, -2);
   x = (pearlrt::Fixed<3>) - 3;
   y = (pearlrt::Fixed<5>)5;
   z = y % x;
   EXPECT_EQ(z.x, 2);
   x = (pearlrt::Fixed<3>) - 3;
   y = (pearlrt::Fixed<5>) - 5;
   z = y % x;
   EXPECT_EQ(z.x, -2);
   x = (pearlrt::Fixed<3>)0;
   y = (pearlrt::Fixed<5>) - 10;
   ASSERT_THROW(y / x, pearlrt::FixedDivideByZeroSignal);
   // pow
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)2;
   EXPECT_EQ(x.pow(y).x, 4);
   x = (pearlrt::Fixed<3>) - 2;
   y = (pearlrt::Fixed<5>)2;
   EXPECT_EQ(x.pow(y).x, 4);
   x = (pearlrt::Fixed<3>)1;
   y = (pearlrt::Fixed<5>) - 2;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<3>) - 1;
   y = (pearlrt::Fixed<5>) - 2;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<3>)1;
   y = (pearlrt::Fixed<5>) - 3;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>) - 2;
   EXPECT_EQ(x.pow(y).x, 0);
   x = (pearlrt::Fixed<3>)4;
   y = (pearlrt::Fixed<5>)0;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<3>) - 4;
   y = (pearlrt::Fixed<5>)0;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<3>)0;
   y = (pearlrt::Fixed<5>)0;
   ASSERT_THROW(x.pow(y).x,
                pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)2;
   ASSERT_NO_THROW(x.pow(y));
   x = (pearlrt::Fixed<3>)2;
   y = (pearlrt::Fixed<5>)3;
   ASSERT_THROW(x.pow(y), pearlrt::FixedRangeSignal);
}


/**
Unit tests for  operations with Fixed(30) and Fixed(50) operands
*/
TEST(Fixed, Fixed_30_50_Operations) {
   pearlrt::Fixed<30> x;
   pearlrt::Fixed<50> y, z;
   int32_t large30 = 0x3fffffffL;
   int64_t large50 = 0x3ffffffffffffLL;
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)6;
   y = y + x;
   EXPECT_EQ(y.x, 8);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)6;
   y = x + y;
   EXPECT_EQ(y.x, 8);
   x = (pearlrt::Fixed<30>)1;
   y = (pearlrt::Fixed<50>)large50;
   ASSERT_THROW(x + y, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<30>)1;
   y = (pearlrt::Fixed<50>)large50;
   ASSERT_THROW(y + x, pearlrt::FixedRangeSignal);
   y = (pearlrt::Fixed<50>)large30;
   ASSERT_NO_THROW(x = y);
   y = (pearlrt::Fixed<50>)large50;
   ASSERT_THROW(x = y, pearlrt::FixedRangeSignal);
   // substraction
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)31;
   ASSERT_NO_THROW(y - x);
   z = y - x;
   EXPECT_EQ(z.x, 29);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)(-large50);
   ASSERT_THROW(y - x, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)(-large50 + 2);
   ASSERT_NO_THROW(y - x);
   z = y - x;
   EXPECT_EQ(z.x, -large50);
   // monadic minus
   y = (pearlrt::Fixed<50>)2;
   ASSERT_NO_THROW(-y);
   z = -y;
   EXPECT_EQ(z.x, -2);
   y = (pearlrt::Fixed<50>)(-large50 - 1);
   ASSERT_THROW(-y, pearlrt::FixedRangeSignal);
   // multiply
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)6;
   y = x * y;
   EXPECT_EQ(y.x, 12);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)(1LL << 48);
   ASSERT_NO_THROW(y * x);
   ASSERT_NO_THROW(x * y);
   x = (pearlrt::Fixed<30>) - 1;
   y = (pearlrt::Fixed<50>)(-large50 - 1);
   ASSERT_THROW(y * x, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)(large50);
   ASSERT_THROW(y * x, pearlrt::FixedRangeSignal);
   ASSERT_THROW(x * y, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<30>) - 2;
   y = (pearlrt::Fixed<50>)(1LL << 49);
   ASSERT_NO_THROW(y * x);
   y = y * x;
   EXPECT_EQ(y.x, (-large50) - 1);
   // divide
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)6;
   y = y / x;
   EXPECT_EQ(y.x, 3);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)6;
   y = x / y;
   EXPECT_EQ(y.x, 0);
   x = (pearlrt::Fixed<30>) - 1;
   y = (pearlrt::Fixed<50>)((-large50) - 1);
   ASSERT_THROW(y / x, pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<30>)0;
   y = (pearlrt::Fixed<50>)6;
   ASSERT_THROW(y / x, pearlrt::FixedDivideByZeroSignal);
   // modulo operation
   x = (pearlrt::Fixed<30>)3;
   y = (pearlrt::Fixed<50>)5;
   z = y % x;
   EXPECT_EQ(z.x, 2);
   x = (pearlrt::Fixed<30>)3;
   y = (pearlrt::Fixed<50>) - 5;
   z = y % x;
   EXPECT_EQ(z.x, -2);
   x = (pearlrt::Fixed<30>) - 3;
   y = (pearlrt::Fixed<50>)5;
   z = y % x;
   EXPECT_EQ(z.x, 2);
   x = (pearlrt::Fixed<30>) - 3;
   y = (pearlrt::Fixed<50>) - 5;
   z = y % x;
   EXPECT_EQ(z.x, -2);
   x = (pearlrt::Fixed<30>)0;
   y = (pearlrt::Fixed<50>) - 10;
   ASSERT_THROW(y / x, pearlrt::FixedDivideByZeroSignal);
   // pow
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)3;
   EXPECT_EQ(x.pow(y).x, 8);
   x = (pearlrt::Fixed<30>) - 2;
   y = (pearlrt::Fixed<50>)3;
   EXPECT_EQ(x.pow(y).x, -8);
   x = (pearlrt::Fixed<30>)1;
   y = (pearlrt::Fixed<50>) - 2;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<30>) - 1;
   y = (pearlrt::Fixed<50>) - 2;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<30>)1;
   y = (pearlrt::Fixed<50>) - 3;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>) - 2;
   EXPECT_EQ(x.pow(y).x, 0);
   x = (pearlrt::Fixed<30>)4;
   y = (pearlrt::Fixed<50>)0;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<30>) - 4;
   y = (pearlrt::Fixed<50>)0;
   EXPECT_EQ(x.pow(y).x, 1);
   x = (pearlrt::Fixed<30>)0;
   y = (pearlrt::Fixed<50>)0;
   ASSERT_THROW(x.pow(y).x,
                pearlrt::FixedRangeSignal);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)29;
   ASSERT_NO_THROW(x.pow(y));
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)30;
   ASSERT_THROW(x.pow(y), pearlrt::FixedRangeSignal);
}

/**
Unit tests for compare operations
*/
TEST(Fixed, Compare) {
   pearlrt::Fixed<30> x;
   pearlrt::Fixed<50> y;
   pearlrt::Fixed<3> a;
   pearlrt::Fixed<5> b;
   // fixed30/50
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)6;

   EXPECT_EQ((x < y).getBoolean(), true);
   EXPECT_EQ((x <= y).getBoolean(), true);
   EXPECT_EQ((x == y).getBoolean(), false);
   EXPECT_EQ((x != y).getBoolean(), true);
   EXPECT_EQ((x >= y).getBoolean(), false);
   EXPECT_EQ((x > y).getBoolean(), false);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)2;
   EXPECT_EQ((x < y).getBoolean(), false);
   EXPECT_EQ((x <= y).getBoolean(), true);
   EXPECT_EQ((x == y).getBoolean(), true);
   EXPECT_EQ((x != y).getBoolean(), false);
   EXPECT_EQ((x >= y).getBoolean(), true);
   EXPECT_EQ((x > y).getBoolean(), false);
   x = (pearlrt::Fixed<30>)2;
   y = (pearlrt::Fixed<50>)1;
   EXPECT_EQ((x < y).getBoolean(), false);
   EXPECT_EQ((x <= y).getBoolean(), false);
   EXPECT_EQ((x == y).getBoolean(), false);
   EXPECT_EQ((x != y).getBoolean(), true);
   EXPECT_EQ((x >= y).getBoolean(), true);
   EXPECT_EQ((x > y).getBoolean(), true);
   // fixed3/5
   a = (pearlrt::Fixed<3>)2;
   b = (pearlrt::Fixed<5>)6;
   EXPECT_EQ((a < b).getBoolean(), true);
   EXPECT_EQ((a <= b).getBoolean(), true);
   EXPECT_EQ((a == b).getBoolean(), false);
   EXPECT_EQ((a != b).getBoolean(), true);
   EXPECT_EQ((a >= b).getBoolean(), false);
   EXPECT_EQ((a > b).getBoolean(), false);
   a = (pearlrt::Fixed<3>)2;
   b = (pearlrt::Fixed<5>)2;
   EXPECT_EQ((a < b).getBoolean(), false);
   EXPECT_EQ((a <= b).getBoolean(), true);
   EXPECT_EQ((a == b).getBoolean(), true);
   EXPECT_EQ((a != b).getBoolean(), false);
   EXPECT_EQ((a >= b).getBoolean(), true);
   EXPECT_EQ((a > b).getBoolean(), false);

   a = (pearlrt::Fixed<3>)2;
   b = (pearlrt::Fixed<5>)1;
   EXPECT_EQ((a < b).getBoolean(), false);
   EXPECT_EQ((a <= b).getBoolean(), false);
   EXPECT_EQ((a == b).getBoolean(), false);
   EXPECT_EQ((a != b).getBoolean(), true);
   EXPECT_EQ((a >= b).getBoolean(), true);
   EXPECT_EQ((a > b).getBoolean(), true);
}

/**
Unit tests for sign and abs
*/
TEST(Fixed, SizeSignAbs) {
   pearlrt::Fixed<15> x;
   x = 0;
   EXPECT_TRUE((x.sign() == (pearlrt::Fixed<15>)0).getBoolean());
   x = -10;
   EXPECT_TRUE((x.sign() == (pearlrt::Fixed<15>)(-1)).getBoolean());
   x = 10;
   EXPECT_TRUE((x.sign() == (pearlrt::Fixed<15>)(1)).getBoolean());
   x = 0;
   EXPECT_TRUE((x.abs() == (pearlrt::Fixed<15>)0).getBoolean());
   x = -10;
   EXPECT_TRUE((x.abs() == (pearlrt::Fixed<15>)(10)).getBoolean());
   x = 10;
   EXPECT_TRUE((x.abs() == (pearlrt::Fixed<15>)(10)).getBoolean());
   x = -32768;
   ASSERT_THROW(x.abs(), pearlrt::FixedRangeSignal);
}

/**
Unit tests for fit
*/
TEST(Fixed, Fit) {
   pearlrt::Fixed<15> x15;
   pearlrt::Fixed<31> x31;
   x15 = 32;
   x31 = x15.fit(x31);
   EXPECT_TRUE((x31 == x15).getBoolean());
   x31 = 32;
   x15 = x31.fit(x15);
   EXPECT_TRUE((x15 == x31).getBoolean());
   x31 = 32768;
   EXPECT_THROW(x31.fit(x15), pearlrt::FixedRangeSignal);
}

/**
Unit tests for assignViaVoidPointer
*/
TEST(Fixed, AssignViaVoidPtr) {
    pearlrt::Fixed<0> x0;
    pearlrt::Fixed<5> x5;
    pearlrt::Fixed<15> x15;
    pearlrt::Fixed<16> x16;
    pearlrt::Fixed<31> x31;
    pearlrt::Fixed<63> x63;

    ASSERT_NO_THROW(
      assignIntToFixedViaVoidPointer(&x0,0,0)
    );

    ASSERT_NO_THROW(
      assignIntToFixedViaVoidPointer(&x0,0,-1)
    );

    ASSERT_THROW(
      assignIntToFixedViaVoidPointer(&x0,0,1),pearlrt::FixedRangeSignal
    );

    ASSERT_THROW(
      assignIntToFixedViaVoidPointer(&x5,5,32),pearlrt::FixedRangeSignal
    );

    ASSERT_THROW(
      assignIntToFixedViaVoidPointer(&x5,5,-33),pearlrt::FixedRangeSignal
    );

    ASSERT_THROW(
      assignIntToFixedViaVoidPointer(&x15,15,32768),pearlrt::FixedRangeSignal
    );

    ASSERT_THROW(
      assignIntToFixedViaVoidPointer(&x16,16,65536),pearlrt::FixedRangeSignal
    );

    ASSERT_THROW(
      assignIntToFixedViaVoidPointer(&x16,16,-65537),pearlrt::FixedRangeSignal
    );

    // since int is 32 bit we need no further overflow tests
    ASSERT_TRUE(sizeof(int)==4);

    // test the values
    assignIntToFixedViaVoidPointer(&x5,5,3);
    ASSERT_EQ(x5.x, 3);
     
    assignIntToFixedViaVoidPointer(&x15,15,3);
    ASSERT_EQ(x15.x, 3);
    assignIntToFixedViaVoidPointer(&x31,31,3);
    ASSERT_EQ(x31.x, 3);
    assignIntToFixedViaVoidPointer(&x31,31,0x0000AAAA);
    ASSERT_EQ(x31.x, 0x0000AAAA);
     
    assignIntToFixedViaVoidPointer(&x31,31,0xAAAA0000);
    ASSERT_EQ(x31.x, 0xAAAA0000);
    
    int64_t i64 = 0xFFFFFFFFAAAA0000ull; 
    assignIntToFixedViaVoidPointer(&x63,63,0xAAAA0000);
    ASSERT_EQ(x63.x, i64);
     
}
