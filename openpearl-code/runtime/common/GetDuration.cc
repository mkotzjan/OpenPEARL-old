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
      int number;
      int hours = 0;
      int  min = 0;
      double sec = 0;
      int c1;
      double timeValue;
      double sign = 1.0 ;
      //                0123456789012345678901234567890123
      char logText[] = "illegal D-format field (at: xxxxx)";
      int setCharsAt = 28;
      int charsToSet = 5;

      enum {NUMBER = 1, HRS = 2, MIN = 4, POINT = 8, SEC = 16, SPACE = 32};
      int possibleElements = NUMBER | SPACE;
      int remainingElements = HRS | MIN | SEC | POINT;

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

      if (helper.skipSpaces() != 0) {
         goto errorExit;
      }

      if (helper.readString("-") == 0) {
         sign = -1;
      }

      do {

         if (possibleElements & SPACE) {
            if (helper.skipSpaces() != 0) {
               goto errorExit;
            }
         }

         if (possibleElements & NUMBER) {

            if (helper.readInteger(&number, width) > 0) {
               possibleElements = SPACE | remainingElements; 
            } else {
               goto errorExit;
            } 
         } else {
            if (possibleElements & (HRS | MIN | SEC | POINT)) {
               c1 = helper.readChar();
               switch (c1) {
               default:
                  goto errorExit;

               case 'H':
                  if (helper.readString("RS") != 0) {
                     goto errorExit;
                  }

                  hours = number;
                  remainingElements &= ~ HRS;
                  possibleElements = SPACE | NUMBER | POINT; 
                  break;

               case 'M':
                  if (helper.readString("IN") != 0) {
                     goto errorExit;
                  }

                  min = number;
                  remainingElements &= ~ MIN;
                  possibleElements = SPACE | NUMBER | POINT; 
                  break;

               case 'S':
                  if (helper.readString("EC") != 0) {
                     goto errorExit;
                  }

                  remainingElements &= ~ (SEC | POINT);
                  possibleElements = 0;
                  sec = number;
                  break;

               case '.':
                  sec = number;
                  c1 = helper.readChar();
                  double factor = 0.1;

                  while (isdigit(c1)) {
                     sec += factor * (c1 - '0');
                     c1 = helper.readChar();
                     factor /= 10.0;
                  }
                  while ( c1 == ' ') { 
                     c1 = helper.readChar();
                  }
                  if (c1 == 'S') {
                     if (helper.readString("EC") != 0) {
                        goto errorExit;
                     }

                     possibleElements = 0;
                  }
               }
            }
         }
      } while (possibleElements);


      timeValue = sec;
      timeValue += ((hours * 60) + min) * 60;
      dur = Duration(timeValue * sign);

      if (sec >= 60) {
         goto errorExit;
      }

      if (min >= 60) {
         goto errorExit;
      }

      if ( helper.skipSpaces() == -1) {
         // end of field reached with spaces
         return 0;
      }

      // else preform error exit routine

errorExit:
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




