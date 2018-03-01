/*
 [A "BSD license"]
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

\brief Test routines basic systemdation (Octopus at90 LED)

\page Testprograms

\section OctopusLEDTests tests/OctopusLEDTests.cc

Dation SystemDationB especially at an Octopus at90 LED-device

 
There are several unit tests using the google test framework.


*/
#include <gtest/gtest.h>
#include "Signals.h"
#include "SystemDationB.h"
#include "Fixed.h"
#include <unistd.h>

using namespace std;


/**
  path test
*/
TEST(SystemDationB, OctopusLED) {
 

   extern pearlrt::Device* octo;
   pearlrt::SystemDationB* octoLED = static_cast<pearlrt::SystemDationB*>(octo);
   
   ASSERT_NO_THROW(octoLED->dationOpen());


   pearlrt::Fixed<31> val1;
   pearlrt::Fixed<31> val2;

   //switch all LEDs on, one after another
   for(int i=3; i < 10;i++){

      val1 =(pearlrt::Fixed<31>) i;
      val2 =(pearlrt::Fixed<31>) 1;

      pearlrt::DataElement data[] = { 
         pearlrt::DataElement(&val1, sizeof(pearlrt::Fixed<31>)),
           pearlrt::DataElement(&val2, sizeof(pearlrt::Fixed<31>))
      };

      sleep(1);

      ASSERT_NO_THROW(octoLED->send((pearlrt::Fixed<31>)2,data));

   }
   //switch all LEDs off, one after another
   for(int j=3; j < 10;j++){

      val1 =(pearlrt::Fixed<31>) j;
      val2 =(pearlrt::Fixed<31>) 0;

      pearlrt::DataElement data[] = { 
         pearlrt::DataElement(&val1, sizeof(pearlrt::Fixed<31>)),
           pearlrt::DataElement(&val2, sizeof(pearlrt::Fixed<31>))
      };

      sleep(1);

      ASSERT_NO_THROW(octoLED->send((pearlrt::Fixed<31>)2,data));

   }
   sleep(1); 

/* blink performance test
    
   for (int i=0; i<10000; i++) {
      val1 =(pearlrt::Fixed<31>) 3;
      val2 =(pearlrt::Fixed<31>) 1;

      pearlrt::DataElement data[] = { 
         pearlrt::DataElement(&val1, sizeof(pearlrt::Fixed<31>)),
           pearlrt::DataElement(&val2, sizeof(pearlrt::Fixed<31>))
      };

      octoLED->send((pearlrt::Fixed<31>)2,data);

      val1 =(pearlrt::Fixed<31>) 3;
      val2 =(pearlrt::Fixed<31>) 0;

      octoLED->send((pearlrt::Fixed<31>)2,data);

   }
*/

   ASSERT_NO_THROW(octoLED->Close());
}












