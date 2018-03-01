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

#ifndef UNIXSIGNAL_INCLUDED
#define UNIXSIGNAL_INCLUDED

#include <signal.h>
#include "Interrupt.h"

namespace pearlrt {
   /**
   \file

   \brief unix signal as interrupt source
   */

   /**
   This class provides some UNIX/Linux signals as interrupt source

   */
   class UnixSignal : public Interrupt {
   private:
      static UnixSignal *listOfUnixSignals;
      UnixSignal * next;
      int        sigNum;
      static int isSet;   //< bitmap of signals in use
      static UnixSignal * sig1;
      static UnixSignal * sig2;
      static UnixSignal * sig3;
      static UnixSignal * sig15;
      static UnixSignal * sig16;
      static UnixSignal * sig17;
      static void unixSignalHandler(int sig);

   public:
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
      UnixSignal(int sigNum);

      /**
      update the given signal mask with the signals defined in this
      class

      \param sig pointer to a sigset_t value which is initialized
                 and setup with othe signals from other routines
      */
      static void updateSigMask(sigset_t * sig);

      /**
      treat the given signal

      \param sig number of the signal which has to be treated
      \returns true, if signal was treated
      \returns false, if signal was not treated

      */
      static bool treat(int sig);

      /**
      enable interrupt in interrupt emitting device

      the device shall only trigger the treatment, if the interrupt
      is enabled by the application. This method is called from the
      enable()-method.
      */
      void devEnable();

      /**
      disable interrupt in interrupt emitting device

      the device shall only trigger the treatment, if the interrupt
      is enabled by the application. This method is called from the
      disable()-method.
      */
      void devDisable();

   };
}

#endif

