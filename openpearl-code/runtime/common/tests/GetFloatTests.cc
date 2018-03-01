/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

\brief FIXED (FLOAT-values) format reading test program

\page Testprograms

\section fixed_read_test_float

This program checks whether the FLOAT  values are read as the expected.


\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "Fixed.h"
#include "Float.h"
#include "compare.h"
#include "GetFloat.h"
#include "GetUtils.h"
#include "Signals.h"
#include "Character.h"
#include "RefChar.h"
#include "RefCharSource.h"

using namespace std;

TEST(GetFloat, conversions) {
   //                        123456789012345678901234567890
   pearlrt::Character<30> d(" 12        X");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Float<53> f53;
   // normal operations
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<53>::fromF(f53, 6, 0, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(12)).getBoolean());
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<53>::fromF(f53, 3, 0, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(12)).getBoolean());
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<53>::fromF(f53, 2, 0, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(1)).getBoolean());
      EXPECT_NO_THROW(pearlrt::GetFloat<53>::fromF(f53, 2, 0, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(2)).getBoolean());
      // empty input
      EXPECT_THROW(pearlrt::GetFloat<53>::fromF(f53, 2, 0, source),
          pearlrt::FixedValueSignal);
   }
   // illegal w,d
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12        X");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 0, 0, source),
         pearlrt::FixedFormatSignal);
      EXPECT_THROW(
         pearlrt::GetFloat<53>::fromF(f53, -1, 0, source),
         pearlrt::FixedFormatSignal);
      EXPECT_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 6, -1, source),
         pearlrt::FixedFormatSignal);
   }
   // inplicit decimal
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12345    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<53>::fromF(f53, 6, 1, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(1234.5)).getBoolean());
   }
   // negative and implicit decimal
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("-12345    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<53>::fromF(f53, 6, 1, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(-1234.5)).getBoolean());
   }
   // skip wrong field
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("r27 69    X");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 4, 0, source),
         pearlrt::FixedValueSignal);
      EXPECT_NO_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 2, 0, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(69)).getBoolean());
   }
   // skip remaining after field
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("27   69   X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 5, 0, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(27)).getBoolean());
      EXPECT_NO_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 2, 0, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(69)).getBoolean());
   }
   // precision with given decimal point
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("12.34      ");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 6, 2, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(12.34)).getBoolean());
   }
   // precision  without given decimal point
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("1234       ");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 6, 2, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(12.34)).getBoolean());
   }
   // long mantissa
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("123456789012345678901234567   ");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 26, 2, source));
      EXPECT_TRUE((f53 > pearlrt::Float<53>(1.234567890e24)).getBoolean());
      EXPECT_TRUE((f53 < pearlrt::Float<53>(1.234567891e24)).getBoolean());
   }
   // long mantissa with leading zeros
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("000000000000000000001234567   ");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(
         pearlrt::GetFloat<53>::fromF(f53, 27, 2, source));
      EXPECT_TRUE((f53 == pearlrt::Float<53>(1.234567e4)).getBoolean());
   }
   // wrong data in field 
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("-.A                           ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW( pearlrt::GetFloat<53>::fromF(f53, 7, 2, source),
         pearlrt::FixedValueSignal);
   }

}

TEST(GetFloat, f24) {
   //                        123456789012345678901234567890
   pearlrt::Character<30> d(" 12        X");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Float<24> f24;
   // normal operations
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12           X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromF(f24, 6, 1, source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(1.2)).getBoolean());
   }
   // overflow 
   {
      //                        123456789012345678901234567890123456789012345
      pearlrt::Character<50> d("1234567890123456789012345678901234567890.0");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromF(f24, 45, 0, source),
          pearlrt::FloatIsINFSignal);
   }
}

TEST(getFloat, EFormat) {
   //                        123456789012345678901234567890
   pearlrt::Character<30> d(" 12E3        X");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Float<24> f24;
   // normal operations
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12E3           X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(1.2E4)).getBoolean());
   }
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12             X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4,source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(1.2E1)).getBoolean());
   }
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 12E-4             X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(1.2E-3)).getBoolean());
   }
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("-12E-4             X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(-1.2E-3)).getBoolean());
   }
   // exotic values
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("1E-4             X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(1E-4)).getBoolean());
   }
   // exotic values
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(".1E-4             X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(1E-5)).getBoolean());
   }
   // exotic values
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("-.1E-4             X");
      rc.setWork(d);
      source.rewind();
      EXPECT_NO_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source));
      EXPECT_TRUE((f24 == pearlrt::Float<24>(-1E-5)).getBoolean());
   }


   // error: illegal w,d,s values
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("                    ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 0, 1, 4, source),
        pearlrt::ExpFormatSignal);
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, -1, 4, source),
        pearlrt::ExpFormatSignal);
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 0, source),
        pearlrt::ExpFormatSignal);
   }
   // error: no data
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d("                    ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value only -
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" -                  ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 6, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value only no mantissa
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" E10                  ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 16, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value only no mantissa
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" +.E10                  ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 16, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value only no mantissa
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" .E10                  ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 16, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value no exponent
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 1E                  ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 16, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value no exponent
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 1E+                  ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 16, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value no exponent
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 1e+3                  ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 16, 1, 4, source),
        pearlrt::ExpValueSignal);
   }
   // error: illegal value too large exponent
   {
      //                        123456789012345678901234567890
      pearlrt::Character<30> d(" 1E+333                ");
      rc.setWork(d);
      source.rewind();
      EXPECT_THROW(pearlrt::GetFloat<24>::fromE(f24, 16, 1, 4, source),
        pearlrt::FloatIsINFSignal);
   }
}
/**
\endcond
*/
