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

#include "Lpc17xxUartInternal.h"
//#include "Lpc17xxUart.h"
#include "Log.h"

extern "C" {
   void UART0_IRQHandler(void) {
      pearlrt::Lpc17xxUartInternal::irqHandler(0);
   }

   void UART2_IRQHandler(void) {
      pearlrt::Lpc17xxUartInternal::irqHandler(2);
   }
};

namespace pearlrt {

   /**
   \brief Lpc17xx uart  low level support with Xon/Xoff protocol support

   */

   static const PINMUX_GRP_T pinmuxingUART0[] = {
      {0,  2,   IOCON_MODE_INACT | IOCON_FUNC1},   /* TXD0 */
      {0,  3,   IOCON_MODE_INACT | IOCON_FUNC1}    /* RXD0 */
   };
   static const PINMUX_GRP_T pinmuxingUART2[] = {
      {0, 10,   IOCON_MODE_INACT | IOCON_FUNC1},   /* TXD2 */
      {0, 11,   IOCON_MODE_INACT | IOCON_FUNC1}    /* RXD2 */
   };

   static const int validBaudRates[] = { 300, 600, 1200,
                                         2400, 4800, 9600,
                                         19200, 38400, 57600, 115200
                                       };

   Lpc17xxUartInternal* Lpc17xxUartInternal::uartObject[2] = {NULL, NULL};

   Lpc17xxUartInternal::Lpc17xxUartInternal(int _port, int baudRate,
         int bitsPerCharacter, int stopBits, char parity, bool xon)
      : GenericUart(xon, _port == 0) {
      setup(_port, baudRate, bitsPerCharacter, stopBits, parity);
   }

   Lpc17xxUartInternal* Lpc17xxUartInternal::getInstance(int _port,
         int baudRate, int bitsPerCharacter, int stopBits, char parity,
         bool xon) {
      if (_port != 0 && _port != 2) {
         Log::error("Lpc17xxUartInternal: illegal port number (%d)", _port);
         throw theInternalDationSignal;
      }

      if (!uartObject[_port / 2]) {
         uartObject[_port / 2] = new Lpc17xxUartInternal(
            _port, baudRate, bitsPerCharacter, stopBits, parity, xon);
      }

      return (uartObject[_port / 2]);
   }

   void Lpc17xxUartInternal::setup(int _port, int baudRate,
                                   int bitsPerCharacter, int stopBits,
                                   char parity) {
      int i;
      int data;
      bool found;

      // test parameters
      if (_port != 0 && _port != 2) {
         Log::error("Lpc17xxUart: illegal port: %d", _port);
         throw theInternalDationSignal;
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
         throw theInternalDationSignal;
      }

      if (bitsPerCharacter < 5 || bitsPerCharacter > 8) {
         Log::error("Lpc17xxUart: illegal number of bits per character: %d",
                    bitsPerCharacter);
         throw theInternalDationSignal;
      }

      if (stopBits < 1 || stopBits > 2) {
         Log::error("Lpc17xxUart: illegal number of stop bits: %d",
                    stopBits);
         throw theInternalDationSignal;
      }

      if (parity != 'O' && parity != 'E' && parity != 'N') {
         Log::error("Lpc17xxUart: illegal parity: %c",
                    parity);
         throw theInternalDationSignal;
      }

      // setup uart
      switch (_port) {
      case 0:
         Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0);
         Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxingUART0,
                   sizeof(pinmuxingUART0) / sizeof(pinmuxingUART0[0]));
         lpcUart = LPC_UART0;

         break;

      case 2:
         Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART2);
         Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxingUART2,
                  sizeof(pinmuxingUART2) / sizeof(pinmuxingUART2[0]));
         lpcUart = LPC_UART2;
         break;
      }

      Chip_UART_Init((LPC_USART_T*)lpcUart);
      Chip_UART_SetBaud((LPC_USART_T*)lpcUart, baudRate);

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

      Chip_UART_ConfigData((LPC_USART_T*)lpcUart, data);

      Chip_UART_SetupFIFOS((LPC_USART_T*)lpcUart,
                           UART_FCR_FIFO_EN |   // enable
                           UART_FCR_RX_RS  |    // reset rx fifo
                           UART_FCR_TX_RS  |    // reset tx fifo
                           UART_FCR_TRG_LEV0);  // int on each rx char

      Chip_UART_TXEnable((LPC_USART_T*)lpcUart);
      Chip_UART_IntEnable((LPC_USART_T*)lpcUart,
                                UART_IER_RBRINT |
                                UART_IER_RLSINT |
                                UART_IER_THREINT);


   }

   void Lpc17xxUartInternal::interruptEnable(bool on) {
      if (on) {
         if ((LPC_USART_T*)lpcUart == LPC_UART0) {
            NVIC_SetPriority(UART0_IRQn, 10);
            NVIC_EnableIRQ(UART0_IRQn);
         } else if ((LPC_USART_T*)lpcUart == LPC_UART2) {
            NVIC_SetPriority(UART2_IRQn, 10);
            NVIC_EnableIRQ(UART2_IRQn);
         }
      } else {
         if ((LPC_USART_T*)lpcUart == LPC_UART0) {
            NVIC_DisableIRQ(UART0_IRQn);
         } else if ((LPC_USART_T*)lpcUart == LPC_UART2) {
            NVIC_DisableIRQ(UART2_IRQn);
         }
      }
   }

   void Lpc17xxUartInternal::irqHandler(int uartIndex) {
      // uartObjects must exist when an interrupt occurs

      switch (uartIndex) {
      case 0:
         uartObject[0]->treatInterrupt();
         break;

      case 2:
         uartObject[1]->treatInterrupt();
         break;
      }
   }

   void Lpc17xxUartInternal::treatInterrupt() {
      int lineStatus;

      // read interrupt reason and clear additional bits
      int intIdReg = Chip_UART_ReadIntIDReg((LPC_USART_T*)lpcUart)
                     & UART_IIR_INTID_MASK;

      // line status interrupt
      if (intIdReg == UART_IIR_INTID_RLS) {
         lineStatus = Chip_UART_ReadLineStatus((LPC_USART_T*)lpcUart);

         if (lineStatus & UART_LSR_BI) {
            // break indicator
            status |= GenericUart::BREAK_INDICATOR;
         } else {
            if (lineStatus & UART_LSR_FE) {
               // framing error
               status |= GenericUart::FRAME_ERROR;
            }

            if (lineStatus & UART_LSR_OE) {
               // receive overrun
               status |= GenericUart::RECEIVE_OVERRUN;
            }

            if (lineStatus & UART_LSR_PE) {
               // receive parity error
               status |= GenericUart::PARITY_ERROR;
            }
         }

         if (lineStatus & UART_LSR_RDR) {
            // add pending char to receive buffer
            doReceiveChar(Chip_UART_ReadByte((LPC_USART_T*)lpcUart));
         }
      }

      // receive data available interrupt
      if (intIdReg == UART_IIR_INTID_RDA) {
         doReceiveChar(Chip_UART_ReadByte((LPC_USART_T*)lpcUart));
      }

      /* character timeout interrupt is not needed, since we
         trigger the interrupt on each singe character FIFO_TRGLEVEL
      if (intIdReg & UART_IIR_INTID_CTI) {
      }
      */

      // transmitter empty interrupt
      if (intIdReg == UART_IIR_INTID_THRE) {
         // just try to send next char.
         // if there is nothing to send, the routine
         // updates the status variable and returns
         sendNextChar();
      }
   }

   void Lpc17xxUartInternal::sendChar(char ch) {
      Chip_UART_SendByte((LPC_USART_T*)lpcUart, ch);
      status |= GenericUart::TX_INTERRUPT_PENDING;
   }

   void Lpc17xxUartInternal::triggerOutput() {
      // test, whether an TX-interrupt is pending. If not,
      // retrigger the output by sending of the next character
      if (!(status & GenericUart::TX_INTERRUPT_PENDING)) {
         sendNextChar();
      }
   }

   void Lpc17xxUartInternal::sendCharPolling(char ch) {
      while ((Chip_UART_ReadLineStatus((LPC_USART_T*)lpcUart)
              & UART_LSR_TEMT) == 0);

      Chip_UART_SendByte((LPC_USART_T*)lpcUart, ch);
   }

   char Lpc17xxUartInternal::readCharPolling() {
      char ch;

      while ((Chip_UART_ReadLineStatus((LPC_USART_T*)lpcUart)
              & UART_LSR_RDR) == 0);

      ch = Chip_UART_ReadByte((LPC_USART_T*)lpcUart);
      return (ch);
   }

   int Lpc17xxUartInternal::getErrorStatus() {
      return status;
   }
}

