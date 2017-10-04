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

#include "Lpc17xxUsb.h"
#include "Lpc17xxUsbDisk.h"
#include "Log.h"
#include "ff.h"
#include "diskio.h"  //registerDisk.h"
#include "FatFs.h"
#include "FatFsVolume.h"

#define VOLUMENAME "1:/"

static SCSI_Capacity_t DiskCapacity;
static pearlrt::FatFsVolume volume(VOLUMENAME);


typedef USB_ClassInfo_MS_Host_t DISK_HANDLE_T;

//static uint8_t buffer[8*1024];
static USB_ClassInfo_MS_Host_t FlashDisk_MS_Interface = {
   .Config = {
      .DataINPipeNumber    = 1,
      .DataINPipeDoubleBank = false,

      .DataOUTPipeNumber = 2,
      .DataOUTPipeDoubleBank = false,

      .PortNumber = 0,
   },
};


extern "C" {

   static void disconnectDevice() {
      volume.setVolumeStatus(pearlrt::FatFsVolume::WasRemoved);
      pearlrt::Log::info("UsbDisk disconnected");

   }

   /* ------------- ChanFS support functions ------------------- */
   /* these functions are influenced by the illustrative code    */
   /* in LPCOpen V2.0 frm NXP Semiconductors
     LPC products.  This software is supplied "AS IS" without any
     warranties of any kind, and NXP Semiconductors and its licensor
     disclaim any and all warranties, express or implied, including
     all implied warranties of merchantability, fitness for a particular
     purpose and non-infringement of intellectual property rights.
     NXP Semiconductors assumes no responsibility  or liability for
     the use of the software, conveys no license or rights under any
     patent, copyright, mask work right, or any other intellectual
     property rights in or to any products.
     NXP Semiconductors reserves the right to make changes
     in the software without notification. NXP Semiconductors also makes no
     representation or warranty that such application will be suitable for the
     specified use without further testing or modification.

     @par
     Permission to use, copy, modify, and distribute this software and its
     documentation is hereby granted, under NXP Semiconductors' and its
     licensor's relevant copyrights in the software, without fee,
     provided that it is used in conjunction with NXP Semiconductors
     microcontrollers.  This copyright, permission, and disclaimer
     notice must appear in all copies of this code.
    */


   /* Get the disk data structure */
   static DISK_HANDLE_T *FSUSB_DiskInit(void) {
      return &FlashDisk_MS_Interface;
   }



   /* Wait for disk to be inserted */
   static int FSUSB_DiskInsertWait(DISK_HANDLE_T *hDisk) {
      while (USB_HostState[hDisk->Config.PortNumber]
             != HOST_STATE_Configured) {
         MS_Host_USBTask(hDisk);
         USB_USBTask(hDisk->Config.PortNumber, USB_MODE_Host);
      }

      return 1;
   }



   /* Disk acquire function that waits for disk to be ready */
   static int FSUSB_DiskAcquire(DISK_HANDLE_T *hDisk) {
      pearlrt::Log::info("Waiting for ready...");

      for (;;) {
         uint8_t ErrorCode = MS_Host_TestUnitReady(hDisk, 0);

         if (!(ErrorCode)) {
            break;
         }

         /* Check if an error other than a logical command error (device busy)
            was received */
         if (ErrorCode != MS_ERROR_LOGICAL_CMD_FAILED) {
            //pearlrt::Log::info("Failed");
            // try to switch to default configuration
            USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
            return 0;
         }
      }

      pearlrt::Log::info("Done.");

      if (MS_Host_ReadDeviceCapacity(hDisk, 0, &DiskCapacity)) {
         pearlrt::Log::error("Error retrieving device capacity.");
         // try to switsch to default configuration
         USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
         return 0;
      }

      pearlrt::Log::info("%u blocks of %u bytes.\r\n",
                         (unsigned)DiskCapacity.Blocks,
                         (unsigned)DiskCapacity.BlockSize);
      return 1;
   }



   /* Get sector count */
   static uint32_t FSUSB_DiskGetSectorCnt(DISK_HANDLE_T *hDisk) {
      return DiskCapacity.Blocks;
   }



   /* Get Block size */
   static uint32_t FSUSB_DiskGetSectorSz(DISK_HANDLE_T *hDisk) {
      return DiskCapacity.BlockSize;
   }



   /* Read sectors */
   static int FSUSB_DiskReadSectors(DISK_HANDLE_T *hDisk,
                                    void *buff, uint32_t secStart,
                                    uint32_t numSec) {
printf("FSUSB: sec=%x numsec=%d\n", (unsigned int)secStart, (int)numSec);
      if (MS_Host_ReadDeviceBlocks(hDisk, 0, secStart, numSec,
                                   DiskCapacity.BlockSize, buff)) {
         // pearlrt::Log::error("Error reading device block.");
         // try to switsch to default configuration
         USB_Host_SetDeviceConfiguration(
            FlashDisk_MS_Interface.Config.PortNumber, 0);
         return 0;
      }

      return 1;
   }



   /* Write Sectors */
   static int FSUSB_DiskWriteSectors(DISK_HANDLE_T *hDisk,
                                     void *buff,
                                     uint32_t secStart,
                                     uint32_t numSec) {
      if (MS_Host_WriteDeviceBlocks(hDisk, 0, secStart, numSec,
                                    DiskCapacity.BlockSize, buff)) {
         pearlrt::Log::error("Error writing device block.");
         return 0;
      }

      return 1;
   }



   /* Disk ready function */
   static int FSUSB_DiskReadyWait(DISK_HANDLE_T *hDisk, int tout) {
      volatile int i = tout * 100;

      while (i--) {	/* Just delay */
      }

      return 1;
   }

   /* Erase block size fixed to 4K */
#define FSUSB_DiskGetBlockSz(hDisk)         (4 * 1024)

   /* ------------------------------------------------------- */

   /* Disk Status */
   static volatile DSTATUS Stat = STA_NOINIT;

   static DISK_HANDLE_T *hDisk;

   /*************************************************************************
    * Private functions
    ************************************************************************/

   /* Initialize Disk Drive */
   static DSTATUS usb_disk_initialize(BYTE drv) {
      if (drv != DEV_USB) {
         return STA_NOINIT;	/* wrong parameter */
      }


      if (Stat != STA_NOINIT) {
         return Stat;			/* card is already enumerated */
      }

      /* Initialize the Card Data Strucutre */
      hDisk = FSUSB_DiskInit();

      /* Reset */
      Stat = STA_NOINIT;

      FSUSB_DiskInsertWait(hDisk); /* Wait for card to be inserted */

      /* Enumerate the card once detected.
         Note this function may block for a little while. */
      if (!FSUSB_DiskAcquire(hDisk)) {
         // printf("Disk Enumeration failed...\r\n");
         return Stat;
      }

      Stat &= ~STA_NOINIT;
      return Stat;

   }

   /* Disk Drive miscellaneous Functions */
   static DRESULT usb_disk_ioctl(BYTE drv, BYTE ctrl, void *buff) {
      DRESULT res;

      if (drv != DEV_USB) {
         return RES_PARERR;
      }

      if (Stat & STA_NOINIT) {
         return RES_NOTRDY;
      }

      res = RES_ERROR;

      switch (ctrl) {
      case CTRL_SYNC:	/* Make sure that no pending write process */
         if (FSUSB_DiskReadyWait(hDisk, 50)) {
            res = RES_OK;
         }

         break;

      case GET_SECTOR_COUNT:	/* Get number of sectors on the disk (DWORD) */
         *(DWORD *) buff = FSUSB_DiskGetSectorCnt(hDisk);
         res = RES_OK;
         break;

      case GET_SECTOR_SIZE:	/* Get R/W sector size (WORD) */
         *(WORD *) buff = FSUSB_DiskGetSectorSz(hDisk);
         res = RES_OK;
         break;

      case GET_BLOCK_SIZE:/* Get erase block size in unit of sector (DWORD) */
         *(DWORD *) buff = FSUSB_DiskGetBlockSz(hDisk);
         res = RES_OK;
         break;

      default:
         res = RES_PARERR;
         break;
      }

      return res;
   }

   /* Read Sector(s) */
   static DRESULT usb_disk_read(BYTE drv, BYTE *buff,
                                DWORD sector, UINT count) {
      if (drv != DEV_USB || !count) {
         return RES_PARERR;
      }

      if (Stat & STA_NOINIT) {
         return RES_NOTRDY;
      }


      if (FSUSB_DiskReadSectors(hDisk, buff, sector, count)) {
         return RES_OK;
      }

      return RES_ERROR;
   }

   /* Get Disk Status */
   DSTATUS usb_disk_status(BYTE drv) {
      if (drv != DEV_USB) {
         return STA_NOINIT;	/* Supports only single drive */
      }

      return Stat;
   }

   /* Write Sector(s) */
   static DRESULT usb_disk_write(BYTE drv, const BYTE *buff,
                                 DWORD sector, UINT count) {

      if (drv != DEV_USB || !count) {
         return RES_PARERR;
      }


      if (Stat & STA_NOINIT) {
         return RES_NOTRDY;
      }

      if (FSUSB_DiskWriteSectors(hDisk, (void *) buff, sector, count)) {
         return RES_OK;
      }

      return RES_ERROR;
   }

   /* ----------------- */

   /*
   In this method will take place the enumeration.
   It will be called from the EVENT_USB_Host_DeviceEnumerationComplete function
   */
   static void Lpc17xxUsbDiskCompleteEnumeration(const uint8_t corenum,
         size_t ConfigDescriptorSize,
         void* ConfigDescriptorData) {

      FlashDisk_MS_Interface.Config.PortNumber = corenum;

      if (MS_Host_ConfigurePipes(&FlashDisk_MS_Interface,
                                 ConfigDescriptorSize, ConfigDescriptorData)
            != MS_ENUMERROR_NoError) {
         pearlrt::Log::error("Attached device is not a valid mass storage.");
         return;
      }

      if (USB_Host_SetDeviceConfiguration(corenum, 1)
            != HOST_SENDCONTROL_Successful) {
         pearlrt::Log::error("Error Setting Device Configuration.");
         return;
      }

      uint8_t MaxLUNIndex;

      if (MS_Host_GetMaxLUN(&FlashDisk_MS_Interface, &MaxLUNIndex)) {
         pearlrt::Log::error("Error retrieving max LUN index.");
         USB_Host_SetDeviceConfiguration(corenum, 0);
         return;
      }

      //pearlrt::Log::info("Total LUNs: %d - Using first LUN in device.",
      //                   (MaxLUNIndex + 1));

      if (MS_Host_ResetMSInterface(&FlashDisk_MS_Interface)) {
         pearlrt::Log::error("Error resetting mass storage interface.");
         USB_Host_SetDeviceConfiguration(corenum, 0);
         return;
      }

      // we must read these value and may discard them
      // otherwise the usb system is blocked

      SCSI_Request_Sense_Response_t SenseData;

      if (MS_Host_RequestSense(&FlashDisk_MS_Interface, 0, &SenseData) != 0) {
         pearlrt::Log::error("Error retrieving device sense.");
         USB_Host_SetDeviceConfiguration(corenum, 0);
         return;
      }

      SCSI_Inquiry_Response_t InquiryData;

      if (MS_Host_GetInquiryData(&FlashDisk_MS_Interface, 0, &InquiryData)) {
         pearlrt::Log::error("Error retrieving device inquiry data.");
         USB_Host_SetDeviceConfiguration(corenum, 0);
         return;
      }


      usb_disk_initialize(DEV_USB);

      volume.setVolumeStatus(pearlrt::FatFsVolume::WasInserted);
      pearlrt::Log::info("USB Disk inserted");

   }
};


namespace pearlrt {

   static void Lpc17xxUsbDiskPoll() {
      MS_Host_USBTask(&FlashDisk_MS_Interface);
      USB_USBTask(FlashDisk_MS_Interface.Config.PortNumber, USB_MODE_Host);
   }


   Lpc17xxUsbDisk::Lpc17xxUsbDisk() {
      int ret;
      static struct DiskIoFunctions diof = {
         usb_disk_status, usb_disk_initialize,
         usb_disk_read, usb_disk_write,
         usb_disk_ioctl
      };

      /* register usb disk as the only usb device in the system */
      ret = registerUsbDevice(Lpc17xxUsbDiskPoll,
                              Lpc17xxUsbDiskCompleteEnumeration,
                              disconnectDevice);

      if (ret) {
         Log::error("Lpc17xxUsbDisk: failed to register device");
         throw theInternalDationSignal;
      }

      /* register the usb disk as low level driver for the fat fs */
      ret = disk_registerDrive(DEV_USB, &diof);

      if (ret) {
         Log::error("Lpc17xxUsbDisk: failed to register volume");
         throw theInternalDationSignal;
      }

      FatFs::registerVolume(DEV_USB, &volume);

   }



}

