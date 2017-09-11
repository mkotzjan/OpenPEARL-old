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

#ifndef LPC17XXINTERRUPT_INCLUDED
#define LPC17XXINTERRUPT_INCLUDED

#include "Interrupt.h"

namespace pearlrt {
   /**
   \file

   \brief GPIO an LPC17xx as interrupt source
   */

   /**
   This class provides some interrupt sources via the LPC17xx GPIO

   */
   class Lpc17xxInterrupt : public Interrupt {
   private:
      // the lpc17xx emits one interrupt for all active gpio bits
      // --> we need a list of defined interrupts to find the
      //     corresponding c++ object
      static Lpc17xxInterrupt * listOfInterrupts;
      Lpc17xxInterrupt * next;

      static int bitsInUse[2];

      int port;  // port number of the gpio interrupt source
      int bit;  // bit number of the gpio interrupt source

   public:
      /**
      define an interrupt  for the given GPIO bit

      \param port the number of the gpio block, valid number are 0 and 2
      \param bit  the number of the bit; note that not all bits may be used

      \throws theInternalDationSignal if port:bit does not adress a valid
                         gpio bit with interrupt support
      \throws theDationParamSignal, if the interrupt is already in use
      */
      Lpc17xxInterrupt(int port, int bit);

      /**
      scan interrupt status registers for active bits
      and pass the triggering job of  the corresponding interrupt object
      to the service task

      This is the C++ part of the interrupt service routine
      */
      static void isrPart2();

      /**
      callback to be called from the service task to trigger the
      treatment of the interrupt handling in the application

      \param irupt  pointer to the Lpc17xxInterrupt object
      */
      static void doServiceJob(Lpc17xxInterrupt * irupt);

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

