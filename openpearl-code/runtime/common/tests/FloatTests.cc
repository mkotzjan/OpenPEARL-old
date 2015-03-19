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

\brief module tests for Float<S> class

These tests use google test frame to verify the proper operation of the methode
implemented in Float.h
*/

#include "gtest.h"

#include "Signals.h"
#include "Float.h"
#include "Fixed.h"
#include "compare.h"



/**
Unit tests for sizes
*/
TEST(Float, Sizes) {
   EXPECT_EQ(sizeof((pearlrt::Float<24>)1), 4);
   EXPECT_EQ(sizeof((pearlrt::Float<53>)1), 8);
}

/**
Unit tests for  operations with Float(24) operands
*/
TEST(Float, Float_24) {
   pearlrt::Float<24> x(1);
   pearlrt::Float<24> y, z;

   ASSERT_THROW(y + y, pearlrt::FloatIsNaNSignal);
   x = (pearlrt::Float<24>)2;
   y = (pearlrt::Float<24>)4;
   z = -y;
   EXPECT_EQ(z.x, -4);
   z = x + y;
   EXPECT_EQ(z.x, 6);
   z = x - y;
   EXPECT_EQ(z.x, -2);
   z = x * y;
   EXPECT_EQ(z.x, 8);
   z = x / y;
   EXPECT_EQ(z.x, 0.5);
   x = (pearlrt::Float<24>)2;
   y = (pearlrt::Float<24>)0;
   ASSERT_THROW(x / y, pearlrt::FloatIsINFSignal);
}

/**
Unit tests for  operations with Float(53) operands
*/
TEST(Float, Float_53) {
   pearlrt::Float<53> x(1);
   pearlrt::Float<53> y, z;

   ASSERT_THROW(y + y, pearlrt::FloatIsNaNSignal);

   x = (pearlrt::Float<53>)2;
   y = (pearlrt::Float<53>)4;
   z = -y;
   EXPECT_EQ(z.x, -4);
   z = x + y;
   EXPECT_EQ(z.x, 6);
   z = x - y;
   EXPECT_EQ(z.x, -2);
   z = x * y;
   EXPECT_EQ(z.x, 8);
   z = x / y;
   EXPECT_EQ(z.x, 0.5);
   x = (pearlrt::Float<53>)2;
   y = (pearlrt::Float<53>)0;
   ASSERT_THROW(x / y, pearlrt::FloatIsINFSignal);

   pearlrt::Float<24> u;
   u = x.fit(u);
   EXPECT_EQ(u.x, 2);

   x = (pearlrt::Float<53>)1e100;
   ASSERT_THROW(x.fit(u), pearlrt::FloatIsINFSignal);

}

/**
Unit tests for mixed operations  with Float(24) and Float(53)
*/
TEST(Float, MixedOperations) {
   pearlrt::Float<24> x(1);
   pearlrt::Float<53> y, z;

   x = (pearlrt::Float<24>)2;
   y = (pearlrt::Float<53>)4;
   z = -x;
   EXPECT_EQ(z.x, -2);
   z = x + y;
   EXPECT_EQ(z.x, 6);
   z = x - y;
   EXPECT_EQ(z.x, -2);
   z = x * y;
   EXPECT_EQ(z.x, 8);
   z = x / y;
   EXPECT_EQ(z.x, 0.5);
   x = (pearlrt::Float<24>)2;
   y = (pearlrt::Float<53>)0;
   ASSERT_THROW(x / y, pearlrt::FloatIsINFSignal);

   x = (pearlrt::Float<24>)4;
   y = (pearlrt::Float<53>)2;
   z = y + x;
   EXPECT_EQ(z.x, 6);
   z = y - x;
   EXPECT_EQ(z.x, -2);
   z = y * x;
   EXPECT_EQ(z.x, 8);
   z = y / x;
   EXPECT_EQ(z.x, 0.5);
   x = (pearlrt::Float<24>)0;
   y = (pearlrt::Float<53>)2;
   ASSERT_THROW(y / x, pearlrt::FloatIsINFSignal);
}

/**
Unit tests for  compare operations  with Float(24)
*/
TEST(Float, CompareOperations_24) {
   pearlrt::Float<24> x(1), y(2);

   EXPECT_TRUE((x == x).getBoolean());
   EXPECT_FALSE((x == y).getBoolean());
   EXPECT_FALSE((x != x).getBoolean());
   EXPECT_TRUE((x != y).getBoolean());
   EXPECT_TRUE((x < y).getBoolean());
   EXPECT_FALSE((y < x).getBoolean());
   EXPECT_TRUE((x <= y).getBoolean());
   EXPECT_TRUE((x <= x).getBoolean());
   EXPECT_FALSE((y <= x).getBoolean());
   EXPECT_FALSE((x > y).getBoolean());
   EXPECT_TRUE((y > x).getBoolean());
   EXPECT_FALSE((x >= y).getBoolean());
   EXPECT_TRUE((y >= x).getBoolean());
   EXPECT_TRUE((x >= x).getBoolean());
}

/**
Unit tests for  compare operations  with Float(53)
*/
TEST(Float, CompareOperations_53) {
   pearlrt::Float<53> x(1), y(2);

   EXPECT_TRUE((x == x).getBoolean());
   EXPECT_FALSE((x == y).getBoolean());
   EXPECT_FALSE((x != x).getBoolean());
   EXPECT_TRUE((x != y).getBoolean());
   EXPECT_TRUE((x < y).getBoolean());
   EXPECT_FALSE((y < x).getBoolean());
   EXPECT_TRUE((x <= y).getBoolean());
   EXPECT_TRUE((x <= x).getBoolean());
   EXPECT_FALSE((y <= x).getBoolean());
   EXPECT_FALSE((x > y).getBoolean());
   EXPECT_TRUE((y > x).getBoolean());
   EXPECT_FALSE((x >= y).getBoolean());
   EXPECT_TRUE((y >= x).getBoolean());
   EXPECT_TRUE((x >= x).getBoolean());
}

/**
Unit tests for  compare operations  with Float(24) and Float(53)
*/
TEST(Float, CompareOperations_24_53) {
   pearlrt::Float<24> x(1);
   pearlrt::Float<53> y(2);

   EXPECT_TRUE((x == x).getBoolean());
   EXPECT_FALSE((x == y).getBoolean());
   EXPECT_FALSE((x != x).getBoolean());
   EXPECT_TRUE((x != y).getBoolean());
   EXPECT_TRUE((x < y).getBoolean());
   EXPECT_FALSE((y < x).getBoolean());
   EXPECT_TRUE((x <= y).getBoolean());
   EXPECT_TRUE((x <= x).getBoolean());
   EXPECT_FALSE((y <= x).getBoolean());
   EXPECT_FALSE((x > y).getBoolean());
   EXPECT_TRUE((y > x).getBoolean());
   EXPECT_FALSE((x >= y).getBoolean());
   EXPECT_TRUE((y >= x).getBoolean());
   EXPECT_TRUE((x >= x).getBoolean());
}

/**
Unit tests for  compare operations  with Float(53) and Float(24)
*/
TEST(Float, CompareOperations_53_24) {
   pearlrt::Float<53> x(1);
   pearlrt::Float<24> y(2);

   EXPECT_TRUE((x == x).getBoolean());
   EXPECT_FALSE((x == y).getBoolean());
   EXPECT_FALSE((x != x).getBoolean());
   EXPECT_TRUE((x != y).getBoolean());
   EXPECT_TRUE((x < y).getBoolean());
   EXPECT_FALSE((y < x).getBoolean());
   EXPECT_TRUE((x <= y).getBoolean());
   EXPECT_TRUE((x <= x).getBoolean());
   EXPECT_FALSE((y <= x).getBoolean());
   EXPECT_FALSE((x > y).getBoolean());
   EXPECT_TRUE((y > x).getBoolean());
   EXPECT_FALSE((x >= y).getBoolean());
   EXPECT_TRUE((y >= x).getBoolean());
   EXPECT_TRUE((x >= x).getBoolean());
}

/**
Unit tests with math functions
*/
TEST(Float, math_24) {
   pearlrt::Float<24> x(0);
   pearlrt::Float<24> y(0);
   pearlrt::Float<24> z(0);
   pearlrt::Fixed<31> f;
   pearlrt::Float<24> epsilon(5.0e-6);

   x = (pearlrt::Float<24>) - 10.4;
   y = (pearlrt::Float<24>)10.4;
   EXPECT_TRUE((x.abs() == y).getBoolean());
   EXPECT_TRUE((x.sign() == pearlrt::Fixed<15>(-1)).getBoolean());

   x = (pearlrt::Float<24>)10.4;
   y = (pearlrt::Float<24>)10.4;
   EXPECT_TRUE((x.abs() == y).getBoolean());
   EXPECT_TRUE((x.sign() == pearlrt::Fixed<15>(1)).getBoolean());

   x = (pearlrt::Float<24>)0;
   y = (pearlrt::Float<24>)0;
   EXPECT_TRUE((x.abs() == y).getBoolean());
   EXPECT_TRUE((x.sign() == pearlrt::Fixed<15>(0)).getBoolean());

   x = (pearlrt::Float<24>)10.0;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(10)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(10)).getBoolean());

   x = (pearlrt::Float<24>)10.4;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(10)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(10)).getBoolean());

   x = (pearlrt::Float<24>)10.5;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(10)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(11)).getBoolean());

   x = (pearlrt::Float<24>) - 10.4;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(-11)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(-10)).getBoolean());

   x = (pearlrt::Float<24>) - 10.5;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(-11)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(-11)).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 1;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>)0;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>)1;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>)2;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>) - 2;
   y = (pearlrt::Float<24>)0.25;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>) - 3;
   y = (pearlrt::Float<24>) - 0.125;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>)3;
   y = (pearlrt::Float<24>) - 8;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());


   for (double phi = 0; phi < 6.3; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.sin() * x.sin() + x.cos() * x.cos() - (pearlrt::Float<24>)1;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -3.0; phi < 3.0; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.sin() / x.cos() - x.tan();
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -1.5; phi < 1.5; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.tan().atan() - x;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -10.0; phi < 10.; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.exp().ln() - x;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -10.0; phi < 10.; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.tanh() -
          ((pearlrt::Float<24>)(1) -
           (pearlrt::Float<24>)(2) /
           ((x + x).exp() + (pearlrt::Float<24>)1));
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = 0.0; phi < 1000.; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = (x * x).sqrt() - x;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   {
      x = (pearlrt::Float<24>)(-1.0);
      EXPECT_THROW(x.ln(), pearlrt::FloatIsNaNSignal);
      EXPECT_THROW(x.sqrt(), pearlrt::FloatIsNaNSignal);
   }

}


/**
Unit tests with math functions

Note that the internal assignments are done with Float<24> -> Float<53>.
This is without problems, since the data are small and exact enough
*/
TEST(Float, math_53) {
   pearlrt::Float<53> x(0);
   pearlrt::Float<53> y(0);
   pearlrt::Float<53> z(0);
   pearlrt::Fixed<31> f;
   pearlrt::Float<53> epsilon(5.0e-12);

   x = (pearlrt::Float<24>) - 10.4;
   y = (pearlrt::Float<24>)10.4;
   EXPECT_TRUE((x.abs() == y).getBoolean());
   EXPECT_TRUE((x.sign() == pearlrt::Fixed<15>(-1)).getBoolean());

   x = (pearlrt::Float<24>)10.4;
   y = (pearlrt::Float<24>)10.4;
   EXPECT_TRUE((x.abs() == y).getBoolean());
   EXPECT_TRUE((x.sign() == pearlrt::Fixed<15>(1)).getBoolean());

   x = (pearlrt::Float<24>)0;
   y = (pearlrt::Float<24>)0;
   EXPECT_TRUE((x.abs() == y).getBoolean());
   EXPECT_TRUE((x.sign() == pearlrt::Fixed<15>(0)).getBoolean());

   x = (pearlrt::Float<24>)10.0;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(10)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(10)).getBoolean());

   x = (pearlrt::Float<24>)10.4;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(10)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(10)).getBoolean());

   x = (pearlrt::Float<24>)10.5;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(10)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(11)).getBoolean());

   x = (pearlrt::Float<24>) - 10.4;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(-11)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(-10)).getBoolean());

   x = (pearlrt::Float<24>) - 10.5;
   EXPECT_TRUE((x.entier() == pearlrt::Fixed<15>(-11)).getBoolean());
   EXPECT_TRUE((x.round() == pearlrt::Fixed<15>(-11)).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 1;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>)0;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>)1;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>)2;
   f = (pearlrt::Fixed<31>)0;
   y = (pearlrt::Float<24>)1;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>) - 2;
   y = (pearlrt::Float<24>)0.25;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>) - 3;
   y = (pearlrt::Float<24>) - 0.125;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());

   x = (pearlrt::Float<24>) - 2;
   f = (pearlrt::Fixed<31>)3;
   y = (pearlrt::Float<24>) - 8;
   EXPECT_TRUE((x.pow(f) == y).getBoolean());


   for (double phi = 0; phi < 6.3; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.sin() * x.sin() + x.cos() * x.cos() - (pearlrt::Float<24>)1;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -3.0; phi < 3.0; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.sin() / x.cos() - x.tan();
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -1.5; phi < 1.5; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.tan().atan() - x;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -10.0; phi < 10.; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.exp().ln() - x;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = -10.0; phi < 10.; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = x.tanh() -
          ((pearlrt::Float<24>)(1) -
           (pearlrt::Float<24>)(2) /
           ((x + x).exp() + (pearlrt::Float<24>)1));
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   for (double phi = 0.0; phi < 1000.; phi += 0.01) {
      x = (pearlrt::Float<24>)(phi);
      z = (x * x).sqrt() - x;
      EXPECT_TRUE((z.abs() < epsilon).getBoolean());
   }

   {
      x = (pearlrt::Float<24>)(-1.0);
      EXPECT_THROW(x.ln(), pearlrt::FloatIsNaNSignal);
      EXPECT_THROW(x.sqrt(), pearlrt::FloatIsNaNSignal);
   }
}

/**
Float + Fixed operations
*/
TEST(Float, fixed) {
   pearlrt::Float<24> x24(1);
   pearlrt::Float<53> x53(1);
   pearlrt::Fixed<15> y(2);
   pearlrt::Float<24> z24(3);
   pearlrt::Float<53> z53(3);
   pearlrt::Fixed<31> y31(2);

   z24 = x24 + y;
   EXPECT_EQ(z24.x, 3);
   z24 = y + x24;
   EXPECT_EQ(z24.x, 3);
   z53 = x53 + y;
   EXPECT_EQ(z53.x, 3);
   z53 = y + x53;
   EXPECT_EQ(z53.x, 3);
   z53 = x24 - y31;
   z53 = x53 - y31;
}

