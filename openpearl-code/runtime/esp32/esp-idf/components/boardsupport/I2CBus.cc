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

#include "I2CBus.h"
#include "I2CProvider.h"
#include "Log.h"
#include "Signals.h"

//#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>  // strerror
#include <errno.h>
#include <unistd.h>

namespace pearlrt {
   I2CBus::I2CBus(const char * deviceName) {
   }

   /**
   read n bytes of data from the i2c device
   \param adr addres of the i2c device
   \param n maximum number of expected data bytes
   \param data array of data elements
   \returns number of receive data bytes
   */
   int I2CBus::readData(int adr, int n, uint8_t * data) {
      return 0;
   }

   int I2CBus::writeData(int adr, int n, uint8_t * data) {
      return 0;
   }


   /**
   perform several i2c action without intermediate stop condition.
   This feature need the capability of REPEATED START from the i2c controller
   If an implementation of a concrete I2CProvider does not support this
   feature, a NULL implementation must be implemented and the absence of
   this feature denoted in the XML-definition of the device

   \param n number of transactions
   \param data array of transactions
   */
   void I2CBus::rdwr(int n, I2CProvider::I2CMessage* data) {
   }

}
