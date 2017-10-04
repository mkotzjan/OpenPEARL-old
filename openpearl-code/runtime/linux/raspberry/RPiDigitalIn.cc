/*
 [A "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2016      Rainer Mueller
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


#include "RPiDigitalIn.h"
#include "RPiGpio.h"
#include "Log.h"
#include "Signals.h"
/**
 \brief Implementation of RPiDigitalIn Systemdation


*/
namespace pearlrt {

   RPiDigitalIn::RPiDigitalIn(int start, int width, char*pud) :
      start(start), width(width) {
      dationStatus = CLOSED;

      if (strcmp(pud, "u") == 0) {
         RPiGpio::instance()->useBits(start, width,
                                      RPiGpio::DIGIN, RPiGpio::UP);
      } else if (strcmp(pud, "d") == 0) {
         RPiGpio::instance()->useBits(start, width,
                                      RPiGpio::DIGIN, RPiGpio::DOWN);
      } else if (strcmp(pud, "n") == 0) {
         RPiGpio::instance()->useBits(start, width,
                                      RPiGpio::DIGIN, RPiGpio::NONE);
      } else {
         Log::error("RPiDigitalIn: illegal pull up/down value (%s)", pud);
         throw theDationParamSignal;
      }
   }

   RPiDigitalIn::~RPiDigitalIn() {
   }

   SystemDationB* RPiDigitalIn::dationOpen(const char * idf, int openParam) {
      if (idf != 0) {
         Log::error("IDF not allowed for RPi digital out device");
         throw theDationParamSignal;
      }

      if ((openParam & (~IN))  != 0) {
         Log::error("No open parameters allowed for RPi digital "
                    "in device (%x)", openParam);
         throw theDationParamSignal;
      }

      if (dationStatus != CLOSED) {
         Log::error("RPiDigitalIn: Dation already open");
         throw theOpenFailedSignal;
      }

      dationStatus = OPENED;

      return this;
   }

   void RPiDigitalIn::dationClose(int closeParam) {
      if (closeParam != 0) {
         Log::error("No close parameters allowed for RPiDigitalIn device");
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("RPiDigitalIn: Dation not open");
         throw theDationNotOpenSignal;
      }

      dationStatus = CLOSED;
   }


   void RPiDigitalIn::dationWrite(void* data, size_t size) {
      throw theInternalDationSignal;
   }

   void RPiDigitalIn::dationRead(void* data, size_t size) {
      int d;

      //check size of parameter!
      // it is expected that a BitString<width> object is passed
      // with a maximum of 32 bits. This fits into 4 byte.
      // Therefore size must be less than 4
      if (size > 4) {
         Log::error("RPiDigitalIn: max 32 bits expected");
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("RPiDigitalIn: Dation not open");
         throw theDationParamSignal;
      }

      // readBits delivers data left adjusted
      d = RPiGpio::instance()->readBits(start, width);

      switch (size) {
      case 1:
         *(char*)data = d >> 24;
         break;

      case 2:
         *(int16_t*)data = d >> 16;
         break;

      case 4:
         *(int32_t*)data = d;
         break;

      default:
         // we must cast to int, since Log does not support %u
         Log::error("RPiDigitalIn: illegal size (%d)", (int)size);
         throw theInternalDationSignal;
      }

   }

   int RPiDigitalIn::capabilities() {
      int cap = IN | ANY;
      return cap;
   }

}
