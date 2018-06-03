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

#ifndef ESP32UART_INCLUDED
#define ESP32UART_INCLUDED

#include "SystemDationNB.h"
#include "Esp32UartInternal.h"
#include "GenericUartDation.h"

//#include "chip.h"
//#include "RingBuffer.h"
#include "CSema.h"
#include "Mutex.h"

namespace pearlrt {
   /**
   \file

   \brief uart support for Esp32
   */

   /**
   This class provides uart support for the Esp32

   The interrupt is only enabled if the device is opened.

   There is no check if more than one userdation is opened
   on the device. It should no problem, since the operations
   are secured by a mutex.


   As connection to a terminal with line edit functions.

   console: LineEdit(80) --- Esp32Uart(....)

   The LineEdit class uses a different interface as the pur dation
   operation, which supplies a raw mode. All data are transfered as specified.
   No early termination on input is possible.
   The line may be used in full duplex
   mode.

   Besides these modes of operation, the communication protocol xon/xoff
   may be selected. This operates in both directions.
   On output, the reception of an XOFF will stop the transmission of more
   characters until an XON is received.
   On input, an XOFF is emitted in case of the reception of a data byte
   with no active input request. The next input action (dationRead()) will
   take the buffered input character and sends an XON.
   The implementation of XonXoff is provided plattform indepoendent in
   GenericUart

   To achieve mutual exclusion between interrupt service routine and
   API function calls, the specific NVI interrupt is cleared and
   set accordingly.

   The UART dation allows multiple open and close operations.
   This provied the possibility to have a normal USERDATION on the
   interface as well as the logging output.

   */
   class Esp32Uart : public GenericUartDation, public SystemDationNB {
   private:
      Mutex mutex; 	// mutex for objects data
      CSema writeSema;  // semaphore to resume operation from interrupt
      CSema readSema;  	// semaphore to resume operation from interrupt
      int nbrOpenUserDations; // multiple dations counter
      int status;  	// status of the current operation
      // for details see enum Esp32UartStatus

      Esp32UartInternal * internalUart;
      struct Job4Isr {
         char * data;
         int nbr;
         int nbrReceived;
         FakeSemaphoreHandle_t blockSema;
      } sendCommand, recvCommand;

      char bufferedInputChar;	// one char may be buffered even if no
      // input is active to run xon/xoff protocol
      // this stores the last received character
      char unGetChar;		// character for unget

   public:
      /**
      define the uart device

      \param port the number (0 or 2)
      \param baudRate the baud rate; all usual values from 300 to 115200 are
        accepted
      \param bitsPerCharacter  5-8  is allowed
      \param stopBits number of stop bits
      \param parity the desired type of parity ('O', 'E', 'N')

      \throws theInternalDationSignal in case of illegal parameter values
                               or if the rxbuffer could not be allocated
      */
      Esp32Uart(int port, int baudRate, int bitsPerCharacter,
                  int stopBits, char* parity, bool xon);

      /**
       open the system dation

       the dation may be opened multiple times in order to
       use it in different configurations. Eg. as error log and
       user console.
       If the device is used for multiple purposes the operations
       may mix.

       \param idfValue is not supported should be NULL
       \param openParam nothing supported; should be 0
       \returns pointer to this object as working object
      */
      Esp32Uart * dationOpen(const char * idfValue, int openParam);

      void  dationClose(int closeParam);

      int capabilities();
      void dationRead(void * destination, size_t size);
      void dationWrite(void * destination, size_t size);
      void dationUnGetChar(const char c);

      /**
      translate newline<br>
      in input: CR -> \n<br>
      in output: \n -> CR+LF

      \param doNewLineTranslation true enables the translation, <br>
              false disables the translation
      */
      void translateNewLine(bool doNewLineTranslation); 
   private:
      void treatInterrupt();
      void doRecvChar();
      bool sendNextChar();
      void interruptEnable(bool on);
      void logError(int status);

      bool getNextTransmitChar(char * ch);
      bool addReceivedChar(char ch);

   };
}

#endif

