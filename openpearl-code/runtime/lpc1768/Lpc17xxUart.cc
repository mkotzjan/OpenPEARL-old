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

#include "Lpc17xxUart.h"
#include "Log.h"
#include "Interrupt.h"


namespace pearlrt {

   /**
   \brief Lpc17xx uart support

   */

   enum Lpc17xxUartStatus {
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

   static const Lpc17xxUartStatus  READ_ERRORMASK =
      (Lpc17xxUartStatus)(BREAK_INDICATOR | PARITY_ERROR |
                          FRAME_ERROR | RECEIVE_OVERRUN);

   static const PINMUX_GRP_T pinmuxingUART0[] = {
      {0,  2,   IOCON_MODE_INACT | IOCON_FUNC1},   /* TXD0 */
      {0,  3,   IOCON_MODE_INACT | IOCON_FUNC1}    /* RXD0 */
   };
   static const PINMUX_GRP_T pinmuxingUART2[] = {
      {0, 10,   IOCON_MODE_INACT | IOCON_FUNC1},   /* TXD2 */
      {0, 11,   IOCON_MODE_INACT | IOCON_FUNC1}    /* RXD2 */
   };

   static const int validBaudRates[] = { 300, 600, 1200, 2400, 4800, 9600,
                                         19200, 38400, 57600, 115200
                                       };

   Lpc17xxUart* Lpc17xxUart::uartObject[2];

   Lpc17xxUart::Lpc17xxUart(int _port, int baudRate, int bitsPerCharacter,
                            int stopBits, char parity, int mode) :
      writeSema(1), readSema(1) {
      int i;
      int data;
      bool found;
      LPC_USART_T * _lpc_uart = NULL;

      // test parameters
      if (_port != 0 && _port != 2) {
         Log::error("Lpc17xxUart: illegal port: %d", _port);
         throw theIllegalParamSignal;
      }

      found = false;

      for (i = 0;
            i < (int)(sizeof(validBaudRates) / sizeof(validBaudRates[0]));
            i++) {
         if (validBaudRates[i] == baudRate) {
            found = true;
         }
      }

      if (! found) {
         Log::error("Lpc17xxUart: illegal baud rate: %d",
                    baudRate);
         throw theIllegalParamSignal;
      }

      if (bitsPerCharacter < 5 || bitsPerCharacter > 8) {
         Log::error("Lpc17xxUart: illegal number of bits per character: %d",
                    bitsPerCharacter);
         throw theIllegalParamSignal;
      }

      if (stopBits < 1 || stopBits > 2) {
         Log::error("Lpc17xxUart: illegal number of stop bits: %d",
                    stopBits);
         throw theIllegalParamSignal;
      }

      if (parity != 'O' && parity != 'E' && parity != 'N') {
         Log::error("Lpc17xxUart: illegal parity: %c",
                    parity);
         throw theIllegalParamSignal;
      }

      if ((mode & 0xfffc0000) != 0) {
         Log::error("Lpc17xxUart: unknown mode bits set");
         throw theIllegalParamSignal;
      }

      // setup attribute values
      rxBufferSize = (mode & 0x0ffff);
      lineEdit = (mode >> 16) & 0x01;
      xonProtocol = (mode >> 17) & 0x01;

      if (lineEdit && rxBufferSize > 0) {
         rxBuffer = (char*)pvPortMalloc(rxBufferSize);

         if (rxBuffer == 0) {
            Log::error("Lpc17xxUart: could not allocate receive buffer");
            throw theIllegalParamSignal;
         }

         rxRingBuffer.setup(rxBuffer, rxBufferSize);
      }

      status = 0;  // not busy
      sendCommand.blockSema = xSemaphoreCreateBinary();
      recvCommand.blockSema = xSemaphoreCreateBinary();

      if (sendCommand.blockSema == 0) {
         Log::error("Lpc17xxUart: could not create block sema");
         throw theIllegalParamSignal;
      }

      if (recvCommand.blockSema == 0) {
         Log::error("Lpc17xxUart: could not create block sema");
         throw theIllegalParamSignal;
      }

      // setup uart
      switch (_port) {
      case 0:
         Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0);
         Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxingUART0,
                     sizeof(pinmuxingUART0) / sizeof(pinmuxingUART0[0]));
         _lpc_uart = LPC_UART0;
         break;

      case 2:
         Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART2);
         Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxingUART2,
                     sizeof(pinmuxingUART2) / sizeof(pinmuxingUART2[0]));
         _lpc_uart = LPC_UART2;
         break;
      }

      lpc_uart = (void*) _lpc_uart;
      Chip_UART_Init(_lpc_uart);
      Chip_UART_SetBaud(_lpc_uart, baudRate);

      data = 0;

      if (stopBits == 1) {
         data |= UART_LCR_SBS_1BIT;
      }

      if (stopBits == 2) {
         data |= UART_LCR_SBS_2BIT;
      }

      if (bitsPerCharacter == 5) {
         data |= UART_LCR_WLEN5;
      }

      if (bitsPerCharacter == 6) {
         data |= UART_LCR_WLEN6;
      }

      if (bitsPerCharacter == 7) {
         data |= UART_LCR_WLEN7;
      }

      if (bitsPerCharacter == 8) {
         data |= UART_LCR_WLEN8;
      }

      if (parity == 'E') {
         data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN);
      }

      if (parity == 'O') {
         data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_ODD);
      }

      if (parity == 'N') {
         // nothing to do here
      }

      Chip_UART_ConfigData(_lpc_uart, data);

      Chip_UART_SetupFIFOS(_lpc_uart,
                           UART_FCR_FIFO_EN |   // enable
                           UART_FCR_RX_RS  |    // reset rx fifo
                           UART_FCR_TX_RS  |    // reset tx fifo
                           UART_FCR_TRG_LEV0);  // int one each rx char


      mutex.name("Lpc17xxUartx");
      nbrOpenUserDations = 0;

      lineEditEcho.setup(echoBuffer, sizeof(echoBuffer));

      uartObject[_port / 2] = this;  // _port is ether 0 or 2 --> map to 0/1

   }


   int Lpc17xxUart::capabilities() {
      return (FORWARD | ANY | PRM | IN | OUT | INOUT);
   }

   Lpc17xxUart* Lpc17xxUart::dationOpen(const char * idfValue,
                                        int openParams) {
      if (openParams & (Dation::IDF | Dation::CAN)) {
         Log::error("Lpc17xxUart: does not support IDF and CAN");
         throw theIllegalParamSignal;
      }

      mutex.lock();

      nbrOpenUserDations++;

      // enable interrupt, if this is the first open on this dation
      if (nbrOpenUserDations == 1) {
         Chip_UART_TXEnable((LPC_USART_T*)lpc_uart);
         Chip_UART_IntEnable((LPC_USART_T*)lpc_uart,
                             UART_IER_RBRINT |
                             UART_IER_RLSINT |
                             UART_IER_THREINT);

         if (lpc_uart == (void*)LPC_UART0) {
            NVIC_SetPriority(UART0_IRQn, 10);
         } else if (lpc_uart == (void*)LPC_UART2) {
            NVIC_SetPriority(UART2_IRQn, 10);
         }

         interruptEnable(true);
      }

      mutex.unlock();

      return this;
   }

   void Lpc17xxUart::interruptEnable(bool on) {
      if (on) {
         if (lpc_uart == (void*)LPC_UART0) {
            NVIC_EnableIRQ(UART0_IRQn);
         } else if (lpc_uart == (void*)LPC_UART2) {
            NVIC_EnableIRQ(UART2_IRQn);
         }
      } else {
         if (lpc_uart == (void*)LPC_UART0) {
            NVIC_DisableIRQ(UART0_IRQn);
         } else if (lpc_uart == (void*)LPC_UART2) {
            NVIC_DisableIRQ(UART2_IRQn);
         }
      }
   }

   void Lpc17xxUart::dationClose(int closeParams) {

      mutex.lock();

      if (nbrOpenUserDations == 0) {
         Log::error("Lpc17xxUart: no dation opened");
         mutex.unlock();
         throw theCloseFailedSignal;
      }

      nbrOpenUserDations--;

      // disable interrupt, if this is the last close on this dation
      if (nbrOpenUserDations == 0) {
         interruptEnable(false);
      }

      if (closeParams & Dation::CAN) {
         Log::error("Lpc17xxUart: CAN not supported");
         mutex.unlock();
         throw theIllegalParamSignal;
      }

      mutex.unlock();
   }

   void Lpc17xxUart::copyRxRingBuffer() {
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

   void Lpc17xxUart::dationRead(void * destination, size_t size) {
      bool errorExit = false;

      if (nbrOpenUserDations == 0) {
         Log::error("Lpc17xxUart: not opened");
         throw theIllegalParamSignal;
      }

      // request semaphores in ordered locking write->read
      if (lineEdit) {
         writeSema.request();
      }

      readSema.request();
      mutex.lock();

      // disable uart interrupt and treat buffered input
      interruptEnable(false);
      status |= READ_IS_ACTIVE;
      recvCommand.data = (char*)destination;
      recvCommand.nbr = size;
      recvCommand.nbrReceived = 0;

      if (status & XOFF_SENT) {
         status &= ~XOFF_SENT;
         Chip_UART_SendByte((LPC_USART_T*)lpc_uart, XON);
      }

      if (status & HAS_UNGETCHAR) {
         *(recvCommand.data++) = unGetChar;
         recvCommand.nbr--;
         recvCommand.nbrReceived ++;
         status &= ~HAS_UNGETCHAR;
      }

      if (!lineEdit) {
         if (status & RXCHAR_IS_BUFFERED && recvCommand.nbr > 0) {
            *(recvCommand.data++) = bufferedInputChar;
            recvCommand.nbr--;
            recvCommand.nbrReceived ++;
            status &= ~RXCHAR_IS_BUFFERED;
         }

         if (recvCommand.nbr  > 0) {
            interruptEnable(true);
            // uart interrupt enabled --> wait until data are ready

            xSemaphoreTake(recvCommand.blockSema, portMAX_DELAY);
            interruptEnable(false);
         }
      } else {
         // lineEdit = true
         copyRxRingBuffer();

         if (recvCommand.nbr  > 0) {
            interruptEnable(true);
            // uart interrupt enabled --> wait until data are ready

            xSemaphoreTake(recvCommand.blockSema, portMAX_DELAY);
            interruptEnable(false);
            copyRxRingBuffer();
         }
      }

      if ((status & READ_ERRORMASK) != 0) {
         errorExit = true;
         logError();
      }

      status &= ~READ_ERRORMASK;
      status &= ~READ_IS_ACTIVE;
      interruptEnable(true);

      mutex.unlock();

      if (lineEdit) {
         writeSema.release();
      }

      readSema.release();

      if (errorExit) {
         throw theReadingFailedSignal;
      }

   }

   void Lpc17xxUart::dationWrite(void * destination, size_t size) {
      bool errorExit = false;

      if (nbrOpenUserDations == 0) {
         Log::error("Lpc17xxUart: not opened");
         throw theIllegalParamSignal;
      }

      // request semaphores in ordered locking write->read
      writeSema.request();

      if (lineEdit) {
         readSema.request();
      }

      mutex.lock();

      interruptEnable(false);

      status |= WRITE_IS_ACTIVE;
      sendCommand.data = (char*)destination;
      sendCommand.nbr = size;

      if (!(status & XOFF_RECEIVED)) {
         sendNextChar();
//         sendCommand.nbr--;
//         Chip_UART_SendByte((LPC_USART_T*)lpc_uart, *(sendCommand.data++));
      }

      interruptEnable(true);
      xSemaphoreTake(sendCommand.blockSema, portMAX_DELAY);
      interruptEnable(false);

      if ((status & READ_ERRORMASK) != 0) {
         errorExit = true;
         logError();
      }

      status &= ~READ_ERRORMASK;
      status &= ~WRITE_IS_ACTIVE;

      interruptEnable(true);

      mutex.unlock();
      writeSema.release();

      if (lineEdit) {
         readSema.release();
      }

      if (errorExit) {
         throw theWritingFailedSignal;
      }

   }

   void Lpc17xxUart::dationUnGetChar(const char c) {
      mutex.lock();
      unGetChar = c;
      status |= HAS_UNGETCHAR;
      mutex.unlock();
   }

   void Lpc17xxUart::logError() {
      // echo only one error reason, eg. BREAK will often
      // come together with PARITY_ERROR
      if (status & BREAK_INDICATOR) {
         Log::error("Lpc17xxUart: break received");
      } else if (status & FRAME_ERROR) {
         Log::error("Lpc17xxUart: frame error");
      } else if (status & PARITY_ERROR) {
         Log::error("Lpc17xxUart: parity error");
      } else  if (status & RECEIVE_OVERRUN) {
         Log::error("Lpc17xxUart: receive overrun");
      }
   }

   void Lpc17xxUart::irqHandler(int uartIndex) {
      switch (uartIndex) {
      case 0:
         uartObject[0]->treatInterrupt();
         break;

      case 2:
         uartObject[1]->treatInterrupt();
         break;
      }
   }

   void Lpc17xxUart::treatInterrupt() {
      LPC_USART_T * uart = (LPC_USART_T*) lpc_uart;
      int lineStatus;

      // read interrupt reason and clear additional bits
      int intIdReg = Chip_UART_ReadIntIDReg(uart) & UART_IIR_INTID_MASK;

      // line status interrupt
      if (intIdReg == UART_IIR_INTID_RLS) {
         lineStatus = Chip_UART_ReadLineStatus(uart);

         if (lineStatus & UART_LSR_BI) {
            // break indicator
            status |= BREAK_INDICATOR;
         } else {
            if (lineStatus & UART_LSR_FE) {
               // framing error
               status |= FRAME_ERROR;
            }

            if (lineStatus & UART_LSR_OE) {
               // receive overrun
               status |= RECEIVE_OVERRUN;
            }

            if (lineStatus & UART_LSR_PE) {
               // receive parity error
               status |= PARITY_ERROR;
            }
         }

         if (lineStatus & UART_LSR_RDR) {
            // add pending char to receive buffer
            doRecvChar();
         }
      }

      // receive data available interrupt
      if (intIdReg == UART_IIR_INTID_RDA) {
         doRecvChar();
      }

      /* character timeout interrupt is not needed, since we
         trigger the interrupt on each singe character FIFO_TRGLEVEL
      if (intIdReg & UART_IIR_INTID_CTI) {
      }
      */

      // transmitter empty interrupt
      if (intIdReg == UART_IIR_INTID_THRE) {

         if (status & XOFF_RECEIVED) {
            // do nothing and wait for reception of xon
            // the receiver part of the isr must retrigger the
            // transmission when xon is received
         } else {
            if (sendNextChar() == false) {
               // nothing to send --> all data sent?
               if (status & WRITE_IS_ACTIVE) {
                  xSemaphoreGiveFromISR(sendCommand.blockSema, NULL);
               }
            }
         }
      }
   }


   // returns false if nothing to send
   bool Lpc17xxUart::sendNextChar() {
      LPC_USART_T * uart = (LPC_USART_T*) lpc_uart;

      if (status & XOFF_MUST_SEND) {
         Chip_UART_SendByte(uart, XOFF);
         status &= ~XOFF_MUST_SEND;
         status |= XOFF_SENT;
      } else if ((status & WRITE_IS_ACTIVE) == 0) {
         // no data write job is active
         // --> test if lineEdit output is pending
         char ch;

         if (lineEditEcho.get(&ch)) {
            Chip_UART_SendByte(uart, ch);
         }
      } else if (sendCommand.nbr > 0) {
         char ch = *(sendCommand.data);

         if (lineEdit && (ch == '\n')) {
            // send \n  --> CR+LF must be transmitted
            if ((status & SEND_CR_LF) == 0) {
               ch = 0x0d; // CR
               // mark LF sending is pending
               status |= SEND_CR_LF;
               Chip_UART_SendByte(uart, ch);
               // stay on the '\n' character
               // the next RDA-interrupt leads to the else path
            } else {
               ch = 0x0a; // LF
               // mark LF as sent
               status &= ~SEND_CR_LF;
               sendCommand.nbr --;
               sendCommand.data++;  // \n is treated --> next byte
               Chip_UART_SendByte(uart, ch);
            }
         } else {
            // transmit normal character
            sendCommand.nbr --;
            Chip_UART_SendByte(uart, *(sendCommand.data++));
         }
      } else {
         return false;
      }

      return true;
   }

   void Lpc17xxUart::doRecvChar() {
      LPC_USART_T * uart = (LPC_USART_T*) lpc_uart;
      char ch;
      int lineStatus;
      bool charTreated = false;

      ch = Chip_UART_ReadByte(uart);

      if (ch == XOFF) {
         if (xonProtocol) {
            charTreated = true;
            status |= XOFF_RECEIVED;
         }
      } else if (ch == XON) {
         if (xonProtocol) {
            charTreated = true;
            status &= ~XOFF_RECEIVED;
            sendNextChar();  // retrigger output interrupt
         }
      } else if (lineEdit) {
         // requires local echo and character filtering
         if (ch == 0x08) {
            // backspace works only until begin of line
            char ch1;

            if (rxRingBuffer.last(&ch1)) {
               // previous entered characters available
               if (ch1 != '\n') {
                  lineEditEcho.add(ch);
                  lineEditEcho.add(' ');
                  lineEditEcho.add(ch);
                  rxRingBuffer.forget();
               } else {
                  // send BELL; try to remove more chars as entered
                  lineEditEcho.add(0x07);
               }
            } else {
               // send BELL; try to remove more chars as entered
               lineEditEcho.add(0x07);
            }
         } else if (ch == 0x0d) {
            // CR

            if (!rxRingBuffer.add('\n')) {
               // buffer full
               lineEditEcho.add(0x08);
               lineEditEcho.add('?');
            } else {
               lineEditEcho.add(ch);
               lineEditEcho.add(0x0a);
            }

            xSemaphoreGiveFromISR(recvCommand.blockSema, NULL);
         } else {
            if (!rxRingBuffer.add(ch)) {
               lineEditEcho.add(0x08); // BS
               lineEditEcho.add(0x07);
               lineEditEcho.add('?');  // show buffer full
            }
         }

         lineStatus = Chip_UART_ReadLineStatus(uart);

         if (lineStatus & UART_LSR_THRE) {
            lineEditEcho.get(&ch);
            Chip_UART_SendByte(uart, ch);
         }

         charTreated = true;    // in lineEdit all characters are treated
      }


      if (charTreated)  {
         return;
      }

      if (recvCommand.nbr > 0) {
         *(recvCommand.data++) = ch;
         recvCommand.nbr --;
         recvCommand.nbrReceived ++;

         if (recvCommand. nbr == 0) {
            xSemaphoreGiveFromISR(recvCommand.blockSema, NULL);
         }
      } else {
         if (xonProtocol) {
            status |= XOFF_MUST_SEND;
         }

         status |= RXCHAR_IS_BUFFERED;
         bufferedInputChar = ch;
      }
   }

}


extern "C" {
   void UART0_IRQHandler(void) {

   }

   void UART2_IRQHandler(void) {
      pearlrt::Lpc17xxUart::irqHandler(2);
   }
};

