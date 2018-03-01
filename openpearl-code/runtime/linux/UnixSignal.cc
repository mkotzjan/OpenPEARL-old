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

#include <pthread.h>
#include <signal.h>
#include "UnixSignal.h"
#include "Log.h"
#include "Interrupt.h"

namespace pearlrt {
   /**
   \file

   \brief unix signal as interrupt source
   */

   int UnixSignal::isSet = 0;
   UnixSignal* UnixSignal::listOfUnixSignals = 0;
   UnixSignal* UnixSignal::sig1 = 0;
   UnixSignal* UnixSignal::sig2 = 0;
   UnixSignal* UnixSignal::sig3 = 0;
   UnixSignal* UnixSignal::sig15 = 0;
   UnixSignal* UnixSignal::sig16 = 0;
   UnixSignal* UnixSignal::sig17 = 0;

   void UnixSignal::updateSigMask(sigset_t * sig) {
      UnixSignal * i = listOfUnixSignals;

      while (i) {
         sigaddset(sig,  i->sigNum);
         i = i -> next;
      }
   }

   void UnixSignal::unixSignalHandler(int sig) {
      UnixSignal * irupt = 0;

      switch (sig) {
      case 1:
         irupt = sig1;
         break;

      case 2:
         irupt = sig2;
         break;

      case 3:
         irupt = UnixSignal::sig3;
         break;

      case 15:
         irupt = UnixSignal::sig15;
         break;

      case 16:
         irupt = UnixSignal::sig16;
         break;

      case 17:
         irupt = UnixSignal::sig17;
         break;

      default:
         Log::error("UnixSignal: illegal number: %d", sig);
         throw theInternalDatatypeSignal;
      }

      if (irupt) {
         irupt->trigger();
      } else {
         Log::error("UnixSignal: handler got spurious signal");
         throw theInternalDatatypeSignal;
      }
   }


   bool UnixSignal::treat(int sig) {
      bool retval;
      UnixSignal * irupt = 0;

      switch (sig) {
      case 1:
         irupt = sig1;
         break;

      case 2:
         irupt = sig2;
         break;

      case 3:
         irupt = UnixSignal::sig3;
         break;

      case 15:
         irupt = UnixSignal::sig15;
         break;

      case 16:
         irupt = UnixSignal::sig16;
         break;

      case 17:
         irupt = UnixSignal::sig17;
         break;

      default:
         Log::error("UnixSignal: illegal number: %d", sig);
         throw theInternalDatatypeSignal;
      }

      if (irupt) {
         irupt->trigger();
         retval = true;
      } else {
         retval = true;
      }

      return retval;
   }


   /**
   create a interrupt handler for the given Unix-signal

   Only some signal numbers are supported:
   <table>
   <tr><td>sigNum </td><td>Unix usage</td></tr>
   <tr><td>1</td><td>SIGHUP</td></tr>
   <tr><td>2</td><td>SIGINT</td></tr>
   <tr><td>3</td><td>SIGQUIT</td></tr>
   <tr><td>15</td><td>SIGTERM</td></tr>
   <tr><td>16</td><td>SIGUSR1</td></tr>
   <tr><td>17</td><td>SIGUSR2</td></tr>
   </table>

   \param sigNum a signal number for the UNIX system
   */
   UnixSignal::UnixSignal(int sigNum) {
      this->sigNum = sigNum;
      next = listOfUnixSignals;
      listOfUnixSignals = this;

      // test valid signal numbers
      switch (sigNum) {
      case 1:
         sig1 = this;
         break;

      case 2:
         sig2 = this;
         break;

      case 3:
         sig3 = this;
         break;

      case 15:
         sig15 = this;
         break;

      case 16:
         sig16 = this;
         break;

      case 17:
         sig17 = this;
         break;

      default:
         Log::error("UnixSignal: illegal number: %d", sigNum);
         throw theInternalDatatypeSignal;
      }

      if (isSet & (1 << sigNum)) {
         Log::error("UnixSignal %d used more than once", sigNum);
         throw theInternalDatatypeSignal;
      }

      isSet |= 1 << sigNum;
   }

   void UnixSignal::devEnable() {
      // the signals are treated via the signalfd() - file
      // it is difficult to modify the mask in running mode
      // we keep them always on
   }

   void UnixSignal::devDisable() {
      // the signals are treated via the signalfd() - file
      // it is difficult to modify the mask in running mode
      // we keep them always on
   }
}


