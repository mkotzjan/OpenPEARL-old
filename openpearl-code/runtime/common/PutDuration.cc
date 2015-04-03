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

\brief output formats for the data type DURATION

\author R. Mueller

*/

#include "PutDuration.h"
#include "Duration.h"
#include "Signals.h"
#include "Sink.h"

using namespace std;

namespace pearlrt {

   /* add a two digit decimal value at the location s */
   static void i2sink(int x, bool suppressZero, Sink& sink) {
      char ch = (x / 10 + '0');

      if (suppressZero && ch == '0') {
         ch = ' ';
      }

      sink.putChar(ch);
      sink.putChar(x % 10 + '0');
   }

   /* add a string value at the location s */
   static void s2sink(const char *s, Sink & sink) {
      while (*s) {
         sink.putChar(*s);
         s++;
      }
   }

   void PutDuration::toD(const Duration& dur,
                         const Fixed<15>& width,
                         const Fixed<15>& decimals,
                         Sink& sink) {
      int decimal;
      int sec = dur.getSec();
      long us = dur.getUsec();;
      int hours;
      int min;
      int w = width.x;
      int d = decimals.x;

      if (dur.get().get() >= (Fixed63::Fixed63_t)(24 * 3600 * 1000000LL)  ||
            dur.get().get() < (Fixed63::Fixed63_t)0) {
         throw  theDurationValueSignal;
      }

      hours = sec / 3600 ;
      sec %= 3600;
      min = sec / 60;
      sec %= 60;

      if (d < 0 || d > 6) {
         throw theDurationFormatSignal;
      }

      if (d == 0) {
         if (w < 20) {
            throw theDurationFormatSignal;
         }

         while (w > 20) {
            sink.putChar(' ');
            w--;
         }
      } else {
         if (w - d - 1 < 20) {
            throw theDurationFormatSignal;
         }

         while (w - d > 21) {
            sink.putChar(' ');
            w--;
         }
      }

      i2sink(hours, true, sink);
      s2sink(" HRS ", sink);
      i2sink(min, false, sink);
      s2sink(" MIN ", sink);
      i2sink(sec, false, sink);

      if (d > 0) {
         static int rounding[] = {0, 50000, 5000, 500, 50, 5, 0};
         sink.putChar('.');
         decimal = 100000;
         us += rounding[d];   // d is from 1 to 6

         do {
            sink.putChar(us / decimal + '0');
            us %= decimal;
            decimal /= 10;
            d--;
         } while (d > 0);
      }

      s2sink(" SEC", sink);
   }
}

