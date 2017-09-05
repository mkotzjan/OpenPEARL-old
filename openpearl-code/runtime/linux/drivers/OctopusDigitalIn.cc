/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014      Rainer Mueller
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


#include "OctopusDigitalIn.h"
#include "Log.h"
#include "Signals.h"
/**
 \brief Implementation of OctopusDigitalIn Systemdation


*/
namespace pearlrt {

   OctopusDigitalIn::OctopusDigitalIn(char port, int start, int width) :
      port(port), start(start), width(width) {
      dationStatus = CLOSED;
      octo = Octopus::getInstance();
      octo->usePortBits(port, start, width, Octopus::DIGIN);
   }

   OctopusDigitalIn::~OctopusDigitalIn() {
      octo->freePortBits(port, start, width);
   }

   void OctopusDigitalIn::internalDationOpen() {
      if (dationStatus != CLOSED) {
         Log::error("OctopusDigitalIn: Dation already open");
         throw theOpenFailedSignal;
      }

      dationStatus = OPENED;
   }
   void OctopusDigitalIn::internalDationClose() {
     
      if (dationStatus != OPENED) {
         Log::error("OctopusDigitalOut: Dation not open");
         throw theCloseFailedSignal;
      }

      dationStatus = CLOSED;
   }

   void OctopusDigitalIn::dationWrite(void* data, size_t size) {
      throw theInternalDationSignal;
   }

   void OctopusDigitalIn::dationRead(void* data, size_t size) {
      int d;

      //check size of parameter!
      // it is expected that a BitString<width> object is passed
      // with a maximum of 8 bits. This fits into 1 byte.
      // Therefore size must be 1
      if (size != 1) {
         Log::error("OctopusDigitalIn: 1 byte expected");
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("OctopusDigitalIn: Dation not open");
         throw theDationNotOpenSignal;
      }

      // expect BitString<width> as data
      d = octo->readPortBits(port, start, width);
      d <<= 8 - width;
      // write data to application memory
      *(char*)data = d;
   }

}

