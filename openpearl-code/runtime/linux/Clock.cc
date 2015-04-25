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
#include <stdint.h>
#include <string>
#include <sys/time.h>
#include <ctype.h>
#include <time.h>

#include "Clock.h"
#include "BitString.h"


namespace pearlrt {
   /*
   calculate the difference between gettimeofday and localtime only once
   and correct at each call of now()
   */
   static int calcTimeShift(void) {
      struct timeval tv;
      struct tm * tm;
      int timeShift;
      gettimeofday(&tv, 0);
      tm = localtime(&tv.tv_sec);
      timeShift = (tv.tv_sec % 86400) -
                  (tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec);
      return timeShift;
   }



   Clock Clock::now(void) {
      static const int timeShift = calcTimeShift();
      struct timeval tv;
      int64_t x;
      gettimeofday(&tv, 0);
      // adjust to local time
      x = (int64_t)((tv.tv_sec - timeShift) % 86400) * 1000000LL + tv.tv_usec;
      Clock c(x / 1e6);
      return (c);
   }

   int Clock::getSec() const {
      Fixed63 x = intval.get() / 1000000;
      return x.get();
   }

   int Clock::getUsec() const {
      int x = intval.get() % 1000000;
      return x;
   }

   const Fixed63& Clock::get() const {
      return intval;
   }

   Clock::Clock() {
      intval = (Fixed63::Fixed63_t)0;
   }

   Clock::Clock(double sec) {
      // adjust time to one day
      // this implementation is slow, but only present to enshure proper
      // values. The compiler should check this.
      while (sec < 0) {
         sec += 86400.0;
      }

      while (sec >= 86400) {
         sec -= 86400.0;
      }

      intval = sec * 1e6 + 0.5;
   }

   Duration Clock::operator-(const Clock& a) {
      Fixed63 d = (intval - a.intval);
      double dur;
      dur = d.get() / 1e6;
      return Duration(dur);
   }


   Clock& Clock::operator+(const Duration & d) {
      intval += d.get();
      adjust();
      return *this;
   }

   Clock& Clock::operator-(const Duration & d) {
      intval -= d.get();
      adjust();
      return *this;
   }

   void Clock::adjust() {
      static Fixed63 maxClock =
         Fixed63((Fixed63::Fixed63_t)(24LL * 60 * 60 * 1000000LL));
      intval %= maxClock;

      if (intval.get() < 0) {
         intval += maxClock;
      }
   }

   int Clock::compare(const Clock& rhs) const {
      return intval.compare(rhs.intval);
   }

   BitString<1> Clock::operator<(const Clock& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) < 0);
   }

   BitString<1> Clock::operator<=(const Clock& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) <= 0);
   }

   BitString<1> Clock::operator==(const Clock& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) == 0);
   }

   BitString<1> Clock::operator!=(const Clock& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) != 0);
   }

   BitString<1> Clock::operator>=(const Clock& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) >= 0);
   }

   BitString<1> Clock::operator>(const Clock& rhs) const {
      return BitString<1>(intval.compare(rhs.intval) > 0);
   }
}
