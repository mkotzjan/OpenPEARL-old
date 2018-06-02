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

//#include "chip.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "Esp32Uart.h"
#include "Esp32UartInternal.h"
#include "Log.h"


namespace pearlrt {


   static const int  READ_ERRORMASK =
      (GenericUart::BREAK_INDICATOR |
       GenericUart::PARITY_ERROR |
       GenericUart::FRAME_ERROR |
       GenericUart::RECEIVE_OVERRUN);


   Esp32Uart::Esp32Uart(int _port, int baudRate, int bitsPerCharacter,
                            int stopBits, char* parity, bool xon) :
      writeSema(1), readSema(1) {

      internalUart = Esp32UartInternal::getInstance(_port, baudRate,
                     bitsPerCharacter, stopBits, *parity, xon);

      // enforces these parameters, if the default setting was used before
      internalUart->setup(_port, baudRate,
                          bitsPerCharacter, stopBits, *parity);

      internalUart->registerUartDation(this);

      status = 0;  // not busy
      sendCommand.blockSema = xSemaphoreCreateBinary();
      recvCommand.blockSema = xSemaphoreCreateBinary();

      if (sendCommand.blockSema == 0) {
         Log::error("Esp32Uart: could not create block sema (send)");
         throw theInternalDationSignal;
      }

      if (recvCommand.blockSema == 0) {
         Log::error("Esp32Uart: could not create block sema (recv)");
         throw theInternalDationSignal;
      }

      mutex.name("Esp32Uartx");
      nbrOpenUserDations = 0;

   }

   void Esp32Uart::logError(int errorStatus) {
      // echo only one error reason, eg. BREAK will often
      // come together with PARITY_ERROR
      if (errorStatus & GenericUart::BREAK_INDICATOR) {
         Log::error("Esp32Uart: break received");
      } else if (errorStatus & GenericUart::FRAME_ERROR) {
         Log::error("Esp32Uart: frame error");
      } else if (errorStatus & GenericUart::PARITY_ERROR) {
         Log::error("Esp32Uart: parity error");
      } else  if (errorStatus & GenericUart::RECEIVE_OVERRUN) {
         Log::error("Esp32Uart: receive overrun");
      }
   }

   int Esp32Uart::capabilities() {
      return (Dation::FORWARD | Dation::ANY | Dation::PRM |
              Dation::IN | Dation::OUT | Dation::INOUT);
   }

   Esp32Uart* Esp32Uart::dationOpen(const char * idfValue,
                                        int openParams) {
      if (openParams & (Dation::IDF | Dation::CAN)) {
         Log::error("Esp32Uart: does not support IDF and CAN");
         throw theDationParamSignal;
      }

      mutex.lock();

      nbrOpenUserDations++;

      // enable interrupt, if this is the first open on this dation
      if (nbrOpenUserDations == 1) {
         internalUart->interruptEnable(true);
      }

      mutex.unlock();

      return this;
   }

   void Esp32Uart::dationClose(int closeParams) {

      mutex.lock();

      if (nbrOpenUserDations == 0) {
         Log::error("Esp32Uart: no dation opened");
         mutex.unlock();
         throw theCloseFailedSignal;
      }

      nbrOpenUserDations--;

      // disable interrupt, if this is the last close on this dation
      if (nbrOpenUserDations == 0) {
          internalUart->interruptEnable(false);
      }

      if (closeParams & Dation::CAN) {
         Log::error("Esp32Uart: CAN not supported");
         mutex.unlock();
         throw theDationParamSignal;
      }

      mutex.unlock();
   }

   bool Esp32Uart::addReceivedChar(char ch) {

      /* this method is called from the character receive isr
      */
      if (recvCommand.nbr > 0) {
         *(recvCommand.data++) = ch;
         recvCommand.nbr--;
         recvCommand.nbrReceived ++;

         if (recvCommand.nbr == 0) {
            xSemaphoreGiveFromISR(recvCommand.blockSema, NULL);
         }

         return true;
      } else {
         bufferedInputChar = ch;
         status |= GenericUart::RXCHAR_IS_BUFFERED;
         return false;
      }
   }

   void Esp32Uart::dationRead(void * destination, size_t size) {
      bool errorExit = false;
      int errorStatus;

      if (nbrOpenUserDations == 0) {
         Log::error("Esp32Uart: not opened");
         throw theDationNotOpenSignal;
      }

      readSema.request();
      mutex.lock();

      // disable uart interrupt and treat buffered input
      internalUart->interruptEnable(false);
      status |= GenericUart::READ_IS_ACTIVE;
      recvCommand.data = (char*)destination;
      recvCommand.nbr = size;
      recvCommand.nbrReceived = 0;

      //internalUart->sendXonIfNecessary();

      if (status & GenericUart::HAS_UNGETCHAR) {
         *(recvCommand.data++) = unGetChar;
         recvCommand.nbr--;
         recvCommand.nbrReceived ++;
         status &= ~GenericUart::HAS_UNGETCHAR;
      }

      if (status & GenericUart::RXCHAR_IS_BUFFERED && recvCommand.nbr > 0) {
         *(recvCommand.data++) = bufferedInputChar;
         recvCommand.nbr--;
         recvCommand.nbrReceived ++;
         status &= ~GenericUart::RXCHAR_IS_BUFFERED;
      }

      if (recvCommand.nbr  > 0) {
         internalUart->interruptEnable(true);
         // uart interrupt enabled --> wait until data are ready

         xSemaphoreTake(recvCommand.blockSema, portMAX_DELAY);
         internalUart->interruptEnable(false);
      }

      errorStatus = internalUart->getErrorStatus();
      if ((errorStatus & READ_ERRORMASK) != 0) {
         errorExit = true;
         logError(errorStatus);
      }

      status &= ~READ_ERRORMASK;
      status &= ~GenericUart::READ_IS_ACTIVE;
      internalUart->interruptEnable(true);

      mutex.unlock();

      readSema.release();

      if (errorExit) {
         throw theReadingFailedSignal;
      }

   }

   void Esp32Uart::dationWrite(void * destination, size_t size) {
      bool errorExit = false;
      int errorStatus;

      if (nbrOpenUserDations == 0) {
         Log::error("Esp32Uart: not opened");
         throw theDationNotOpenSignal;
      }

      // request semaphores in ordered locking write->read
      writeSema.request();

      mutex.lock();

      internalUart->interruptEnable(false);

      status |= GenericUart::WRITE_IS_ACTIVE;
      sendCommand.data = (char*)destination;
      sendCommand.nbr = size;


      internalUart->interruptEnable(true);
      internalUart->triggerOutput();

      xSemaphoreTake(sendCommand.blockSema, portMAX_DELAY);
      internalUart->interruptEnable(false);

      errorStatus = internalUart->getErrorStatus();
      if ((errorStatus & READ_ERRORMASK) != 0) {
         errorExit = true;
         logError(errorStatus);
      }

      status &= ~READ_ERRORMASK;
      status &= ~GenericUart::WRITE_IS_ACTIVE;

      internalUart->interruptEnable(true);

      mutex.unlock();
      writeSema.release();

      if (errorExit) {
         throw theWritingFailedSignal;
      }

   }

   bool Esp32Uart::getNextTransmitChar(char * ch) {
      if (sendCommand.nbr > 0) {
         *ch = *(sendCommand.data++);
         sendCommand.nbr --;

         if (sendCommand.nbr == 0) {
            xSemaphoreGiveFromISR(sendCommand.blockSema, NULL);
         }

         return true;
      }

      return false;
   }

   void Esp32Uart::dationUnGetChar(const char c) {
      mutex.lock();
      unGetChar = c;
      status |= GenericUart::HAS_UNGETCHAR;
      mutex.unlock();
   }

   void Esp32Uart::translateNewLine(bool doNewLineTranslation) {
      // just delegate to the generic uart driver
      internalUart->translateNewLine(doNewLineTranslation);
   }

}

