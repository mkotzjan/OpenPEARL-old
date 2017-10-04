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


#include <stdio.h>
#include <stdlib.h>
#include "gtest.h"

#include "PearlIncludes.h"
#include "RefCharSource.h"

using namespace std;

TEST(RefCharSourceTests, Operations) {
   pearlrt::Character<10> d(10, (char*)"1234567890");
   pearlrt::RefCharacter rc(d);
   pearlrt::RefCharSource source(rc);
   char ch;
   rc.setWork(d);
   ASSERT_EQ(ch = source.getChar(), '1');
   ASSERT_EQ(ch = source.getChar(), '2');
   ASSERT_EQ(ch = source.getChar(), '3');
   ASSERT_EQ(ch = source.getChar(), '4');
   ASSERT_EQ(ch = source.getChar(), '5');
   ASSERT_EQ(ch = source.getChar(), '6');
   ASSERT_EQ(ch = source.getChar(), '7');
   ASSERT_EQ(ch = source.getChar(), '8');
   ASSERT_EQ(ch = source.getChar(), '9');
   ASSERT_EQ(ch = source.getChar(), '0');
   source.unGetChar(ch);
   ASSERT_EQ(ch = source.getChar(), '0');
   ASSERT_THROW(source.getChar(), pearlrt::NoMoreCharactersSignal);
   source.rewind();
   ASSERT_EQ(ch = source.getChar(), '1');
}


