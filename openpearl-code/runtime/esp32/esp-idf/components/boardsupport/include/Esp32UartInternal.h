#ifndef ESP32UARTINTERNAL_INCLUDED
#define ESP32UARTINTERNAL_INCLUDED
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

#include "FreeRTOS.h"
#include "GenericUart.h"
extern "C" {
#include "freertos/queue.h"
#include "driver/uart.h"
}

namespace pearlrt {

   /**
   \brief Esp32 uart  low level support with RTS/CTS hardware control support

   This class is implemented as singleton, since it is also responsible
   to supply the support for printf via the retarget-mechanisme of the
   glibc. During the initialization of retarget the UART0-interface is
   setup with default values.
   */
   class Esp32UartInternal : public GenericUart {

   private:
      static Esp32UartInternal* uartObject[2];
      void * espUart;    // use void pointer to remove chip specific stuff
      // from generated PearlIncludes.h
      QueueHandle_t uart_queue;

      /**
      ctor of the native uart support

      \param _port the number of the uart device (0 and 2 are allowed)
      \param baudRate transmission speed in bits/seconds (baud). Only
            some prefined values are allowed
      \param  bitsPerCharacter number of bits for one character (5-8 is ok)
      \param stopBits number of stop bits (1 or 2 is ok)
      \param parity ('O', 'E', or 'N' are ok)
      \param xon RTS/CTS hardware control selected (true = enable, false not enabled)
      */
      Esp32UartInternal(int _port, int baudRate,
                          int bitsPerCharacter, int stopBits,
                          char parity, bool hc);

   public:

      /**
      get access to the selected uart object. If the object does not exist
      it is created with the given parameters.

      \param _port the number of the uart device (0 and 2 are allowed)
      \param baudRate transmission speed in bits/seconds (baud). Only
            some prefined values are allowed
      \param  bitsPerCharacter number of bits for one character (5-8 is ok)
      \param stopBits number of stop bits (1 or 2 is ok)
      \param parity ('O', 'E', or 'N' are ok)
      \param xon RTS/CTS hardware control selected (true = enable, false not enabled)
      */
      static Esp32UartInternal* getInstance(int _port,
                                              int baudRate,
                                              int bitsPerCharacter,
                                              int stopBits, char parity,
                                              bool hc);

      /**
       interrupt control of the UART device

       \param on  true enables the interrupts, false disables the interrupts
      */
      void interruptEnable(bool on);

      /**
      the content of the interrupt service routine for the
      selected uart index

      \param uartIndex number of the uart, which triggered the interrupt.
          Valid numbers are 0 and 2
      */
      static void irqHandler(int uartIndex);

      void setup(int _port, int baudRate,
                 int bitsPerCharacter, int stopBits, char parity);


      /**
      restart output, if necessary
      */
      void triggerOutput();

      /**
      return the error status to the superior uart layer
      \returns the error status as bit array of type GenericUartStatus
      */
      int getErrorStatus();

   private:
      void treatInterrupt();

      /**
      send the given character

      \param ch character to transmit
      */
      void sendChar(char ch);

      /** send a character in polling mode
      \param ch the character to send
      */
      void sendCharPolling(char ch);

      /** read a character in polling mode
      \returns  the character read
      */
      char readCharPolling();


   };
}
#endif

