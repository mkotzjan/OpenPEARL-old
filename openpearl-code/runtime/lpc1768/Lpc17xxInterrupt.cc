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

#include "Lpc17xxInterrupt.h"
#include "Log.h"
#include "Interrupt.h"
#include "service.h"

namespace pearlrt {

   /**

   \brief Lpc17xx GPIO bits as interrupt source
   */

   int Lpc17xxInterrupt::bitsInUse[2] = {0, 0};
   Lpc17xxInterrupt* Lpc17xxInterrupt::listOfInterrupts = NULL;

   void Lpc17xxInterrupt::doServiceJob(Lpc17xxInterrupt* irupt) {
      irupt->trigger();
   }

   void Lpc17xxInterrupt::isrPart2() {
      // cast the type of the function pointer --> modify the type of the
      // parameter form Lpc17xxInterrupt* to void*
      static ServiceJob s = {(void (*)(void*))doServiceJob, NULL};
      Lpc17xxInterrupt * i = listOfInterrupts;

      while (i != NULL) {
         switch (i->port) {
         case 0:  // test for bit in P0
            if (LPC_GPIOINT->IO0.STATF & (1 << i->bit)) {
               s.param = i;
               add_service_from_ISR(&s);
               LPC_GPIOINT->IO0.CLR |= 1 << i->bit;
            }

            break;

         case 2:  // test for bit in P2
            if (LPC_GPIOINT->IO2.STATF & (1 << i->bit)) {
               s.param = i;
               add_service_from_ISR(&s);
               LPC_GPIOINT->IO2.CLR |= 1 << i->bit;
            }

            break;
         }

         i = i->next;
      }

      if (LPC_GPIOINT->IO0.STATF != 0 || LPC_GPIOINT->IO2.STATF != 0) {
         printf/*Log::error*/("Lpc17xxInterrupt: spurious interrupt (%x, %x)",
                              (unsigned int)LPC_GPIOINT->IO0.STATF,
                              (unsigned int)LPC_GPIOINT->IO2.STATF);
         LPC_GPIOINT->IO0.CLR = LPC_GPIOINT->IO0.STATF;
         LPC_GPIOINT->IO2.CLR = LPC_GPIOINT->IO2.STATF;
      }
   }


   Lpc17xxInterrupt::Lpc17xxInterrupt(int _port, int _bit) {
      port = _port;
      bit = _bit;
      // chapter 9.6.5 in UM10360.pdf
      static const int intBits[2] = {0x7fff8fff, 0x00007fff };

      if (port != 0 && port != 2) {
         Log::error("Lpc17xxInterrupt: Illegal port (%d)", port);
         throw theInternalDationSignal;
      }

      if ((intBits[port / 2] & (1 << bit)) == 0) {
         Log::error("Lpc17xxInterrupt: no interrupt on bit(P%d:%d)",
                    port, bit);
         throw theInternalDationSignal;
      }

      if (bitsInUse[port / 2] & (1 << bit)) {
         Log::error("Lpc17xxInterrupt: no interrupt already in use (P%d:%d)",
                    port, bit);
         throw theDationParamSignal;
      }

      LPC_GPIO[port].DIR &= ~(1 <<  bit); // set input mode

      if (bitsInUse[0] == 0 && bitsInUse[1] == 0) {
         NVIC_SetPriority(EINT3_IRQn, 10);
         NVIC_EnableIRQ(EINT3_IRQn);
      }

      bitsInUse[port / 2] |= 1 << bit;	// mark gpio bit as in use

      // append this interrupt as first in the list
      next = listOfInterrupts;
      listOfInterrupts = this;
   }

   void Lpc17xxInterrupt::devEnable() {
      printf("ENABLE\n");

      // enable the correspondig bit in the interrupt enable register
      switch (port) {
      case 0:
         LPC_GPIOINT->IO0.ENF |= 1 <<  bit;
         break;

      case 2:
         LPC_GPIOINT->IO2.ENF |= 1 <<  bit;
         break;
      }
   }

   void Lpc17xxInterrupt::devDisable() {
      // disable the correspondig bit in the interrupt enable register
      switch (port) {
      case 0:
         LPC_GPIOINT->IO0.ENF &= ~(1 <<  bit);
         break;

      case 2:
         LPC_GPIOINT->IO2.ENF &= ~(1 <<  bit);
         break;
      }
   }
}


extern "C" {
   void EINT3_IRQHandler(void) {

      // check the falling edge status bits and trigger
      // the registeres actions
      pearlrt::Lpc17xxInterrupt::isrPart2();
      // the C++ part should clears all pending interrupt request flags

      // thats all folks...
      NVIC_ClearPendingIRQ(EINT3_IRQn);
   }
};

