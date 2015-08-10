/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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

\brief input formating  for the data type DURATION


\author R. Mueller

*/

#include <stdio.h>
#include <ctype.h>

#include "Duration.h"
#include "GetDuration.h"
#include "Signals.h"
#include "GetHelper.h"
#include "Log.h"


namespace pearlrt {

   int GetDuration::fromD(Duration& dur ,
                          const Fixed<31> w,
                          const Fixed<31> d,
                          Source & source) {
      int width = w.x;
      int decimals = d.x;
      int hours = 0, min = 0;
      double sec;
      int c1;
      double timeValue;
      double sign = 1.0 ;
      //                0123456789012345678901234567890123
      char logText[] = "illegal D-format field (at: xxxxx)";
      int setCharsAt = 28;
      int charsToSet = 5;

      if (width <= 0) {
         Log::debug("fromD: width <= 0");
         return theDurationFormatSignal.whichRST();
      }

      if (decimals < 0) {
         Log::debug("fromD: width < 0");
         return theDurationFormatSignal.whichRST();
      }

      GetHelper helper(w, &source);
      helper.setDelimiters(GetHelper::EndOfLine);

      if (helper.skipSpaces() == 0) {
         if (helper.readString("-") == 0) {
            sign = -1;
         }

         if (helper.readInteger(&hours, width) > 0) {
            if (helper.readString(" HRS ") == 0) {
               if (helper.readFixedInteger(&min, 2) > 0) {
                  if (min <= 59 && helper.readString(" MIN ") == 0) {
                     // read seconds
                     width = helper.getRemainingWidth();

                     if (helper.readSeconds(&sec, width, decimals) > 0) {
                        if (sec < 60) {
                           if (helper.readString(" SEC") == 0) {
                              timeValue = sec;
                              timeValue += ((hours * 60) + min) * 60;
                              dur = Duration(timeValue * sign);

                              if ((hours = helper.skipSpaces()) < 0) {
                                 return 0;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }

      // format error at all else cases
      width = helper.getRemainingWidth();

      while (width > 0) {
         width --;
         c1 = source.getChar();

         if (charsToSet > 0) {
            logText[setCharsAt++] = c1;
            charsToSet --;
         }
      }

      while (charsToSet > 0) {
         logText[setCharsAt++] = ' ';
         charsToSet --;
      }

      Log::info(logText);
      return theDurationValueSignal.whichRST();
   }
}
