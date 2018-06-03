/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
 Copyright (c) 2018 Michael Kotzjan
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

#include "Esp32UartInternal.h"

#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

//#include "Esp32Uart.h"
#include "Log.h"

#define BUF_SIZE (1024)

extern "C" {
   void UART0_IRQHandler(void) {
      pearlrt::Esp32UartInternal::irqHandler(0);
   }

   void UART2_IRQHandler(void) {
      pearlrt::Esp32UartInternal::irqHandler(2);
   }
};

namespace pearlrt {

   /**
   \brief Esp32 uart  low level support with RTS/CTS hardware control support

   */

   static const int validBaudRates[] = { 300, 600, 1200,
                                         2400, 4800, 9600,
                                         19200, 38400, 57600, 115200
                                       };

   Esp32UartInternal* Esp32UartInternal::uartObject[2] = {NULL, NULL};

   Esp32UartInternal::Esp32UartInternal(int _port, int baudRate,
         int bitsPerCharacter, int stopBits, char parity, bool hc)
      : GenericUart(hc, _port == 0) {
      setup(_port, baudRate, bitsPerCharacter, stopBits, parity);
   }

   Esp32UartInternal* Esp32UartInternal::getInstance(int _port,
         int baudRate, int bitsPerCharacter, int stopBits, char parity,
         bool hc) {
      if (_port != 0 && _port != 2) {
         Log::error("Esp32UartInternal: illegal port number (%d)", _port);
         throw theInternalDationSignal;
      }

      if (!uartObject[_port / 2]) {
         uartObject[_port / 2] = new Esp32UartInternal(
            _port, baudRate, bitsPerCharacter, stopBits, parity, hc);
      }

      return (uartObject[_port / 2]);
   }

   void Esp32UartInternal::setup(int _port, int baudRate,
                                   int bitsPerCharacter, int stopBits,
                                   char parity) {
      int i;
      bool found;

      // test parameters
      if (_port != 0 && _port != 2) {
         Log::error("Esp32Uart: illegal port: %d", _port);
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
         Log::error("Esp32Uart: illegal baud rate: %d",
                    baudRate);
         throw theInternalDationSignal;
      }

      if (bitsPerCharacter < 5 || bitsPerCharacter > 8) {
         Log::error("Esp32Uart: illegal number of bits per character: %d",
                    bitsPerCharacter);
         throw theInternalDationSignal;
      }

      if (stopBits < 1 || stopBits > 2) {
         Log::error("Esp32Uart: illegal number of stop bits: %d",
                    stopBits);
         throw theInternalDationSignal;
      }

      if (parity != 'O' && parity != 'E' && parity != 'N') {
         Log::error("Esp32Uart: illegal parity: %c",
                    parity);
         throw theInternalDationSignal;
      }

      // setup uart
      switch (_port) {
      case 0:
         espUart = (void *)UART_NUM_0;

         break;

      case 2:
         espUart = (void *)UART_NUM_2;
         break;
      }

      uart_set_baudrate(*(uart_port_t*)espUart, baudRate);

      if (stopBits == 1) {
         uart_set_stop_bits(*(uart_port_t*)espUart, UART_STOP_BITS_1);
      }

      if (stopBits == 2) {
         uart_set_stop_bits(*(uart_port_t*)espUart, UART_STOP_BITS_2);
      }

      if (bitsPerCharacter == 5) {
         uart_set_word_length(*(uart_port_t*)espUart, UART_DATA_5_BITS);
      }

      if (bitsPerCharacter == 6) {
         uart_set_word_length(*(uart_port_t*)espUart, UART_DATA_6_BITS);
      }

      if (bitsPerCharacter == 7) {
         uart_set_word_length(*(uart_port_t*)espUart, UART_DATA_7_BITS);
      }

      if (bitsPerCharacter == 8) {
         uart_set_word_length(*(uart_port_t*)espUart, UART_DATA_8_BITS);
      }

      if (parity == 'E') {
         uart_set_parity(*(uart_port_t*)espUart, UART_PARITY_EVEN);
      }

      if (parity == 'O') {
         uart_set_parity(*(uart_port_t*)espUart, UART_PARITY_ODD);
      }

      if (parity == 'N') {
         uart_set_parity(*(uart_port_t*)espUart, UART_PARITY_DISABLE);
      }

      uart_set_pin(*(uart_port_t*)espUart, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

      uart_driver_install(*(uart_port_t*)espUart, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart_queue, 0);

   }

   void Esp32UartInternal::interruptEnable(bool on) {
      if (on) {
         uart_enable_intr_mask(*(uart_port_t*)espUart, 0x7FFFF);
      } else {
         uart_disable_intr_mask(*(uart_port_t*)espUart, 0x0);
      }
   }

   void Esp32UartInternal::irqHandler(int uartIndex) {
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

   void Esp32UartInternal::treatInterrupt() {
      uart_event_t event;
      
      if(xQueueReceive(uart_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
         switch(event.type) {
            case UART_DATA:
               
               break;
            case UART_BREAK:
               // break indicator
               status |= GenericUart::BREAK_INDICATOR;
               break;
            case UART_PARITY_ERR:
               // receive parity error
               status |= GenericUart::PARITY_ERROR;
               break;
            case UART_FRAME_ERR:
               // framing error
               status |= GenericUart::FRAME_ERROR;
               break;
            case UART_FIFO_OVF:
               // receive overrun
               status |= GenericUart::RECEIVE_OVERRUN;
               break;
            default:
               // don't handle other interrupts at the moment
               break;
         }
      }

      // receive data available interrupt
      //if (intIdReg == UART_IIR_INTID_RDA) {
      //   doReceiveChar(Chip_UART_ReadByte((LPC_USART_T*)lpcUart));
   }

   void Esp32UartInternal::sendChar(char ch) {
      uart_write_bytes(*(uart_port_t*)espUart, &ch, sizeof(ch));
      status |= GenericUart::TX_INTERRUPT_PENDING;
   }

   void Esp32UartInternal::triggerOutput() {
      // test, whether an TX-interrupt is pending. If not,
      // retrigger the output by sending of the next character
      if (!(status & GenericUart::TX_INTERRUPT_PENDING)) {
         sendNextChar();
      }
   }

   void Esp32UartInternal::sendCharPolling(char ch) {
   }

   char Esp32UartInternal::readCharPolling() {
      return '\0';
   }

   int Esp32UartInternal::getErrorStatus() {
      return status;
   }
}

