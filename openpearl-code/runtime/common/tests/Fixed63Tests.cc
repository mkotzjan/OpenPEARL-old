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

\brief FIXED63 test program

\page Testprograms

\section fixed63_test

This program checks whether the Fixed63 class delivers the expected
results.
The program tests all constructors and all operations.
It compares the real results with the expected results.
This includes the exception generation.

All individual tests are logged to the standard out channel together with
a rating of the result.
In case that all tests pass the return value is 0.

\note The type Fixed63 is only for internal use in the PEARL run time system

\cond TREAT_EXAMPLES

*/
#include <stdlib.h>
#include "gtest.h"

using namespace std;

#include "Fixed63.h"
#include "Signals.h"

using namespace pearlrt;

TEST(Fixed63, AllTests) {
   Fixed63 x((Fixed63::Fixed63_t)0x7fffffffffffffffLL),
           y((Fixed63::Fixed63_t)1LL),
           z((Fixed63::Fixed63_t)0LL);
   double dx;
   ASSERT_EQ(sizeof(Fixed63::Fixed63_t), 8);
   /* maxint +1 */
   x = (Fixed63::Fixed63_t)0x7fffffffffffffffLL;
   ASSERT_THROW(x += y, ArithmeticOverflowSignal);
   /* minint -1 */
   x = (Fixed63::Fixed63_t)0x8000000000000000LL;
   ASSERT_THROW(x -= y, ArithmeticUnderflowSignal);
   /* cout << "MinInt + (-1): "; */
   x = (Fixed63::Fixed63_t)0x8000000000000000LL;
   y = (Fixed63::Fixed63_t) - 1LL;
   ASSERT_THROW(x += y, ArithmeticUnderflowSignal);
   /* cout << "MaxInt - (-1): "; */
   x = (Fixed63::Fixed63_t)0x7fffffffffffffffLL;
   y = (Fixed63::Fixed63_t) - 1LL;
   ASSERT_THROW(x -= y, ArithmeticOverflowSignal);
   /* cout << "10/0 "; */
   x = (Fixed63::Fixed63_t)10LL;
   ASSERT_THROW(x /= (Fixed63::Fixed63_t)0, ArithmeticDivideByZeroSignal);
   /* cout << "-MinInt "; */
   x = (Fixed63::Fixed63_t)0x8000000000000000LL;
   ASSERT_THROW(-x, ArithmeticOverflowSignal);
   /*cout << "MinInt /( -1) "; */
   x = (Fixed63::Fixed63_t)0x8000000000000000LL;
   ASSERT_THROW(x / (Fixed63::Fixed63_t) - 1, ArithmeticOverflowSignal);
   /* cout << "MinInt%-1 "; */
   x = (Fixed63::Fixed63_t)0x8000000000000000LL;
   y = (Fixed63::Fixed63_t) - 1;
   x %= y;
   ASSERT_EQ(x.get(), 0);
   /* cout << "10%0 "; */
   x = (Fixed63::Fixed63_t)10;
   ASSERT_THROW(x %= (Fixed63::Fixed63_t)0, ArithmeticDivideByZeroSignal);
   /* cout << "modulo operations  " << endl;; */
   x = (Fixed63::Fixed63_t)5;
   y = (Fixed63::Fixed63_t)3;
   z = x % y;
   ASSERT_EQ(z.get(), 2);
   x = (Fixed63::Fixed63_t) - 5;
   y = (Fixed63::Fixed63_t)3;
   z = x % y;
   ASSERT_EQ(z.get(), -2);
   x = (Fixed63::Fixed63_t)5;
   y = (Fixed63::Fixed63_t) - 3;
   z = x % y;
   ASSERT_EQ(z.get(), 2);
   x = (Fixed63::Fixed63_t) - 5;
   y = (Fixed63::Fixed63_t) - 3;
   z = x % y;
   ASSERT_EQ(z.get(), -2);
   /* cout << "Multiplication " << endl; */
   y = (Fixed63::Fixed63_t)0x800000000000000LL;
   x = (Fixed63::Fixed63_t) - 16;
   ASSERT_THROW(z = x * y, ArithmeticOverflowSignal);
   x = (Fixed63::Fixed63_t)0x800000000000000LL;
   y = (Fixed63::Fixed63_t) - 16;
   ASSERT_THROW(z = x * y, ArithmeticOverflowSignal);
   y = (Fixed63::Fixed63_t)0x800000000000000LL;
   x = (Fixed63::Fixed63_t)16;
   ASSERT_THROW(z = x * y, ArithmeticOverflowSignal);
   x = (Fixed63::Fixed63_t)0x800000000000000LL;
   y = (Fixed63::Fixed63_t)16;
   ASSERT_THROW(z = x * y, ArithmeticOverflowSignal);
   x = (Fixed63::Fixed63_t)123456789012LL;
   y = (Fixed63::Fixed63_t)10;
   ASSERT_NO_THROW(z = x * y);
   ASSERT_EQ(z.get(), 1234567890120LL);
   y = (Fixed63::Fixed63_t)123456789012LL;
   x = (Fixed63::Fixed63_t)10;
   ASSERT_NO_THROW(z = x * y);
   ASSERT_EQ(z.get(), 1234567890120LL);
   x = (Fixed63::Fixed63_t)123;
   y = (Fixed63::Fixed63_t)10;
   ASSERT_NO_THROW(x * y);
   ASSERT_EQ((x * y).get(), 1230);
   /* cout << "Float constructor" << endl; */
   /* cout << "   MaxInt + 1: "; */
   dx = 0x7fffffffffffffffLL;
   dx += 1 ;// 1010;
   ASSERT_THROW(x = Fixed63(dx), ArithmeticOverflowSignal);
   /* cout << "   MinInt - 1: "; */
   dx = 0x8000000000000000LL;
   dx = -dx;
   dx -= 2000 ;// 1010;
   ASSERT_THROW(x = Fixed63(dx), ArithmeticUnderflowSignal);
   /* cout << "    compare: " << endl; */
   x = Fixed63((Fixed63::Fixed63_t)10LL);
   y = x + (Fixed63::Fixed63_t)1LL;
   ASSERT_TRUE(x.compare(y) < 0);
   x = Fixed63((Fixed63::Fixed63_t)10LL);
   y = x;
   ASSERT_TRUE(x.compare(y) == 0);
   x = Fixed63((Fixed63::Fixed63_t)10LL);
   y = x - (Fixed63::Fixed63_t)1LL;
   ASSERT_TRUE(x.compare(y) > 0);
}

/**
\endcond
*/
