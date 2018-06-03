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

#include "LineEdit.h"
#include "Log.h"
#include "Interrupt.h"

namespace pearlrt {

   /**
   \brief LineEdit support

   Common stuff for all serial (UART) connections

   */

   enum UartStatus {
      BREAK_INDICATOR = (1 << 0), // return code Break detected
      PARITY_ERROR = (1 << 1), // return code parity error
      FRAME_ERROR = (1 << 2),  // return code frame error
      RECEIVE_OVERRUN = (1 << 3), // return code receive overrun
      SEND_CR_LF = (1 << 4), // internal marker for CR/LF transmission
      WRITE_IS_ACTIVE  = (1 << 5), // internal marker that a write is active
      READ_IS_ACTIVE  = (1 << 6), // internal marker that a read is active
      XOFF_MUST_SEND  = (1 << 7), // internal marker for xon/xoff protocol
      XOFF_SENT       = (1 << 8), // internal marker for xon/xoff protocol
      XOFF_RECEIVED   = (1 << 9), // internal marker for xon/xoff protocol
      RXCHAR_IS_BUFFERED = (1 << 10), // internal marker for xon/xoff protocol
      HAS_UNGETCHAR = (1 << 11) // an unGet-character is present
   };

#define XOFF (0x13)
#define XON  (0x11)

   static const UartStatus  READ_ERRORMASK =
      (UartStatus)(BREAK_INDICATOR | PARITY_ERROR |
                          FRAME_ERROR | RECEIVE_OVERRUN);


   LineEdit::LineEdit(CharInOutProvider * provider, int bufferSize) {
       char * rxBuffer;

       inOutProvider = provider;
       rxBuffer = (char*)pvPortMalloc(rxBufferSize);

      if (rxBuffer == 0) {
         Log::error("LineEdit: could not allocate receive buffer");
         throw theIllegalParamSignal;
      }

      rxRingBuffer.setup(rxBuffer, bufferSize);

      status = 0;       // not busy
      sendCommand.blockSema = xSemaphoreCreateBinary();
      recvCommand.blockSema = xSemaphoreCreateBinary();

      if (sendCommand.blockSema == 0) {
         Log::error("LineEdit: could not create block sema");
         throw theIllegalParamSignal;
      }

      if (recvCommand.blockSema == 0) {
         Log::error("LineEdit: could not create block sema");
         throw theIllegalParamSignal;
      }

      provider->registerCharReceive(this, receiveCharCallback);

   }


   void LineEdit::copyRxRingBuffer() {
      bool goOn = true;
      char ch;

      while (goOn && recvCommand.nbr > 0) {
         goOn = rxRingBuffer.get(&ch);

         if (goOn) {
            *(recvCommand.data++) = ch;
            recvCommand.nbr--;
            recvCommand.nbrReceived ++;

            if (ch == '\n') {
               goOn = false;   // stop delivery at end of line
            }
         }
      }
   }

#if 0
// vermutlich obsolete

   // returns false if nothing to send, else returns true
   bool LineEdit::getNextTransmitChar(char * nextCharToSend) {
      LPC_USART_T * uart = (LPC_USART_T*) lpc_uart;

      if (status & XOFF_MUST_SEND) {
         *nextCharToSend = XOFF;
         status &= ~XOFF_MUST_SEND;
         status |= XOFF_SENT;
         return(true);
      } else if ((status & WRITE_IS_ACTIVE) == 0) {
         // no data write job is active
         // --> test if lineEdit output is pending
         char ch;

         if (lineEditEcho.get(&ch)) {
            *nextCharToSend = ch;
            return(true);
         } else if (sendCommand.nbr > 0) {
            char ch = *(sendCommand.data);

	    if (lineEdit && (ch == '\n')) {
	       // send \n  --> CR+LF must be transmitted
	       if ((status & SEND_CR_LF) == 0) {
	          ch = 0x0d; // CR
	          // mark LF sending is pending
	          status |= SEND_CR_LF;
                  *nextCharToSend = ch;
                  return(true);
	          // stay on the '\n' character
	          // the next RDA-interrupt leads to the else path
	       } else {
	          ch = 0x0a; // LF
	          // mark LF as sent
	          status &= ~SEND_CR_LF;
	          sendCommand.nbr --;
	          sendCommand.data++;  // \n is treated --> next byte
                  *nextCharToSend = ch;
                  return(true);
	       }
	    } else {
	       // transmit normal character
	       sendCommand.nbr --;
               *nextCharToSend = *(sendCommand.data++);
               return(true);
	    }
         }
      } else {
        return false;
      }

      return true;   // should never be reached
   }
#endif


   void LineEdit::receiveCharCallback(char ch) {
      int lineStatus;
      bool charTreated = false;

      // requires local echo and character filtering
      if (ch == 0x08) {
         // backspace works only until begin of line
         char ch1;

         if (rxRingBuffer.last(&ch1)) {
            // previous entered characters available
            if (ch1 != '\n') {
      	      charOutPrivider->write(ch);
      	      charOutPrivider->write(' ');
      	      charOutPrivider->write(ch);
	      rxRingBuffer.forget();
	    } else {
	       // send BELL; try to remove more chars as entered
      	       charOutPrivider->write(0x07);
	    }
         } else {
            // send BELL; try to remove more chars as entered
            charOutPrivider->write(0x07);
         }
      } else if (ch == 0x0d) {
	 // CR

	 if (!rxRingBuffer.add('\n')) {
	    // buffer full
	    charOutProvider->write(0x08);
	    charOutProvider->write('?');
	 } else {
	    charOutProvider->write(ch);
	    charOutProvider->write(0x0a);
	 }

	 xSemaphoreGiveFromISR(recvCommand.blockSema, NULL);
     } else {
        if (!rxRingBuffer.add(ch)) {
           charOutProvider->write(0x08); // BS
           charOutProvider->write(0x07);
           charOutProvider->write('?');  // show buffer full
        }
     }

#if 0
   // das muss vermutlich noch in CharOutPrivider::write() integriert werden
     lineStatus = Chip_UART_ReadLineStatus(uart);

     if (lineStatus & UART_LSR_THRE) {
        lineEditEcho.get(&ch);
>>>>	    Chip_UART_SendByte(uart, ch);
     }
#endif

   }
}

