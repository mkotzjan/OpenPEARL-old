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

#ifndef LPC17XXUART_INCLUDED
#define LPC17XXUART_INCLUDED

#include "SystemDationNB.h"
//#include "chip.h"
#include "RingBuffer.h"
#include "CSema.h"

namespace pearlrt {
   /**
   \file

   \brief uart support for LPC17xx
   */

   /**
   This class provides uart support for the LPC17xx

   The dation initializes the uart device to the required
   line characteristic. If an input buffer is required, it
   is allocated on the FreeRTOS-heap.

   The interrupt is only enabled if the device is opened.

   There is no check if more than one userdateion is opened
   on the device. It should no problem, since the operations
   are secured by a mutex.

   */
   class Lpc17xxUart : public SystemDationNB {
   private:
      void* lpc_uart;  // pointer to port (in LPCOpen)
      bool lineEdit;
      bool xonProtocol;
      Mutex mutex;
      CSema writeSema;
      CSema readSema;
      int nbrOpenUserDations;
      int status;    // status of the current operation
      // for details see enum Lpc17xxUartStatus
      static Lpc17xxUart * uartObject[2];
      struct Job4Isr {
         char * data;
         int nbr;
         int nbrReceived;
         SemaphoreHandle_t blockSema;
      } sendCommand, recvCommand;

      char lineEditEchoBuffer[10];  // --> rename towards echoBuffer
      RingBuffer<char> lineEditEcho;
      char bufferedInputChar;
      char unGetChar;

   public:
      /**
      define the uart device

      \param port the number (0 or 2)
      \param baudRate the baud rate; all usual values from 300 to 115200 are
        accepted
      \param bitsPerCharacter  5-8  is allowed
      \param stopBits number of stop bits
      \param parity the desired type of parity ('O', 'E', 'N')
      \param mode multiplex parameter
                  <ul>
                  <li>Bit 0: 1= line edit on
                  <li>Bit 1: 1=xon/xoff protocol enabled
                  </ul>
      If lineEdit is selected, dationRead and dationWrite may only
      be used exclusive. Simultaneous calls for read and write are
      sequenced internally.

      If no lineEdit is selected, the dation works in full duplex mode.
      Note that xoff-sending has higher priority than data sending.

      \throws theIllegalParamSignal in case of illegal parameter values
      */
      Lpc17xxUart(int port, int baudRate, int bitsPerCharacter,
                  int stopBits, char parity, int mode);

      /**
       open the system dation

       the dation may be opened multiple times in order to
       use it in different configurations. Eg. as error log and
       user console.
       If the device is used for multiple purposes the operations
       may mix.

       \param idfValue is not supported should be NULL
       \param openParam nothing supported; should be 0
      */
      Lpc17xxUart * dationOpen(const char * idfValue, int openParam);

      void  dationClose(int closeParam);

      int capabilities();
      void dationRead(void * destination, size_t size);
      void dationWrite(void * destination, size_t size);
      void dationUnGetChar(const char c);

      static void irqHandler(int uartIndex);
   private:
      void treatInterrupt();
      void doRecvChar();
      bool sendNextChar();
      void interruptEnable(bool on);
      void logError();

   };
}

#endif

