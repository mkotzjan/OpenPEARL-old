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

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"

#include "GenericUart.h"
#include "Retarget.h"

#define XON   0x11
#define XOFF  0x13
namespace pearlrt {

   GenericUart::GenericUart(bool xon, bool _isConsole) {
      xonProtocol = xon;
      isConsole = _isConsole;
      systemDation = NULL;
      status = 0;
   }

   void GenericUart::registerUartDation(GenericUartDation * u) {
      systemDation = u;
   }

   void GenericUart::translateNewLine(bool _doNewLineTranslation) {
       doNewLineTranslation = _doNewLineTranslation;
   }

   void GenericUart::sendNextChar() {
      char ch;

      // test for XOFF received --> no more transmission
      // else check for reenable communcation (transmit XON)
      // else transmit retarget data, if available
      // else transmit dation output data, if available
      if (status & XOFF_RECEIVED) {
         // do nothing until XON is received
         status &= ~TX_INTERRUPT_PENDING;;
      } else if (status & XOFF_MUST_SEND) {
         sendChar(XOFF);
         status &= ~XOFF_MUST_SEND;
         status |= XOFF_SENT | TX_INTERRUPT_PENDING;;
      } else if (status & LF_MUST_SEND) {
         sendChar('\n');
         status &= ~LF_MUST_SEND;
         status |= TX_INTERRUPT_PENDING;
      } else if (isConsole &&
                 Retarget::getNextStdOutChar(&ch) == true) {

         if (doNewLineTranslation && ch == '\n') {
             status |= LF_MUST_SEND;
             ch = '\r';
         }

         sendChar(ch);
         status |= TX_INTERRUPT_PENDING;;
      } else if (systemDation &&
                 (systemDation->getNextTransmitChar(&ch) == true)) {

         if (doNewLineTranslation && ch == '\n') {
             status |= LF_MUST_SEND;
             ch = '\r';
         }

         sendChar(ch);
         status |= TX_INTERRUPT_PENDING;;
      } else {
    	  status &= ~TX_INTERRUPT_PENDING;
      }
   }

   void GenericUart::sendXonIfNecessary() {
      if (xonProtocol) {
         if (status & XOFF_SENT) {
            status |= XON_MUST_SEND;

            if (!(status & TX_INTERRUPT_PENDING)) {
               sendNextChar();
            }
         }
      }
   }

   void GenericUart::doReceiveChar(char ch) {

      if (ch == XOFF) {
         if (xonProtocol) {
            status |= XOFF_RECEIVED;
         }
      } else if (ch == XON) {
         if (xonProtocol) {
            status &= ~XOFF_RECEIVED;

            if (!(status & TX_INTERRUPT_PENDING)) {
               sendNextChar();  // retrigger output interrupt
            }
         }
      } else {
         if (doNewLineTranslation && ch == '\r') {
            ch = '\n';
         }

         // test if scanf is active on stdin or a system dation wants
         // data. If none, then send Xoff.
         if (isConsole && Retarget::addNextStdInChar(ch) == false) {
            // if not -> delegate to superior layer
            // the superior layer is responsible to buffer at least one
            // input character

            if (systemDation && systemDation->addReceivedChar(ch) == false) {
               // return value false -> no input request --> send xoff
               status |= XOFF_MUST_SEND;

               if (!(status & TX_INTERRUPT_PENDING)) {
                  sendNextChar();  // retrigger output interrupt
               }
            }
         }
      }
   }

}

