/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

#include <cstdarg>   // for va_start,..
#include <unistd.h>
#include <fcntl.h>

#include "Clock.h"
#include "PutClock.h"
#include "Character.h"
#include "RefChar.h"
#include "RefCharSink.h"
#include "Log.h"

namespace pearlrt {
#define ERRORMESSAGE "\n                     **** above line truncated ****\n"

   bool Log::initialized = false;
   bool Log::ctorIsActive = false;
   int Log::logLevel = Log::WARN | Log::ERROR;
   SystemDationNB* Log::provider = NULL;
   Log* Log::instance = NULL;
   Mutex Log::mutex;

   Log* Log::getInstance() {
      if (!instance) {
        try {
         instance = new Log();
        } catch ( ... ) {
          printf("failed to create logger\n");
        }
      }
      return instance;
   }

   Log::Log(SystemDationNB * _provider, char * level) {
      int newLogLevel = 0;

      if (ctorIsActive) {
         printf("RECURSION!\n");
      }
      ctorIsActive = true;
      if (initialized) {
          provider->dationClose(0);
      }
      provider = _provider;
    
      while (*level) {
          switch(*level) {
             case 'E': newLogLevel |= ERROR;
                       break;
             case 'W': newLogLevel |= WARN;
                       break;
             case 'D': newLogLevel |= DEBUG;
                       break;
             case 'I': newLogLevel |= INFO;
                       break;
             /* no error handling here ! */
             /* ignore other characters  */
          }
          level ++;
      }
      setLevel(newLogLevel);

      provider = provider->dationOpen(NULL,0);
      ctorIsActive = false;

      instance=this;
   }

   void Log::exit(void) {
      if (initialized) {
         provider->dationClose(0);
         initialized = false;
      }
   }

   void Log::doit(const Character<7>& type,
                  const char * format,
                  va_list args) {
      Character<128> line;
      RefCharacter rc(line);

      try {
         doFormat(type, rc, format, args);

         mutex.lock();
         provider->dationWrite(rc.getCstring(), rc.getCurrent());
         mutex.unlock();
      } catch (CharacterTooLongSignal s) {
         mutex.lock();
         provider->dationWrite(line.get(), (size_t)(line.upb().x));
         provider->dationWrite((void*)ERRORMESSAGE, strlen(ERRORMESSAGE));
         mutex.unlock();
      }

   }


   void Log::info(const char * format, ...) {
      if (logLevel & Log::INFO) {
         Character<7> type("INFO:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::error(const char * format, ...) {
      if (logLevel & Log::ERROR) {
         Character<7> type("ERROR:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::warn(const char * format, ...) {
      if (logLevel & Log::WARN) {
         Character<7> type("WARN:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::debug(const char * format, ...) {
      if (logLevel & Log::DEBUG) {
         Character<7> type("DEBUG:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   static void addInt(int value, RefCharacter & rc) {
      int l;  // nbr of digits
      int val;
      static int powers[] = {     1,
                                  10,
                                  100,
                                  1000,
                                  10000,
                                  100000,
                                  1000000,
                                  10000000,
                                  100000000,
                                  1000000000
                            };  // 2^31 \approx 2*10^9

      if (value < 0) {
         rc.add('-');
         value = -value;
      }

      l = 0;
      val = value;

      do {
         l++;
         val /= 10;
      } while (val);

      val = value;

      while (l > 0) {
         rc.add((val / powers[l - 1]) + '0');
         val %= powers[l - 1];
         l --;
      }
   }

   static void addHexInt(int value, RefCharacter & rc) {
      static char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
                                };
      // detect nbr set bit
      int nbrOfSetBits;
      int outDigits;
      int index;
      unsigned int wrk = (unsigned) value;

      for (nbrOfSetBits = 0; wrk ; nbrOfSetBits++) {
         wrk >>= 1;
      }

      outDigits = (nbrOfSetBits + 3) / 4 ;

      if (!outDigits) {
         outDigits = 1;   // minimum 1 digit for 0
      }

      while (outDigits > 0) {
         index = (value >> 4 * (outDigits - 1)) & 0x0f;
         rc.add(hexDigits[index]);
         outDigits --;
      }
   }
   static void addFloat(double fVal, int precision, RefCharacter & rc) {
      double rounding = 0.5;

      if (fVal < 0) {
         rc.add('-');
         fVal = -fVal;
      }

      for (int i = 0; i < precision; i++) {
         rounding /= 10.0;
      }

      fVal += rounding;
      int iVal = fVal;
      addInt(iVal, rc);
      fVal -= iVal;
      rc.add('.');

      while (precision > 0) {
         fVal *= 10.0;
         iVal = fVal;
         iVal = (iVal < 0) ? 0 : iVal;
         iVal = (iVal > 9) ? 9 : iVal;
         rc.add(iVal + '0');
         fVal -= iVal;
         precision --;
      }
   }

   static void addUInt(unsigned int value, RefCharacter & rc) {
      int l;  // nbr of digits
      unsigned int val;
      static unsigned int powers[] = {     1,
                                           10,
                                           100,
                                           1000,
                                           10000,
                                           100000,
                                           1000000,
                                           10000000,
                                           100000000,
                                           1000000000
                                     };  // 2^32 \approx 4*10^9
      l = 0;
      val = value;

      do {
         l++;
         val /= 10;
      } while (val);

      val = value;

      while (l > 0) {
         rc.add((val / powers[l - 1]) + '0');
         val %= powers[l - 1];
         l --;
      }
   }

   void Log::doFormat(const Character<7>& type,
                            RefCharacter & rc,
                            const char * format,
                            va_list args) {
      const char * rp;   // pointer in format string
      RefCharSink sink(rc);
      bool percentFound = false;
      bool decimalFound = false;
      bool precisionComplete = false;
      int  precision = 6;
      int iVal;
      unsigned int uiVal;
      double fVal;
      char * sPtr;
      rc.clear();

      Clock c = Clock::now();
      PutClock::toT(c, 15, 6, sink);
      rc.add(' ');
      rc.add(type);
      // simulate vsnprintf with formats %s, %d, %u
      rp = format;

      while (*rp) {  // repeat until end of string
         if (*rp == '%') {
            percentFound = true;
            decimalFound = false;
            precisionComplete = false;
            precision = 6;
            rp ++;
         } else if (decimalFound && precisionComplete == false) {
            // read precision
            if (*rp >= '0' && *rp <= '9') {
               precision *= 10;
               precision += (*rp) - '0';
               rp ++;
            } else {
               precisionComplete = true;
            }
         } else if (percentFound) {
            switch (*(rp)) {
            case '.':
               decimalFound = true;
               precisionComplete = false;
               precision = 0;
               rp ++;
               break;

            case 'f':
               fVal = va_arg(args, double);
               addFloat(fVal, precision, rc);
               percentFound = false;
               rp ++;
               break;

            case 'x':
               iVal = va_arg(args, int);
               addHexInt(iVal, rc);
               percentFound = false;
               rp ++;
               break;

            case 'd':
               iVal = va_arg(args, int);
               addInt(iVal, rc);
               percentFound = false;
               rp ++;
               break;

            case 'u':
               uiVal = va_arg(args, unsigned int);
               addUInt(uiVal, rc);
               percentFound = false;
               rp ++;
               break;

            case 's':
               sPtr = va_arg(args, char *);

               while (*sPtr) {
                  rc.add(*sPtr);
                  sPtr ++;
               }

               percentFound = false;
               rp ++;
               break;

            case 'c':
               iVal = va_arg(args, int);
               rc.add(iVal);
               percentFound = false;
               rp ++;
               break;

            default:
               percentFound = false;
               rc.add('%');
               rc.add(*(rp + 1));
               rp += 2;
               break;
            }
         } else {
            rc.add(*rp);
            rp ++;
         }
      }

      rc.add('\n');
   }

   void Log::setLevel(int level) {
      logLevel = level;
   }

}

