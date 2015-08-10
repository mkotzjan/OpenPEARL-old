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

/**
\file

\brief output formating  for the data type CLOCK

\author R. Mueller

*/

#include "Clock.h"
#include "PutClock.h"
#include "Signals.h"
#include "Fixed.h"
#include "Log.h"


namespace pearlrt {

   /* add a two digit decimal value at the location s */
   static void i2sink(int x, bool suppressLeadingZero, Sink& sink) {
      char ch;
      ch = x / 10 + '0';

      if (suppressLeadingZero && ch == '0') {
         ch = ' ';
      }

      sink.putChar(ch);
      sink.putChar(x % 10 + '0');
   }


   void PutClock::toT(const Clock & x,
                      const  Fixed<31>& width, const Fixed<31>& decimals,
                      Sink& sink) {
      int decimal;
      int hours;
      int min;
      int sec = x.getSec();
      long us = x.getUsec();
      int d = decimals.x;
      int w = width.x;
      hours = sec / 3600 ;
      sec %= 3600;
      min = sec / 60;
      sec %= 60;

      if (d < 0 || d > 6) {
         Log::info("decimals out of range %d (not in 0..6)", d);
         throw theClockFormatSignal;
      }

      if (d == 0) {
         if (w < 8) {
            Log::info("width too large %d (<8)", w);
            throw theClockFormatSignal;
         }

         while (w > 8) {
            sink.putChar(' ');
            w --;
         }
      } else {
         if (w - d - 1 < 8) {
            Log::info("width too small");
            throw theClockFormatSignal;
         }

         while (w - d - 1 > 8) {
            sink.putChar(' ');
            w --;
         }
      }

      i2sink(hours, true, sink);
      sink.putChar(':');
      i2sink(min, false, sink);
      sink.putChar(':');
      i2sink(sec, false, sink);

      if (d > 0) {
         static int rounding[] = {0, 50000, 5000, 500, 50, 5, 0};
         sink.putChar('.');
         decimal = 100000;
         us +=  rounding[d];   // d is from 1 to 6

         do {
            sink.putChar(us / decimal + '0');
            us %= decimal;
            decimal /= 10;
            d--;
         } while (d > 0);
      }

      return;
   }
}

