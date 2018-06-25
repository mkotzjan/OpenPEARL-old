/*
 [A "BSD license"]
 Copyright (c) 2015      Rainer Mueller
 Copyright (c) 2018      Michael Kotzjan
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

#include "Esp32BME280.h"
#include "Log.h"
#include "Signals.h"

extern "C" {
   #include "bme280_wrapper.h"
}

/**
 \brief Implementation of Esp32BME280 Systemdation


*/

namespace pearlrt {

   Esp32BME280::Esp32BME280(int sda, int scl) {
      dationStatus = CLOSED;
      init_i2c(sda, scl);
   }

   SystemDationB* Esp32BME280::dationOpen(const char* idf,
         int openParam) {
      if (idf != 0) {
         Log::error("IDF not allowed for Esp32BME280 device");
         throw theDationParamSignal;
      }

      if (openParam != IN) {
         Log::error("No open parameters except IN allowed for Esp32BME280 device");
         throw theDationParamSignal;
      }

      if (dationStatus != CLOSED) {
         Log::error("Esp32BME280: Dation already open");
         throw theOpenFailedSignal;
      }

      if (!init_bme280()) {
         Log::error("Esp32BME280: init_bme280() returned error");
         throw theOpenFailedSignal;
      }

      dationStatus = OPENED;
      return this;
   }

   void Esp32BME280::dationClose(int closeParam) {

      if (dationStatus != OPENED) {
         Log::error("Esp32BME280: Dation not open");
         throw theDationNotOpenSignal;
      }
      deinit_bme280();

      dationStatus = CLOSED;
   }

   void Esp32BME280::dationWrite(void* data, size_t size) {
      Log::error("Esp32BME280: dationWrite not allowed");
      throw theInternalDationSignal;
   }

   void Esp32BME280::dationRead(void* data, size_t size) {
      if (size != 12) { // 3*sizeof(int32_t)
         Log::error("Esp32BME280: dationRead: size does not match 12");
      }
      read_bme280(data);

   }

   int Esp32BME280::capabilities() {
      return IN;
   }

}

