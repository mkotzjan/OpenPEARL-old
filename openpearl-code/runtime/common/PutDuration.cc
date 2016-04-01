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

   /* add a decimal value with max 4 digits at the location s */
   /* leading zeros are suppressed and not printed            */
   static void i4sink(int x, Sink& sink) {
      bool suppressZero = true;
      char ch;

      ch  = (x / 1000 + '0');
      if (suppressZero && ch == '0') {
         // ch = ' ';
      } else {
         suppressZero = false;
         sink.putChar(ch);
      }
      ch %= 1000;

      ch  = (x / 100 + '0');
      if (suppressZero && ch == '0') {
         //ch = ' ';
      } else {
         suppressZero = false;
         sink.putChar(ch);
      }
      ch %= 100;

      ch  = (x / 10 + '0');
      if (suppressZero && ch == '0') {
         //ch = ' ';
      } else {
         suppressZero = false;
         sink.putChar(ch);
      }

      sink.putChar(x % 10 + '0');
   }

   /* add a two digit decimal value at the location s */
   static void i2sink(int x, bool suppressZero, Sink& sink) {
      char ch = (x / 10 + '0');

      if (suppressZero && ch == '0') {
        // ch = ' ';
      } else {
         sink.putChar(ch);
      }

      sink.putChar(x % 10 + '0');
   }

   /* add a string value at the location s */
   static void s2sink(const char *s, Sink & sink) {
      while (*s) {
         sink.putChar(*s);
         s++;
      }
   }

   void PutDuration::toD(const Duration& _dur,
                         const Fixed<31>& width,
                         const Fixed<31>& decimals,
                         Sink& sink) {
      int decimal;        // helper value for decimal digits
      int sec;
      long us;
      int hours;
      int min;
      int w = width.x;   // int represetation of width
      int d = decimals.x; // int representation of decimals
      int digitsNeeded;
      int fillZeros;     // number of artifical 0 decimals
      int isNegativ = 0;
      Duration dur = _dur;;
      bool leadingSpace;
      bool showHours, showMinutes;

      if ( _dur.get().get() < (Fixed63::Fixed63_t)0) {
        isNegativ = 1;
        dur = - _dur;
      } 

      if (dur.get().get() >= (Fixed63::Fixed63_t)(24 * 3600 * 1000000LL)  ||
            dur.get().get() < (Fixed63::Fixed63_t)0) {
         throw  theDurationValueSignal;
      }

      sec = dur.getSec();
      us = dur.getUsec();

      hours = sec / 3600 ;
      sec %= 3600;
      min = sec / 60;
      sec %= 60;

      //                           vvv add character for decimal point
      showMinutes = (w > d + 12 + (d>0)); // one digit for min
      showHours   = (w > d + 19 + (d>0)); // one digit for hour

      digitsNeeded = d + 4 + (d>0); // decimals + "_SEC"
     
      if (isNegativ) {
        digitsNeeded ++;
      }

      if (!showHours && hours > 0) {
         Log::error("D-format: no space for hours");
         throw theDurationValueSignal;
      }

      if (!showMinutes && min > 0) {
         Log::error("D-format: no space for minutes");
         throw theDurationValueSignal;
      }

      if (showHours) {
        if (hours < 10) {
          digitsNeeded += 15;    // "x_HRS_xx_MIN_xx"
        } else if (hours < 100) {
          digitsNeeded += 16;   // "xx_HRS_xx_MIN_xx"
        } else if (hours < 1000) {
           digitsNeeded += 17;   // "xxx_HRS_xx_MIN_xx"
        } else {
          digitsNeeded += 18;   // "xxxx_HRS_xx_MIN_xx" sufficient for 100 days
        }
      } else if (showMinutes) {
        if (min < 10) {
          digitsNeeded += 8;    // "x_MIN_xx"
        } else {
          digitsNeeded += 9;   // "xx_MIN_xx"
        }
      } else {
        if (sec < 10) {
          digitsNeeded += 1;    // "x"
        } else {
          digitsNeeded += 2;   // "xx"
        }
      }

      if (d < 0 || w <= d+4 ) {
         Log::error("D-format: w,d mismatch (w=%d, d=%d)", w,d);
         throw theDurationFormatSignal;
      }

      if (w < digitsNeeded) {
         Log::error("D-format: field too small (%d required)", digitsNeeded);
         throw theDurationValueSignal;
      }

      while (w > digitsNeeded) {
         sink.putChar(' ');
         w--;
      }
      if (isNegativ) {
         sink.putChar('-');
      }
 
      leadingSpace = true;
      if (showHours) {
         i4sink(hours, sink);
         leadingSpace = false;
         s2sink(" HRS ", sink);
      } 
      if (showMinutes) {
         i2sink(min, leadingSpace, sink);
         leadingSpace = false;
         s2sink(" MIN ", sink);
      }
      i2sink(sec, leadingSpace, sink);

      // the internal storageformat supports only us -> only 6 decimals possible
      fillZeros = 0;
      if (d > 6) {
        fillZeros = d-6;
        d=6;
      }
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
      while (fillZeros > 0) {
         sink.putChar('0');
         fillZeros --;
      }
      s2sink(" SEC", sink);
   }
}

