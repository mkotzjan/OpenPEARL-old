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

#include "chip.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Retarget.h"

#include "SystemConsole.h"

extern "C" {
   static pearlrt::GenericUart * console = 0;
   static bool useInterruptFlag = false;

   struct StdInOutJob {
      int len;
      char * data;
   };
   static struct StdInOutJob *stdOutJob = NULL;
   static struct StdInOutJob *stdInJob = NULL;

   int __attribute__((used)) _write(int fd, char * ptr, int len) {
      if (! console) {
         console = pearlrt::SystemConsole::getInstance();
      }

      if (useInterruptFlag) {
         // output in interrupt  mode
         struct StdInOutJob job;

         job.len = len;
         job.data = ptr;

         // wait until job from other thread has finished
         while (!stdOutJob) {};

         stdOutJob = &job;

         console->triggerOutput();

         // wait until this job has finished
         while (!stdOutJob) {};

         return len;
      } else {
         // output in polling mode
         int n = len;
         console->interruptEnable(false);

         while (len > 0) {
            console->sendCharPolling(*ptr);

            if (*ptr == '\n') {
               console->sendCharPolling('\r');
            }

            ptr ++;
            len --;
         }

         console->interruptEnable(true);
         return (n);
      }
   }

#if 0
     void _ttywrch(int ch) {
         /* Write one char "ch" to the default console
          * Need implementing with UART here. */

         char c = ch;


        // sendChar(c);


      }
#endif




   int __attribute__((used)) _read(int fd, char * ptr, int len) {
      if (! console) {
         console = pearlrt::SystemConsole::getInstance();
      }

      if (useInterruptFlag) {
         // input in interrupt mode
         struct StdInOutJob job;

         job.len = len;
         job.data = ptr;

         while (!stdInJob) {}; // wait until job from other thread has finished

         stdInJob = &job;

         while (!stdInJob) {}; // wait until this job has finished

         return len;
      } else {
         // input in polling mode
         int received = 0;
         console->interruptEnable(false);

         while (received < len) {
            *ptr =  console->readCharPolling();

            // backspace and del
            if (*ptr == '\b' || *ptr == 0x7f) {
               if (received > 0) {
                  received --;
                  console->sendCharPolling('\b'); // local echo
                  console->sendCharPolling(' '); // local echo
                  console->sendCharPolling('\b'); // local echo
                  ptr --;
               }

               // test for newline
               // \r is not transmitted by lpc21isp in terminal mode
            } else if (*ptr == '\n') {
               console->sendCharPolling(*ptr); // local echo
               *ptr = '\n';
               console->sendCharPolling(*ptr); // local echo
               *(ptr + 1) = 0;
               received ++;
               break;
            } else {
               console->sendCharPolling(*ptr); // local echo
               ptr ++;
               received ++;
            }
         }

         console->interruptEnable(true);
         return (received);
      }
   }

};

namespace pearlrt {
   void Retarget::useInterrupts() {
      useInterruptFlag = true;
   }

   bool Retarget::getNextStdOutChar(char * nextTransmitChar) {
      if (stdOutJob) {
         if (stdOutJob->len > 0) {
            stdOutJob->len --;
            *nextTransmitChar = *(stdOutJob->data);
            stdOutJob->data ++;

            if (stdOutJob->len == 0) {
               stdOutJob = 0;
            }

            return true;
         }
      }

      return false;
   }

   bool Retarget::addNextStdInChar(char ch) {
      if (stdInJob) {
         if (stdInJob->len > 0) {
            stdInJob->len --;
            *(stdInJob->data) = ch;
            stdInJob->data ++;

            if (stdInJob->len == 0) {
               stdInJob = 0;
            }

            return true;
         }
      }

      return false;
   }
}

