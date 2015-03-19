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

\brief Character format read test program

\page Testprograms

\section char_read_test

This program checks whether Char variables are read as expected.

\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "Character.h"
#include "GetCharacter.h"
#include "GetUtils.h"
#include "Signals.h"
#include "RefChar.h"
#include "RefCharSource.h"
#include "compare.h"

using namespace std;

TEST(GetCharacter, A_format) {
   //                        12345678901234567890
   pearlrt::Character<20> d(" PEARL A B C D E F G");
   pearlrt::RefCharacter rc;
   rc.setWork(d);
   pearlrt::RefCharSource source(rc);
   pearlrt::Character<5> x;
   // test format width exceptions
   ASSERT_THROW(
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>) - 1, source),
      pearlrt::CharacterFormatSignal);
   // test reading with width=0
   ASSERT_NO_THROW(
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)0, source));
   {
      pearlrt::Character<5> x1("     ");
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::Character<5> x2(" PEAR");
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test reading with shorter variable
   {
      source.rewind();
      pearlrt::Character<5> x1(" PEAR");
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x1).getBoolean());
   }
   // test reading with space appending variable
   {
      source.rewind();
      pearlrt::Character<5> x1(" PEA ");
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)4, source);
      ASSERT_TRUE((x == x1).getBoolean());
   }
   // test stop reading
   {
      source.rewind();
      pearlrt::Character<5> x1(" PEAR");
      pearlrt::Character<5> x2("L A B");
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test stop reading  with skipping input
   {
      source.rewind();
      pearlrt::Character<5> x1(" PEAR");
      pearlrt::Character<5> x2(" A B ");
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)6, source);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test stop reading at end of line
   {
      pearlrt::Character<20> d(" P\nEARL A B C D E F ");
      pearlrt::RefCharacter rc;
      rc.setWork(d);
      pearlrt::RefCharSource source(rc);
      pearlrt::Character<5> x1(" P   ");
      pearlrt::Character<5> x2("     ");
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
   // test stop reading at end of line with SKIP the NEwLine
   {
      pearlrt::Character<20> d(" P\nEARL A B C D E F ");
      pearlrt::RefCharacter rc;
      rc.setWork(d);
      pearlrt::RefCharSource source(rc);
      pearlrt::Character<5> x1(" P   ");
      pearlrt::Character<5> x2("EARL ");
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x1).getBoolean());
      pearlrt::GetUtils::fromSkip(1, source);
      pearlrt::GetCharacter<5>::fromA(x, (pearlrt::Fixed<15>)5, source);
      ASSERT_TRUE((x == x2).getBoolean());
   }
}


/**
\endcond
*/
