/*
 [A "BSD license"]
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


#include "Dation.h"
#include "PCan.h"
#include "Log.h"
#include "Signals.h"
#include <iostream>
#include <libpcan.h>
#include <fcntl.h> /* O_RDWR */

/**
 \file 
 \brief Implementation of the PEAK CAN device systemdation


*/
namespace pearlrt {

   PCan::PCan(char *devicePath, int speed) {
      deviceNode = devicePath;

      // set the bitrate parameter depending to the transmission speed
      // only some values are supported by the driver
      // the values are given in libpcan.h (or bitratetest -f=/dev/pcan32)
      switch (speed) {
         default:
            Log::error("PCan: bitrate (%d) not supported", speed);
            throw theDationParamSignal;
         case 125000:
           bitRate = CAN_BAUD_125K;
           break;
         case 250000:
           bitRate = CAN_BAUD_250K;
           break;
         case 500000:
           bitRate = CAN_BAUD_500K;
           break;
         case 1000000:
           bitRate = CAN_BAUD_1M;
           break;
      }

      openCount = 0;
      dationStatus = CLOSED;

   }

   SystemDationB* PCan::dationOpen(const char * idf, int params) {
      int ret;

      if (idf) {
         Log::error("PCan: no IDF allowed");
         throw theDationParamSignal;
      }

      if (params & ~(RST | IN | OUT | INOUT)) {
         Log::error("PCan: only RST allowed");
         throw theDationParamSignal;
      }


      if (openCount == 0) {
         h = LINUX_CAN_Open(deviceNode, O_RDWR);
         if (h == NULL) {
             Log::error("PCan: can't open device %s", deviceNode);
	     throw theOpenFailedSignal;
         }

         // use standard frames (11Bit ID)
         ret = CAN_Init(h, bitRate, CAN_INIT_TYPE_ST);
         if (ret) {
            Log::error("PCan: error in CAN_Init");
	     throw theDationParamSignal;
         }
      }

      openCount ++;   
      dationStatus = OPENED;
      return this;
   }

   void PCan::dationClose(int params) {

      if (dationStatus != OPENED) {
         Log::error("PCan: Dation not open");
         throw theCloseFailedSignal;
      }

      if (params & ~(RST | IN | OUT | INOUT)) {
         Log::error("PCan: only RST allowed");
         throw theDationParamSignal;
      }

      openCount --;
      if (openCount  == 0) {
         CAN_Close(h);
         dationStatus = CLOSED;
      } 
   }

   void PCan::dationWrite(void* data, size_t size) {
      TPCANMsg m;
      Can2AMessage * d = (Can2AMessage*) data;
      int ret;

      // check size of parameter!
      if (size != sizeof(Can2AMessage)) {
         Log::error("PCan: %d byte expected (got %d)",
                    (int)(sizeof(Can2AMessage)), (int)size);
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("PCan: Dation not open");
         throw theDationNotOpenSignal;
      }

      m.ID = d->identifier.x;
      if (d->dataLength.x > 8) {
          Log::error("PCan: write length too large (%d)", d->dataLength.x);
          throw theDationParamSignal;
      }
      m.LEN = d->dataLength.x;
      if (d->rtrRequest.x) {
         // is RTR-Request
         m.MSGTYPE = MSGTYPE_RTR;
      } else {
         m.MSGTYPE = MSGTYPE_STANDARD;
         for (int i=0; i<m.LEN; i++) {
            m.DATA[i] = d->data[i].x;
         }
      }

      // CAN_Write blocks until data is written
      ret = CAN_Write(h, &m);
      if (ret) {
          Log::error("PCan: write error (%04x)", ret);
          throw theWritingFailedSignal;
      }
   }

   void PCan::dationRead(void* data, size_t size) {

      TPCANMsg m;
      Can2AMessage * d = (Can2AMessage*) data;
      int ret;

      // check size of parameter!
      if (size != sizeof(Can2AMessage)) {
         Log::error("PCan: %d byte expected (got %d)",
                    (int)(sizeof(Can2AMessage)), (int)size);
         throw theDationParamSignal;
      }

      if (dationStatus != OPENED) {
         Log::error("PCan: Dation not open");
         throw theDationNotOpenSignal;
      }

      // CAN_Reade blocks until data is available
      // and discard status messages
      do {
         ret = CAN_Read(h, &m);
         if (ret) {
             Log::error("PCan: read error (%4x)", ret);
             throw theReadingFailedSignal;
         }
      } while (m.MSGTYPE == MSGTYPE_STATUS); 

      d->identifier.x = m.ID;
      if (m.MSGTYPE == MSGTYPE_RTR) {
        d->rtrRequest.x = 0x80; // set to true
      } else if (m.MSGTYPE == MSGTYPE_STANDARD) {
        d->rtrRequest.x = 0x00; // set to false
      } else {
          Log::error("PCan: unexpected message type received (%d)", m.MSGTYPE);
          throw theReadingFailedSignal;
      }

      d->dataLength.x = m.LEN;

      for (int i=0; i<m.LEN; i++) {
         d->data[i].x = m.DATA[i];
      }

   }

   int PCan::capabilities() {
      int cap =  IN  | OUT | INOUT;
      return cap;
   }
}

