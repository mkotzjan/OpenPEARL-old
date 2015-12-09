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

\brief module tests for PrioMapper class

These tests use google test frame to verify the proper operation of the methode
implemented in Fixed.h
*/

#include "simpleTests.h"

#include "Signals.h"
#include "Fixed.h"
#include "PrioMapper.h"


/**
Unit tests for prio mapper
*/
TEST(PrioMapper, Values) {
   int sys;
   pearlrt::Fixed<15> prl;

   pearlrt::PrioMapper * pm = pearlrt::PrioMapper::getInstance();
   prl = 1;
   ASSERT_NO_THROW(pm->fromPearl(prl));
   prl = 97;
   ASSERT_NO_THROW(pm->fromPearl(prl));
   prl = 255;
   ASSERT_NO_THROW(pm->fromPearl(prl));
//   prl = 98;
//   ASSERT_THROW(pm->fromPearl(prl), pearlrt::PriorityNotMapableSignal);
//   prl = 254;
//   ASSERT_THROW(pm->fromPearl(prl), pearlrt::PriorityNotMapableSignal);

   sys = pm->getSystemPrio();
   printf("SystemPriority=       %3d\n", sys);
   sys = pm->fromPearl(1);
   printf("PriorityMapper:   1-> %3d\n", sys);
   sys = pm->fromPearl(97);
   printf("PriorityMapper:  97-> %3d\n", sys);
   sys = pm->fromPearl(255);
   printf("PriorityMapper: 255-> %3d\n", sys);

}
