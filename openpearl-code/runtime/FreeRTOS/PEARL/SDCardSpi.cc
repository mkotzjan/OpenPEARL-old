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

/*
\file

\brief generic SD-Card support 
*/

#include "FreeRTOS.h"
#include "task.h"    // vTaskDelay for non busy wait and vTaskCreate
#include "allTaskPriorities.h"

#include "SDCardSpi.h"
#include "Log.h"
#include "Signals.h"
#include "ff.h"
#include "diskio.h"  //registerDisk.h"
#include "FatFs.h"
#include "FatFsVolume.h"
#include "chip.h"
#include "FreeRTOS.h"

//#define DEBUG

/**
\brief the volume name of sd cards

The volume name must be unique for all FatFs participants.
In OpenPearl, SDCards are accessed via 0:
*/
#define VOLUMENAME "0:/"

/**
\brief number of ticks to delay, if reply is not prompt

the value of 2 seems to be a good value in respect of performance and
cpu load 
*/
#define DELAY  2 

/**
\brief the sector size

we work with a fixed sector size, independent of the setting on the sd card
*/
#define SECTOR_SIZE 512


extern "C" {
   static int status = STA_NOINIT;
   static int sectorCount = -1;
   static int blockSize = -1;


   /* Disk Drive miscellaneous Functions */
   static DRESULT chanDiskIoctl(BYTE drv, BYTE ctrl, void *buff) {
      DRESULT res;

      if (drv != DEV_SD) {
         return RES_PARERR;
      }

      if (status & STA_NOINIT) {
         return RES_NOTRDY;
      }

      res = RES_ERROR;

      switch (ctrl) {
      case CTRL_SYNC:	/* Make sure that no pending write process */
         // nothing to do as lonag as we don not use interrupts
         res = RES_OK;
         break;

      case GET_SECTOR_COUNT:	/* Get number of sectors on the disk (DWORD) */
         *(DWORD *) buff = sectorCount;
         res = RES_OK;
         break;

      case GET_SECTOR_SIZE:	/* Get R/W sector size (WORD) */
         *(WORD *) buff = SECTOR_SIZE;
         res = RES_OK;
         break;

      case GET_BLOCK_SIZE:/* Get erase block size in unit of sector (DWORD) */
         *(DWORD *) buff = blockSize;
         res = RES_OK;
         break;

      default:
         res = RES_PARERR;
         break;
      }

      return res;
   }

   /* Read Sector(s) */
   static DRESULT chanDiskRead(BYTE drv, BYTE *buff,
                               DWORD sector, UINT count) {
      pearlrt::SDCardSpi * o;

      if (drv != DEV_SD || !count) {
         return RES_PARERR;
      }

      if (status & STA_NOINIT) {
         return RES_NOTRDY;
      }


      o = pearlrt::SDCardSpi::getInstance();

      if (o->sdDiskReadSectors(buff, sector, count)) {
         return RES_OK;
      }

      return RES_ERROR;
   }

   /* Get Disk Status */
   static DSTATUS chanDiskStatus(BYTE drv) {
      if (drv != DEV_SD) {
         return STA_NOINIT;	/* Supports only single drive */
      }

      return status;
   }

   /* Write Sector(s) */
   static DRESULT chanDiskWrite(BYTE drv, const BYTE *buff,
                                DWORD sector, UINT count) {
      pearlrt::SDCardSpi * o;

      if (drv != DEV_SD || !count) {
         return RES_PARERR;
      }


      if (status & STA_NOINIT) {
         return RES_NOTRDY;
      }

      o = pearlrt::SDCardSpi::getInstance();

      if (o->sdDiskWriteSectors((void *) buff, sector, count)) {
         return RES_OK;
      }

      return RES_ERROR;
   }
}

namespace pearlrt {
   static pearlrt::FatFsVolume volume(VOLUMENAME);
   SDCardSpi* SDCardSpi::obj = NULL;

   /* ----------------- */
   static DSTATUS chanDiskInitialize(BYTE drv);


   // -----------------------------
   // SdCard specific helper defines
#define SD_OK    1      ///< return code OK for commands
#define SD_FAIL  0      ///< return code FAIL for commands


#define CMD0 (0x40+0)   ///< go idle state 
#define CMD1 (0x40+1)   ///< send op for mmc
#define CMD8 (0x40+8)   ///< send if condition
#define CMD9 (0x40+9)   ///< send csd (Card Specific Data register)
#define CMD10 (0x40+10)  ///< send cid (Card IDendification register)
#define CMD13 (0x40+13)  ///< send status  (as app cmd for sd card)
#define CMD12 (0x40+12)  ///< send stop transmission
#define CMD16 (0x40+16) ///< set block length
#define CMD17 (0x40+17) ///< read block
#define CMD18 (0x40+18) ///< read multiple blocks
#define CMD24 (0x40+24) ///< write block
#define CMD25 (0x40+25) ///< write multiple blocks
#define CMD41 (0x40+41) ///< send operation condition
#define CMD55 (0x40+55) ///< app command (for sd card)
#define CMD58 (0x40+58) ///< send ocr
#define CMD59 (0x40+59) ///< crc on/off

// detected card types

/**
\brief the detected card type

The card detection results into one of the values
SDNOCARD, SDV1, SDV2SC, SDV2HC
*/
#define SDNOCARD 0
#define SDMMC 1    ///< MMC cards are detected but not supported
#define SDV1  2    ///< SD Card Version 1 (< 1GB)
#define SDV2SC 3   ///< SD Card Version 2 (1GB - 8GB)
#define SDV2HC 4   ///< SD Card Version 2 > 8GB

//#define SECTOR_SIZE 512		// we work with a fixed sector size of 512 byte

/**
\brief R1 response with no error

The R1 response signals the command result status.
*/
#define SD_R1_NO_ERROR			0x00

/**
\brief bit mask indicating that the card is in idle state in R1 response
*/
#define SD_R1_IDLE_STATE 		0x01

/**
\brief bit mask indicating that the command was not understood by the card
in R1 response
*/
#define SD_R1_ILLEGAL_CMD		0x04

   static int blockSize = -1;
   static int cardType = SDNOCARD;
   static int sectorCount;
//static int status = STA_NOINIT;     // state of the sd card communication

   static void sdCardPollTask(void* pvParameters) {
      TickType_t nextActivation;
      static int lastStateIsCardInserted = 0;
      int isCardInserted;

      const TickType_t period = 100 / portTICK_PERIOD_MS;

      nextActivation = xTaskGetTickCount();
      Log::info("SDCard-Poll Task started");

      while (1) {
         isCardInserted = pearlrt::SDCardSpi::getInstance()->spiIsCardInserted();
         if (isCardInserted != lastStateIsCardInserted) {
             if (isCardInserted) {
                volume.setVolumeStatus(pearlrt::FatFsVolume::WasInserted);
                pearlrt::Log::info("SDCard inserted");
             } else {
                volume.setVolumeStatus(pearlrt::FatFsVolume::WasRemoved);
                pearlrt::Log::info("SDCard removed");
             } 
             lastStateIsCardInserted = isCardInserted;
         }
         vTaskDelayUntil(&nextActivation, period);
      }
   }


   int SDCardSpi::wait4Ready() {
      int retries = 500;
      int reply;

      spiRwByte(0x0ff); // force eanble D0 output

      do {
         reply = spiRwByte(0x0ff);

//printf("wait4ready: reply=%x\n", reply);
         if (reply == 0xFF) {
            return SD_OK;
         }
      } while (--retries > 0);

      return 0 ;
   }


   int SDCardSpi::sendCommand(int cmd, int arg,
                              int respLength, char respBuffer[]) {
      int8_t crc;
      int reply;
      int i;

      spiSelect(1);   // enshure CS to be low - it is released after

      // tha last command of a sequence
      if (wait4Ready() != SD_OK) {
         //printf("SDCard not ready\n");
         return 0;
      }

      spiRwByte(cmd);
      spiRwByte(arg >> 24); // no need to mask here, this is
      spiRwByte(arg >> 16); // done inside of spiSend
      spiRwByte(arg >> 8);
      spiRwByte(arg);
      // send crc
      crc = 0x01; // dummy value for all other commands

      if (cmd == CMD0) {
         crc = 0x95;
      }

      if (cmd == CMD8) {
         crc = 0x87;
      }

      spiRwByte(crc);

      // command response time is up to 8 bytes read time
      for (i = 0; i < 9; i++) {
         reply = spiRwByte(0x0ff); // get reply

         if (reply != 0x0ff) {
            break;   // got valid reply --> try again
         }
      }
#ifdef DEBUG
      if (i > 5) {
         printf("cmd = %d data=%x R1-response=%x, i=%d \n",
                cmd - 0x40, arg, reply, i);
      }
#endif

      if (respLength && respBuffer) {
         do {
            *respBuffer ++ = spiRwByte(0xff);
         } while (--respLength);
      }

      return reply;
   }

   /**
   read data block with wait for start token 0xfe

   \param buffer buffer for the incoming data
   \param len number of bytes to receive
   \returns 1, of ok \n other for fail
   */
   int SDCardSpi::readData(char * buffer, int len) {
      char data;
      int attempts = 1000;

      do {
         data = spiRwByte(0xff);

         if (data == 0xfe) {
            break;
         }
      } while (--attempts);
#ifdef DEBUG
      printf("readData: attempts=%d data=%x\n", attempts, data);
#endif

      if (data != 0xfe) {
         return 0;   // fail
      }

      spiReadBlock(buffer, len);

      // discard crc
      spiRwByte(0xff);
      spiRwByte(0xff);
      return 1;
   }

   /**
   read a data block

   * wait for ready token (0xfe)
   * then start reading

   \param buffer adress of the data to be written
   \param len number of bytes to send
   \return 1, if ok\n others if fails
   */
   int SDCardSpi::readSector(char * buffer, int len) {
      int reply;
      int i;

      for (i = 0; i < 10000000; i++) {
         reply = spiRwByte(0xff);

         if (reply == 0xfe) {
            break;
         }

         if (i > 10) {
            vTaskDelay(DELAY);  // non busy wait
         }
      }

#ifdef DEBUG
      if ( i > 10) {
        printf("read sector: end of wait: i=%d\n", i);
      }
#endif

      if (reply != 0xfe) {
//         printf("readSector: fe not received\n");
         return SD_FAIL;
      }

      spiReadBlock(buffer, len);

      // discard crc
      spiRwByte(0xff);
      spiRwByte(0xff);
      return SD_OK;
   }

   /**
   write a data block

   the token (single or multiple) block must have been sent in advance
   the block will be transmitted
   waits until data is stored

   \param buffer adress of the data to be written
   \param len number of bytes to send
   \return 1, if ok\n others if fails
   */
   int SDCardSpi::writeSector(char * buffer, int len) {
      int reply;
      int i;

      // send data block
      spiSendBlock(buffer, len);

      // send dummy crc, since we do not use crc
      spiRwByte(0xff);
      spiRwByte(0xff);

      // read response after complete block
      reply = spiRwByte(0xff);

      if ((reply & 0x0f) != 0x05) {
         // wrong reply
         //printf("writeSector: reply=%x\n", reply);
         return FALSE;
      }

      // wait until data is written
      for (i = 0; i < 100000; i++) {
         reply = spiRwByte(0xff);

         if (reply == 0xff) {
            break;
         }

         if (i > 10) {
            vTaskDelay(DELAY);  // non busy wait
         }
      }

#ifdef DEBUG
      if ( i > 10) {
         printf("write sector: end of wait: i=%d\n", i);
      }
#endif

      if (reply == 0xff) {
         return SD_OK;
      }

      //printf("write block: write timeout\n");
      return SD_FAIL;
   }


   int SDCardSpi::sendACommand(int cmd, int arg,
                               int respLength, char respBuffer[]) {
      int reply = sendCommand(CMD55, 0, 0, NULL);

      if (reply == SD_R1_IDLE_STATE || reply == SD_R1_NO_ERROR) {
         // first part succeeded --> send real command
         reply = sendCommand(cmd, arg, respLength, respBuffer);
      }

      return reply;
   }

   int SDCardSpi::initializeCard() {
      int i;
      int reply;
      char response[4];

      //printf("try to detect card type\n");
      spiSelect(0);  // deselect disk
      spiHighSpeed(0); // init must be done in low speed mode

      // give card type to settle
      for (i = 0; i < 16; i++) {
         spiRwByte(0xff);
      }

      // reset card via CMD0
      spiSelect(1);  // select disk
      reply = sendCommand(CMD0, 0, 0, NULL);

      if (reply != SD_R1_IDLE_STATE) {
         goto card_not_known;
      }

      // the sd card is in idle state
      // the remaining stuff max be done in high speed mode
      spiHighSpeed(1);

      // detect type of the card as described by Chan and the
      // simplified SD Specification V2.00
      printf("detect card type\n");
      reply = sendCommand(CMD8, 0x01AA, 4, response);

      if (reply == SD_R1_IDLE_STATE) {
         if (response[2] != 0x01 || response[3] != 0x0AA) {
            goto card_not_known;
         }

         // base card type is V2 -- lets check if it is a SC or HC version
         // test ACMD41 with HCS bit set; if this this is in the response
         // it is a HC version
         do {
            reply = sendACommand(CMD41, 1 << 30, 0, NULL);
         } while (reply == SD_R1_IDLE_STATE);

         if (reply > SD_R1_IDLE_STATE) {
            goto card_not_known;
         }

         reply = sendCommand(CMD58, 0, 4, response);

         if (reply != SD_R1_NO_ERROR) {
            goto card_not_known;
         }

         //printf("OCR: %x\n", response[0]);

         if (response[0] & 0x40) {
            cardType = SDV2HC;

            // leave idle state
            while (sendACommand(CMD41, 0, 0, NULL) != SD_R1_NO_ERROR);

            pearlrt::Log::warn("Lpc17xxSdcard: SDHC Card not tested yet");
         } else {
            cardType = SDV2SC;

            // leave idle state
            while (sendCommand(CMD1, 0, 0, NULL) != SD_R1_NO_ERROR);
         }
      } else {
         // it may be sd card of version 1 or mmc
         // test with valid sd command
         reply = sendACommand(CMD55, 0, 0, NULL);

         if (reply == SD_R1_NO_ERROR) {
            cardType = SDV1;
            pearlrt::Log::warn("Lpc17xxSdcard: SD Card Version 1"
                               " not tested yet");
         } else {
            pearlrt::Log::error("Lpc17xxSdcard: MMC card not supported");
            //cardType = SDMMC;  // lets assume to be a MMC card
            cardType = SDNOCARD;  // forget this card
         }
      }

      //printf("card type = %d\n", cardType);
      // final setup
      // cardType is set to something > 0
      // if one of the following commands fail -> forget the card type

      // disable crc
      if (sendCommand(CMD59, 0, 0, NULL)) {
         cardType = SDNOCARD;
      }

      // set rw block length
      if (sendCommand(CMD16, SECTOR_SIZE, 0, NULL)) {
         cardType = SDNOCARD;
      }

      spiSelect(0);
      return 1;   // success

card_not_known:
      spiSelect(0);  // deselect disk
      return 0;
   }

   int SDCardSpi::readConfig() {
      char buffer[16]; // ocr needs 4 bytes, cid and csd need 16 byte
      int i;
      //int voltageRange;
      int serialNumber;
      int cardSize;

      printf("read config\n");

      // read ocr -_> voltage range
      if (sendCommand(CMD58, 0, 4, buffer) != SD_R1_NO_ERROR) {
         goto errorExit;
      }

#ifdef DEBUG
      printf("ocr: %02x %02x %02x %02x\n",
             buffer[0], buffer[1], buffer[2], buffer[3]);
#endif

      //voltageRange = ((buffer[2] >> 7) & 0x01) | (buffer[1] << 1);
//      printf("voltage Range=%x (0x1FF means 3.6-2.7 volts)\n", voltageRange);
      // we work normally with 3.3V.
      // This is supoorted by definition by all cards --> no need to check

      // read cid
      if (sendCommand(CMD10, 0, 0, NULL) != SD_R1_NO_ERROR) {
         goto errorExit;
      }

      readData(buffer, 16);
#ifdef DEBUG
      printf("cid: ");

      for (i = 0; i < 16; i++) {
         printf(" %02x", buffer[i]);
      }
      

      printf("\nManufacturer/OEM-ID: %x/%02x%x02",
             buffer[0], buffer[1], buffer[2]);
      printf("Product: %c%c%c%c%c\n",
             buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
      printf("Revision: %d.%d\n", buffer[8] >> 4, buffer[8] & 0x0f);
#endif
      Log::info("SDCard: %c%c%c%c%c\n",
             buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
      serialNumber = buffer[9] << 24 | buffer[10] << 16 |
                     buffer[11] << 8 | buffer[12];
      Log::info("   Serial Number: %x", serialNumber);
      Log::info("   Manufacture Date: %d/%d",
             buffer[14] & 0x0f,
             ((buffer[13] & 0x0f) << 4) | (((buffer[14] >> 4) & 0x0f) + 2000));

      // read csd
      if (sendCommand(CMD9, 0, 0, NULL) != SD_R1_NO_ERROR) {
         goto errorExit;
      }

      readData(buffer, 16);
#ifdef DEBUG
      printf("\ncsd: ");

      for (i = 0; i < 16; i++) {
         printf(" %02x", buffer[i]);
      }

      printf("\nCSD struct: %d\n", (buffer[0] >> 6) & 0x03);
#endif

      if (((buffer[0] >> 6) & 0x03) != 0) {
         // CSD version V2.0 (high/extended capacity)
         // C_SIZE  BIT(73:62)
         cardSize = (((uint32_t)buffer[7]) << 16) +
                    (((uint32_t)buffer[8]) << 8) +
                    ((uint32_t)buffer[9]);
         cardSize &= 0x3FFFFF;
         // -- CSD Version 2.0
         // RD_BL_LEN is always 512Byte
         // capacity = (C_SIZE+1)*512KB
         // we use a fixed sector size of 512B
         // thus sectore = (C_SIZE+1)*512KB/512 = (C_SIZE +1) * 1024
         sectorCount = (cardSize + 1) * 1024;
//         printf("CSD V2: sectorCount = %d sectors of 512 bytes\n",
//            sectorCount);
      } else {
         int mult, blLen;
         cardSize = (((uint32_t)buffer[6] & 0x03) << 10) +
                    (((uint32_t)buffer[7]) << 2) +
                    ((uint32_t)buffer[8] >> 6);
         cardSize &= 0xFFF;

         mult = ((buffer[9] & 0x03) << 1) + ((buffer[10] >> 7) & 0x01);
         blLen = buffer[5] & 0x0f;

 //        printf("CSD V1: cardSize = %d mult=%d blLen = %d\n", cardSize,
 //               mult, blLen);
         // we use a fixed sector size of 512 bytes this means 2^9 bytes/sect
         // mult is interpreted as 2^(mult+2)
         // cardSize must be inkremented by 1
         sectorCount = (cardSize + 1) << (blLen + mult - 7);
//         printf("sectors: %d of size 512 byte\n", sectorCount);

      }

 //     printf("\nretrieve Blocksize for cardType %d\n", cardType);

      switch (cardType) {
      default:
         pearlrt::Log::error("Lpc17xxSDcard: card type %d not supported",
                             cardType);
         break;

      //case SDV1:  // not supported yet - need card to perform tests
      //   break;
      case SDV2SC:
      case SDV2HC:
         if (sendACommand(CMD13, 0, 1, buffer) != SD_R1_NO_ERROR) {
            goto errorExit;
         }

         // we need the first 16 bytes of the replied data
         if (readData(buffer, 16) == 0) {
            goto errorExit;
         }

         for (i = 0; i < 64 - 16; i++) { // discard the remaining reply
            spiRwByte(0x0ff);
         }

         // select field AU_SIZE (Allocation Unit SIZE)
         //   this is the upper 4 bit in buffer[10]
         // we use 512 recording units per AU
         // AU_SIZE calculation
         // field value   size
         //  1            16kB
         //  2            32kB
         //  ....
         //  9            4MB
         // ==> AU_Size = 8kB << (field)
         // divide AU-Size in 512 recording blocks
         // blockSize = AU_Size >> 9  // divide by 512
         // simplified using 8192 = 1 << 13:
         //  blockSize = 1 << (13-9+AU_field_value)

         //printf("buf[10] = %02x au_size: %d\n", buffer[10], buffer[10] >> 4);
         blockSize = 1UL << (((unsigned)(buffer[10]) >> 4) + 4);
         //printf("SDV2SC/HC: blocksize=%d\n", blockSize);
         break;
      }

      spiSelect(0);
      return 1;

errorExit:
      spiSelect(0);
      return 0;
   }

   int SDCardSpi::sdDiskReadSectors(void * b, int sector, int count) {
      int result = SD_FAIL;
      char * buff = (char*)b;

      if (cardType != SDV2HC) {
         sector <<= 9;   // non HC cards want a byte adress (*512)
      }

      if (count > 1) {
         // read multiple sectors
         if (sendCommand(CMD18, sector, 0, NULL) == SD_R1_NO_ERROR) {
            while (count--) {
               if (readSector(buff, SECTOR_SIZE) != SD_OK) {
                  break;
               }

               buff += SECTOR_SIZE;
            }

            sendCommand(CMD12, 0, 0, NULL); // send 'stop transmission'

            if (wait4Ready() == SD_R1_NO_ERROR) {
               result = SD_OK;
            }
         }
      } else {
         // read single sector
         if (sendCommand(CMD17, sector, 0, NULL) == SD_R1_NO_ERROR) {
            if (readSector(buff, SECTOR_SIZE) == SD_OK) {
               result = SD_OK;
            }
         }
      }

      spiSelect(0);
      return result;
   }

   int SDCardSpi::sdDiskWriteSectors(void * b, int sector, int count) {
      int result = SD_FAIL;
      char * buff = (char*)b;

      if (cardType != SDV2HC) {
         sector <<= 9;   // non HC cards want a byte adress (*512)
      }

      if (count > 1) {
         // write multiple sectors
         if (sendCommand(CMD25, sector, 0, NULL) == SD_R1_NO_ERROR) {
            spiRwByte(0xfc); // send start of multiple block transmission

            while (count--) {
               spiRwByte(0xfc); // send start of multiple block transmission

               if (writeSector(buff, SECTOR_SIZE) == SD_FAIL) {
                  break;
               }

               buff += SECTOR_SIZE;
            }

            spiRwByte(0xfd); // send 'stop transmission'

            if (wait4Ready() == SD_R1_NO_ERROR) {
               result = SD_OK;
            }
         }
      } else {
         // writesingle sector
         if (sendCommand(CMD24, sector, 0, NULL) == SD_R1_NO_ERROR) {
            spiRwByte(0xfe); // send start of single block transmission
            result =  writeSector(buff, SECTOR_SIZE);
         }
      }

      spiSelect(0);
      return result;
   }


   /* Initialize Disk Drive */
   static DSTATUS chanDiskInitialize(BYTE drv) {
      int ret;
      pearlrt::SDCardSpi * o;

      if (drv != DEV_SD) {
         return STA_NOINIT;	/* wrong parameter */
      }


      if (status != STA_NOINIT) {
         return status;			/* card is already detected */
      }

      o = pearlrt::SDCardSpi::getInstance();

      o->spiHighSpeed(0);  // perform detection in low speed mode
      o->spiSelect(0);

      ret =  o->spiIsCardInserted();

      if (ret) {
         // card inserted try to detect card type
         // preset value to be illegal
         cardType = SDNOCARD;
         blockSize = -1;
         sectorCount = -1;
      }

      /* Initialize the Card Data Strucutre */
      if (o->initializeCard()) {
         if (o->readConfig()) {
            pearlrt::Log::info("  type: %d / sectors: %d / blocksize: %d",
                               cardType, sectorCount, blockSize);
            status &= ~STA_NOINIT;
         }
      }

      /* Reset */

      status &= ~STA_NOINIT;
      return status;

   }


   SDCardSpi::SDCardSpi() {
      int ret;
      static struct DiskIoFunctions diof = {
         chanDiskStatus, chanDiskInitialize,
         chanDiskRead, chanDiskWrite,
         chanDiskIoctl
      };
      static TCB_t pollTaskTCB;
      static xTaskHandle pollTaskHandle;

#define STACK_SIZE_SDCARDTASK  1000
      static StackType_t pollTaskStack[STACK_SIZE_SDCARDTASK];

      StructParameters_t createParameters;

      createParameters.pvParameter = NULL;
      createParameters.stack = pollTaskStack;
      createParameters.tcb = &pollTaskTCB;


      /* register the  disk as low level driver for the fat fs */
      ret = disk_registerDrive(DEV_SD, &diof);

      if (ret) {
         Log::error("SDCardSpi: failed to register volume");
         throw theInternalDationSignal;
      }

      FatFs::registerVolume(DEV_SD, &volume);
      obj = this;

      ret = xTaskCreate(sdCardPollTask, "SDCardPollTask",
                        STACK_SIZE_SDCARDTASK,
                        &createParameters, PRIO_TASK_SERVICE, &pollTaskHandle);

      if (ret != pdPASS) {
         pearlrt::Log::error("Lpc17xxSDCard: could not create poll task");
         throw theInternalDationSignal;
      }

   }

   SDCardSpi* SDCardSpi::getInstance() {
      if (obj) {
         return obj;
      }

      Log::error("SDCard::getinstance: oops no valid object");
      throw theInternalDationSignal;
   }

}

