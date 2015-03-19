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

\brief loggin test program

\page Testprograms

\section log_test

This program checks whether the logging system works.

The program writes thre log messages and exits. The file 'pearl_log.txt'
should contain the written messages.

Expected result of the log file:
\verbatim
 9:50:34.684299: INFO:      info message
 9:50:34.684399: INFO:      x=1230, y=2
 9:50:34.684499: INFO:      string=shortText
 9:50:34.694525: WARNING:   warn message
 9:50:34.696037: ERROR:     error message
 9:50:34.700000: ERROR:     01234567890123456789012345678901234567890123456789
012345678901234567890123456789012345678901234567
                  ***** above line truncated ****
\endverbatim

\cond TREAT_EXAMPLES
*/


#include "Log.h"

using namespace pearlrt;

int main() {
   int x = 1230;
   int y = 2;
   double f = 3.1456;
   char shortText[] = "short Text";
   char longText[128];

   for (unsigned int i = 0; i < sizeof(longText); i++) {
      longText[i] = (i % 10) + '0';
   }

   Log::info("info message");
   Log::info("x=%d, y=%d", x, y);
   Log::info("f=%f f1=%.1f f2=%.2f f3=%.3f f4=%.4f f5=%.5f f6 = %.6f",
             f, f, f, f, f, f, f);
   f = - f;
   Log::info("f=%f f1=%.1f f2=%.2f f3=%.3f f4=%.4f f5=%.5f f6 = %.6f",
             f, f, f, f, f, f, f);
   Log::info("string=%s", shortText);
   Log::warn("warn message");
   Log::error("error message");
   Log::error("too long message %s", longText);
}

/**
\endcond
*/
