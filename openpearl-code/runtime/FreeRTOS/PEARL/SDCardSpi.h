/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

/**
\file

\brief Generic SD-Card support
*/

#ifndef SDCARD_INCLUDED
#define SDCARD_INCLUDED
namespace pearlrt {
   /** generic class for SD-card mass storage devices

   This class needs to be specialized for a concrete sd card device.
   The specialization must support the input and output operations via
   spi.

   During the construction of the specialized sd-card object, the
   constructor of this class is called. The sd-card is registered
   in the FAT file system as volume 0: automatically.
   The first file operation on this volume will detect, that the
   sd-card is not initialized and will perform the card detection.
   This mechanisme is located in Elm Chan's file system routines.

   The routines of Chan are pure C-code. In order to get access to
   the C++-object, the ctor memorizes the objects adress - similar to the
   singleton pattern. Via the static getInstance() method, the C-functions
   get access to the concrete spezialized SDCard object.

   Note: \n
   Setup and operation strategy is decribed in
   http://elm-chan.org/docs/mmc/mmc_e.html

   For details in the specification see:
      https://ww.sdcard.org/downloads/pls;
        SD Specifications
             Part 1
          Physical Layer
       Simplified Specification
            V6.0
       April 10,2017
   */
   class SDCardSpi {
   private:
      static SDCardSpi * obj;

   public:
      SDCardSpi();

      /**
      accessor method to find the (single) object of the sdcard
      driver from the C-world

      \returns pointer to the SDcard driver object
      */
      static SDCardSpi* getInstance();

      /**
      test if card is inserted via the card detect line.

      \returns 1 if card is inserted\n 0 else
      */
      virtual int spiIsCardInserted() = 0;

      /**
      The SPI interface has a chip-select line. This method controls this
      signal. This Method must be implemented by each concrete sdcard
      interface.

      \param select if 1, the chip select line goes low\n
                    if 0, the chip select line goes high
      */
      virtual void spiSelect(int select) = 0;

      /**
      The SPI interface may be used with different bit rates.
      This method controls this feature.
      This Method must be implemented by each concrete sdcard
      interface.

      \param highSpeed if 1, the high transmission rate is used\n
                    if 0, low transmission rate is used
      */
      virtual void spiHighSpeed(int highSpeed) = 0;

      /**
      Send and/or receive one byte via spi to the sd card.

      In order to receive a byte the parameter out is set to 0x0ff.

      \param out the data to be sent
      \returns the received byte
      */
      virtual char spiRwByte(char out) = 0;

      /**
      send a block of byte (usually a complete sector) to the sd card.

      This method should be implemented with interrupt or dma transfer.

      \param buffer the adress of the memory segment to be transmitted
      \param nbr the number of bytes to transmit
      */
      virtual void spiSendBlock(char * buffer, int nbr) = 0;

      /**
      read a block of byte (usually a complete sector) from the sd card.

      This method should be implemented with interrupt or dma transfer.

      \param buffer the adress of the memory segment to be read
      \param nbr the number of bytes to read
      */
      virtual void spiReadBlock(char * buffer, int nbr) = 0;

   private:

      /**
      wait by polling of the spi input, until the sdcard replies
      */
      int wait4Ready();

      /**
      perform one SD-Card command

      at the beginning the CS-line is activated (low)
      it must be released at the end of the complete operation.
      This happens not in this method.

      Some commands trigger the sdcard to send some data
      (like CMD58 - send OCR).
      These data appear with a start value which is different to 0xff.

      \param cmd the command byte
      \param arg a 32 bit parameter for the command
      \param respLength the response data. Must be 0 if no response is expected
      \param respBuffer the responded data.
              Must be NULL, if no response is expected
      \returns the R1 response to the command\n 0 if failed
      */
      int sendCommand(int cmd, int arg, int respLength, char respBuffer[]);


      /**
      perform one SD-Card application specific command
      \param cmd the (real) command byte
      \param arg a 32 bit parameter for the command
      \returns the R1 response to the command\n 0 if if failed
      */
      int sendACommand(int cmd, int arg, int respLength, char respBuffer[]);

      /**
      read a block of data after the start token 0xfe.

      This kind of data is eg the csd and cid buffers.

      \param buffer buffer for the incoming data
      \param len number of bytes to receive
      \returns 1, of ok \n other for fail
      */
      int readData(char * buffer, int len);

      /**
      read a sector of data
       * wait for ready token (0xfe)
      * then start reading

      \param buffer adress of the data to be written
      \param len number of bytes to send
      \return 1, if ok\n others if fails
      */
      int readSector(char * buffer, int len);

      /**
      write a sector of data

      It is possible to write one sector or to write multiple sectors.
      Even for writing of multiple sectors, each sector data mus be preceeded
      with a 0xfc token. A single sector must be preceeded with a 0xfc token.
      The transmission of these tokens must happen in the calling routine.

      \param buffer adress of the data to be written
      \param len number of bytes to send
      \return 1, if ok\n others if fails
      */
      int writeSector(char * buffer, int len);

   public:
      /**
         initialize sd card communication and detect the card type.

         \returns 0 if fail\n 1 if success
      */
      int initializeCard();


      /**
      retrieve card characteristics
         - sector size
         - sector count
         - erase block size as number of sectors
         - ocr (test voltage range)
         - cid (dump card characteristics to log)
           the cid structure is described in
           http://www.getusb.info/how-to-read-cid-on-sd-card and
           http://users.ece.utexas.edu/~valvano/EE345M/
           SD_Physical_Layer_Spec.pdf
         - csd  (gather the card size)

      \return 1, if success\n 0 if fail
      */
      int readConfig();

   public:
      /**
      read sectors

      \param b pointer to the storage, where the be read are placed
      \param sector the starting sector number
      \param count the number of sectors to read

      \returns 1, of ok\n 0, if fail
      */
      int sdDiskReadSectors(void * b, int sector, int count);


      /**
      write sectors

      \param b pointer to the storage, which data should be written
      \param sector the starting sector number
      \param count the number of sectors to write

      \returns 1, of ok\n 0, if fail
      */
      int sdDiskWriteSectors(void * b, int sector, int count);

   };
}
#endif
