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

#ifndef I2PROVIDER_INCLUDED
#define I2PROVIDER_INCLUDED

#include <stdint.h>

/**
\file
*/

namespace pearlrt {
   /**
   \brief Interface to the platform dependent I2C bus controllers.
   */
   class I2CProvider {
   public:
      /**
      Flags for i2c commands
      */
      enum I2CMessageFlags {
         /** this transfer element must be read from the device
         if not set, the transfer element is weritten to the device
         */
         READ = 0x01, 
         /** if set, use 10 bit adressing 
         */
         TENBIT = 0x10
      };

      /**
      transfer command structure.

      Each transfer element needs the same addres.
      The flags are defined in I2CMessageFlags
      */
      struct I2CMessage {
         uint16_t addr; ///< i2c slave adress
         uint16_t flags; ///< identical values to the linux i2c system
         uint16_t len;  ///< number of bytes to transfer
         uint8_t * data; ///< pointer to the transfer buffer
      };

      /**
      read n bytes of data from the i2c device
      \param adr addres of the i2c device
      \param n maximum number of expected data bytes
      \param data array of data elements
      \returns number of receive data bytes
      */
      virtual int readData(int adr, int n, uint8_t * data) = 0;

      /**
      write n bytes of data to the i2c device
      \param adr addres of the i2c device
      \param n number of data bytes to be written
      \param data array of data elements
      \returns number of transmitted data bytes
      */
      virtual int writeData(int adr, int n, uint8_t * data) = 0;

      /**
      perform several i2c action without intermediate stop condition.
      This feature need the capability of REPEATED START from the i2c controller
      If an implementation of a concrete I2CProvider does not support this
      feature, a NULL implementation must be implemented and the absence of
      this feature denoted in the XML-definition of the device

      \param n number of transactions
      \param data array of transactions
      */
      virtual void rdwr(int n, I2CMessage* data) = 0;
   };
}
#endif

