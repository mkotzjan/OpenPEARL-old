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

#ifndef I2CBUS_INCLUDED
#define I2CBUS_INCLUDED

#include "I2CProvider.h"
#include "Mutex.h"

/**
\file
*/


namespace pearlrt {
   /**
   \addtogroup io_linux_driver
   @{
   */

   /**
   \brief an I2CBusProvider using the linux device node /dev/i2c-x

    long test jsjsjs sds ds
    jkl asd jl
  
   */
   class I2CBus : public I2CProvider {
   private:
      int i2c_file;   // file decriptor for the i2c-connection
      Mutex mutex;    // only one i2c-operation at one time
      // (set addr and read) must be atomic
   public:
      /**
      create the I2C-bus device on the named interface

      \param deviceName is the name of the device (e.g. /dev/i2c-1)

      \throws DeviceNotFoundSignal if the device is not accessible
      \throws DationParamSignal if the device is not accessible
      */
      I2CBus(const char * deviceName);

      /**
      read n bytes of data from the i2c device
      \param adr addres of the i2c device
      \param n maximum number of expected data bytes
      \param data array of data elements
      \throws InternalDationSignal if the slave adress is not setable
      \throws ReadingFailedSignal if a read problem was detected
      \returns number of receive data bytes
      */
      int readData(int adr, int n, uint8_t * data);

      /**
      write n bytes of data to the i2c device
      \param adr addres of the i2c device
      \param n number of data bytes to be written
      \param data array of data elements
      \returns number of transmitted data bytes
      \throws InternalDationSignal if the slave adress is not setable
      \throws WritingFailedSignal if a write problem was detected
      */
      int writeData(int adr, int n, uint8_t * data);

      /**
      perform several i2c action without intermediate stop condition.
      This feature need the capability of REPEATED START from the i2c controller
      If an implementation of a concrete I2CProvider does not support this
      feature, a NULL implementation must be implemented and the absence of
      this feature denoted in the XML-definition of the device

      \param n number of transactions
      \param data array of transactions
      \throws ReadingFailedSignal if a read problem was detected
      */
      void rdwr(int n, I2CMessage* data);
   };
   /** @} */
}
#endif

