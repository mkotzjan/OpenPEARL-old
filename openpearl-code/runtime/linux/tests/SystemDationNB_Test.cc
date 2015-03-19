/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
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

\brief Test routines generic non-basic systemdations

\page Testprograms

\section Dation SystemDation
Test routines generic non-basic systemdations and systemdation dir

There are several unit tests using the google test framework.


*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "SystemDationNB.h"
#include "Dir.h"
using namespace std;


/**
  path test
*/
TEST(SystemDationNB, path) {
   pearlrt::Dir dir("/home/");
   ASSERT_STREQ("/home/", dir.get());
}
/**
  defice file test
*/
TEST(SystemDationNB, DeviceFileTest) {
   //directory
   pearlrt::Dir dir((char*)"/home/");
   ASSERT_EQ(dir.isDevicePath(), false);
   //check device file path
   pearlrt::Dir dir2((char*)"/dev/ttyS0");
   ASSERT_EQ(dir2.isDevicePath(), true);
   //check whether signals called for illegal path and no file
   ASSERT_THROW(pearlrt::Dir dir3((char*)"/home/tmp"),
                pearlrt::NoFileInfoSignal);
   ASSERT_THROW(pearlrt::Dir dir3((char*)"/etc/passwd"),
                pearlrt::IllegalPathSignal);
}

/**
  testing open and close methods
*/
TEST(SystemDationNB, OpenCloseTest) {
   pearlrt::Dir dir((char*)"/home/");
   ASSERT_THROW(dir.dationOpen(), pearlrt::NotAllowedSignal);
   ASSERT_THROW(dir.dationClose(), pearlrt::NotAllowedSignal);
}
/**
  test pre and post open/close
*/

TEST(SystemdationNB, PrePostOpenClose) {
   pearlrt::Dir dir((char*)"/home/");
   ASSERT_EQ(dir.preOpenConfig(), 0);
   ASSERT_EQ(dir.postOpenConfig(NULL), 0);
   ASSERT_EQ(dir.preCloseConfig(NULL), 0);
   ASSERT_EQ(dir.postCloseConfig(), 0);
}

