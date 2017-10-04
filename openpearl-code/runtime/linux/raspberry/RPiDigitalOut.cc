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


#include "RPiDigitalOut.h"
#include "RPiGpio.h"
#include "Log.h"
#include "Signals.h"
/**
 \brief Implementation of RPiDigitalOut Systemdation


*/
namespace pearlrt {

   RPiDigitalOut::RPiDigitalOut(int start, int width) :
      start(start), width(width) {
      dationStatus = CLOSED;
      RPiGpio::instance()->useBits(start, width,
                                   RPiGpio::DIGOUT, RPiGpio::NONE);
   }

   RPiDigitalOut::~RPiDigitalOut() {
   }

   SystemDationB* RPiDigitalOut::dationOpen(const char * idf, int openParam) {
      if (idf != 0) {
         Log::error("IDF not allowed for RPi digital out device");
         throw theDationParamSignal;
      }

      if ((openParam & (~OUT))  != 0) {
         Log::error("No open parameters allowed for RPi digital"
                    " out device (%x)", openParam);
         throw theDationParamSignal;
      }

      if (dationStatus != CLOSED) {
         Log::error("RPiDigitalOut: Dation already open");
         throw theOpenFailedSignal;
      }

      dationStatus = OPENED;

      return this;
   }

   void RPiDigitalOut::dationClose(int closeParam) {
      if (closeParam != 0) {
         Log::error("No close parameters allowed for RPiDigitalOut device");
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("RPiDigitalOut: Dation not open");
         throw theDationNotOpenSignal;
      }

      dationStatus = CLOSED;
   }


   void RPiDigitalOut::dationRead(void* data, size_t size) {
      throw theDationParamSignal;
   }

   void RPiDigitalOut::dationWrite(void* data, size_t size) {
      int d;

      //check size of parameter!
      // it is expected that a BitString<width> object is passed
      // with a maximum of 32 bits. This fits into 4 byte.
      // Therefore size must be less than 4
      if (size > 4) {
         Log::error("RPiDigitalOut: max 32 bits expected");
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("RPiDigitalOut: Dation not open");
         throw theDationParamSignal;
      }

      // expect BitString<width> as data
      // bits are left adjusted in data, thus the data must be
      // shifted according the concrete size of data
      switch (size) {
      case 1:
         d = (*(char*)data) << 24;
         break;

      case 2:
         d = (*(int16_t*)data) << 16;
         break;

      case 4:
         d = (*(int32_t*)data);
         break;

      default:
         // we must cast to int, since Log does not support %u
         Log::error("RPiDigitalOut: illegal size (%d)", (int)size);
         throw theInternalDationSignal;
      }

      // shifting the data to the correct position occurs in writeBits
      RPiGpio::instance()->writeBits(start, width, d);
   }

   int RPiDigitalOut::capabilities() {
      int cap = OUT | ANY;
      return cap;
   }

}
