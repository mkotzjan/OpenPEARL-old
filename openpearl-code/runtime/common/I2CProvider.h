#ifndef I2PROVIDER_INCLUDED
#define I2PROVIDER_INCLUDED

#include <stdint.h>


namespace pearlrt {
   class I2CProvider {
   public:
      /**
      Flags for i2c commands
      */
      enum I2CMessageFlags {
         READ = 0x01, ///< this transfer element must be read from the device
         ///< if not set, the transfer element is weritten to the device
         TENBIT = 0x10
      }; ///< use 10 bit adressing

      /**
      transfer command structure.

      Each transfer element needs the same addres.
      The flags are defined in I2CMessageFlags
      */
      struct I2CMessage {
         uint16_t addr; ///< i2c slave addres
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

