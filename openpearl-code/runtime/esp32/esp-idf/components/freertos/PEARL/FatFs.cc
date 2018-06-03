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
\brief Implementation of the wrapper of named files on Chans fat file system

*/

#include "ff.h"
#include "ff_errors.h"
#include "FatFs.h"
#include "Character.h"
#include "RefChar.h"
#include "Dation.h"
#include "Log.h"
#include "Signals.h"
#include "Mutex.h"
#include "FreeRTOS.h"
#include "semphr.h"
//#include <time.h>
#include <sys/time.h>

extern "C" {
   /**
   get the system clock.

   This function is usually avaiable in <time.h>, but the cross compiler
   does not contain this element in the standatd include file.

   \param c identier of the clock base. We need CLOCK_REALTIME
   \param t struct receiving the system clock value
   \returns 0 if success\n -1 if fail 
   */
   extern int clock_gettime(clockid_t c, struct timespec* t);

   /**
     get the current date/time for the FAT modification date
     @return      the current time in FAT format
    */
   DWORD get_fattime() {
      struct timespec now;
      struct tm ts;
      // struct timeval tv;
      clock_gettime(CLOCK_REALTIME, &now);

      ts = *localtime(&now.tv_sec);

      /* Pack date and time into a DWORD variable */
      return ((DWORD)(ts.tm_year + 1900 - 1980) << 25)
             // struct tm has month from 0..11
             | ((DWORD)(ts.tm_mon + 1) << 21)
             | ((DWORD) ts.tm_mday << 16)
             | ((DWORD) ts.tm_hour << 11)
             | ((DWORD) ts.tm_min << 5)
             // modification time has a resolution of 2 sec
             | ((DWORD) ts.tm_sec >> 1);
   }

};

namespace pearlrt {
   FatFs::FatFsFile * FatFs::FatFsFile::firstUsedFatFsFile[] =
   {NULL};

   // pointer to the volume working storage
   static FatFsVolume *volumes[_VOLUMES] = {0};



   FatFs::FatFs(const char* dev, const int nbrOfFiles) :
      SystemDationNB() {
      /* ctor is called before multitasking starts --> no mutex required */
      devicePath = (char*)dev;
      mutex.name("FatFs");
      capacity = nbrOfFiles;

      if (*dev < '0' ||  *dev >= '0' + _VOLUMES) {
         Log::error("FatFs: %s invalid volume number", dev);
         throw theInternalDationSignal;
      }

      /* concrete volumes are configured via configuration items
         in the system part. The IMC makes shure that these
         elements are instanciated before dation objects
         The array of volumes is set configured. If the desired
         pointer is still 0, the corresponding device is missing.
      */
      vol = volumes[(*dev) - '0'];

      if (! vol) {
         Log::error("FatFs: %s : no device configured", dev);
         throw theDationParamSignal;
      }

      // check for trailing /
      if (devicePath[strlen(devicePath) - 1] != '/') {
         Log::error("FatFs: %s needs trailing /", dev);
         throw theDationParamSignal;
      }


      // check folder permissions
      cap = IDF | DIRECT | FORWARD;

      cap |= OUT | IN | INOUT;

      if (cap & OUT) {
         cap |= (NEW | ANY | PRM | CAN);
      }

      if (cap & IN) {
         cap |= (OLD | ANY);
      }

      // initialize data elements
      usedCapacity = 0;

      if (capacity < 1) {
         Log::error("FatFs: need at least 1 channels (demanded was %d)",
                    capacity);
         throw theDationParamSignal;
      }

      object = new FatFs::FatFsFile*[capacity];

      if (object == NULL) {
         Log::error("FatFs: could not allocate %d channels", capacity);
         throw theInternalDationSignal;
      }

      for (int i = 0; i < capacity; i++) {
         object[i] = new FatFsFile(this);

         if (object[i] == NULL) {
            Log::error("FatFs: could not allocate channel %d", i);
            throw theInternalDationSignal;
         }
      }
   }

   FatFs::~FatFs() {
      delete[] object;
   }

   int FatFs::capabilities() {
      return cap;
   }


   FatFs::FatFsFile * FatFs::dationOpen(const char * idfValue,
                                        int openParams) {
      char fileName[40]; // automatic filename
      const char * fn = idfValue;
      mutex.lock();

      if (usedCapacity >=  capacity) {
         Log::error("FatFs: more than %d files open.", capacity);
         mutex.unlock();
         throw theOpenFailedSignal;
      }

      // find free handle
      int f = -1;

      for (int i = 0; i < capacity && f == -1; i++) {
         if (! object[i]->inUse) {
            f = i;
         }
      }


      // check parameters
      if ((openParams & ANY) == 0 &&
            ((openParams & IDF) != IDF || idfValue == 0)) {
         Log::error("FatFs: open need filename");
         mutex.unlock();
         throw theDationParamSignal;
      }

      // setup objects data
      FatFs::FatFsFile * o = object[f];

      if (openParams & ANY && idfValue == 0) {
         // create temp file name
         struct timeval tv;
         gettimeofday(&tv, NULL);
         sprintf(fileName, "AutoFile%ld.%06ld", (long)time(NULL), tv.tv_usec);
         fn = fileName;
      }

      o->dationOpen(fn, openParams);
      o->inUse = true;
      usedCapacity ++;
      mutex.unlock();
      return o;
   }

   void FatFs::dationClose(int closeParams) {
   }

   void FatFs::dationRead(void * destination, size_t size) {
   }
   void FatFs::dationWrite(void * destination, size_t size) {
   }
   void FatFs::dationUnGetChar(const char x) {
   }
   void FatFs::FatFs::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }
   char * FatFs::getDevicePath() {
      return devicePath;
   }

   void FatFs::registerVolume(int nbr, FatFsVolume* v) {
      if (nbr < 0 || nbr >= _VOLUMES) {
         Log::error("illegal volume number (%d)", nbr);
      } else {
         volumes[nbr] = v;
      }
   }

   /* ============================================================= */
   /* FatFsFile - stuff                                             */
   /* ============================================================= */
   FatFs::FatFsFile::FatFsFile(FatFs* disc) {
      inUse = false;
      myFatFs = disc;
   }

   int FatFs::FatFsFile::capabilities() {
      return 0;  // will never called; the parents capabilities are requested
   }


   char* FatFs::FatFsFile::getFileName() {
      return completeFileName.get();
   }

   FatFs::FatFsFile* FatFs::FatFsFile::dationOpen(const char * fn,
         int openParams) {

      char * completeFn;
      int volumeNbr;

      // setup completeFileName
      rcFn.setWork(completeFileName);
      rcFn.clear();
      rcFn.add(myFatFs->getDevicePath());
      rcFn.add(fn);
      completeFn = rcFn.getCstring();  // force null-byte at the end

      volumeNbr = (*completeFn) - '0';

      // check if a file with the same name is already opened
      // this would crash the FAT-fs module as described in
      // the application note
      for (FatFs::FatFsFile* curr = firstUsedFatFsFile[volumeNbr];
            curr != NULL; curr = curr->nextUsedFatFsFile) {
         if (strcmp(completeFn, curr->getFileName()) == 0) {
            Log::error("FatFs: %s already opened", completeFn);
            myFatFs->mutex.unlock();
            throw theOpenFailedSignal;
         }
      }

      // setup open mode
      //            IN      OUT      INOUT  precondition
      //  OLD       r        w+       w+    file must exist
      //  NEW       !!       w        r+    file must not exist
      //  ANY       like OLD, if file exists
      //            like NEW, if file does not exist

      // easy case: NEW+IN is ridiculous
      if ((openParams & (NEW | IN)) == (NEW | IN)) {
         Log::error("FatFs: open NEW+IN is ridiculous");
         myFatFs->mutex.unlock();
         throw theDationParamSignal;
      }

      BYTE mode = -1;
      FIL fpTest;
      FRESULT result;

      myFatFs->vol->lock();
      myFatFs->vol->treatVolumeStatus();
      result = f_open(&fpTest, completeFn, FA_READ | FA_OPEN_EXISTING);
      myFatFs->vol->unlock();

      if (result != FR_OK) {
         if (openParams & OLD || openParams & IN) {
            Log::error("FatFs: not found: %s (%s)", fn, f_strerror(result));
            myFatFs->mutex.unlock();
            throw theOpenFailedSignal;
         }

         if (openParams & (NEW | ANY) && openParams & INOUT) {
            mode = FA_CREATE_NEW | FA_WRITE | FA_READ;
         }

         if (openParams & (NEW | ANY) && openParams & OUT) {
            mode = FA_CREATE_NEW | FA_WRITE;
         }
      } else {
         // file exists

         myFatFs->vol->lock();
         f_close(&fpTest);
         myFatFs->vol->unlock();

         if (openParams & NEW) {
            Log::error("FatFs: exists: %s", fn);
            myFatFs->mutex.unlock();
            throw theOpenFailedSignal;
         }

         if (openParams & (OLD | ANY)) {
            if (openParams & IN) {
               mode = FA_OPEN_EXISTING | FA_READ;
            }

            if (openParams & (OUT | INOUT)) {
               mode = FA_OPEN_EXISTING | FA_READ | FA_WRITE;
            }
         }
      }

      if (mode == -1) {
         Log::error("FatFs: confusing open parameters %x", openParams);
         myFatFs->mutex.unlock();
         throw theOpenFailedSignal;
      }

      myFatFs->vol->lock();
      result = f_open((FIL*)&fil, completeFn, mode);
      myFatFs->vol->unlock();

      if (result != FR_OK) {
         Log::error("FatFs: error opening file %s (%s)",
                     fn, f_strerror(result));
         myFatFs->mutex.unlock();
         throw theOpenFailedSignal;
      }

      nextUsedFatFsFile = firstUsedFatFsFile[volumeNbr];
      firstUsedFatFsFile[volumeNbr] = this;

      return this;
   }

   void FatFs::FatFsFile::dationClose(int closeParams) {
      FRESULT ret;
      int volumeNbr;

      volumeNbr = (*getFileName()) - '0';

      myFatFs->mutex.lock();
      inUse = false;
      myFatFs->usedCapacity --;

      // remove entry from list of open files
      FatFs::FatFsFile* curr = firstUsedFatFsFile[volumeNbr];

      if (curr == this) {
         firstUsedFatFsFile[volumeNbr] = this->nextUsedFatFsFile;
      } else {
         for (
            /* preset already done */;
            curr != NULL; curr = curr->nextUsedFatFsFile) {
            if (curr->nextUsedFatFsFile == this)  {
               curr->nextUsedFatFsFile = this->nextUsedFatFsFile;
               break;  // leave loop
            }
         }
      }

      myFatFs->vol->lock();
      myFatFs->vol->treatVolumeStatus();
      ret = f_close((FIL*)&fil);
      myFatFs->vol->unlock();

      myFatFs->mutex.unlock();

      if (ret != FR_OK) {
         Log::error("FatFs: error at close (%s)", f_strerror(ret));
         throw theCloseFailedSignal;
      }

      if (closeParams & CAN) {
         myFatFs->vol->lock();
         myFatFs->vol->treatVolumeStatus();
         ret = f_unlink(rcFn.getCstring());
         myFatFs->vol->unlock();

         if (ret) {
            Log::error("FatFs: file %s error at close/remove (%s)",
                       rcFn.getCstring(), f_strerror(ret));
            throw theCloseFailedSignal;
         }
      }
   }


   void FatFs::FatFsFile::dationRead(void * destination, size_t size) {
      FRESULT ret;
      UINT got;

      myFatFs->vol->lock();
      myFatFs->vol->treatVolumeStatus();
      ret = f_read((FIL*)&fil, destination, size, &got);
      myFatFs->vol->unlock();

      if (ret != FR_OK) {
         Log::error("FatFs: error at read (%s)", f_strerror(ret));
         throw theReadingFailedSignal;
      }

      if (got != size) {
         Log::error("reached EOF ");
         throw theDationEOFSignal;
      }
   }


   void FatFs::FatFsFile::dationWrite(void * source, size_t size) {
      FRESULT ret;
      UINT written;

      myFatFs->vol->lock();
      myFatFs->vol->treatVolumeStatus();
      ret = f_write((FIL*)&fil, source, size, &written);
      myFatFs->vol->unlock();

      if (ret != FR_OK || written != size) {
         Log::error("FatFs: error at write (%s, bytes=%d)", f_strerror(ret),
                    (int) written);
         throw theWritingFailedSignal;
      }

   }

   void FatFs::FatFsFile::dationSeek(const Fixed<31>& p,
                                     const int dationParam) {
      FRESULT ret;

      if (dationParam & Dation::DIRECT) {
         myFatFs->vol->lock();
         myFatFs->vol->treatVolumeStatus();
         ret = f_lseek((FIL*)&fil, p.x);
         myFatFs->vol->unlock();

         if (ret != 0) {
            Log::error("FatFs: positioning failed (%s)", f_strerror(ret));
            throw thePositioningFailedSignal;
         }
      } else if (dationParam & Dation::FORWARD) {
         /* do nothing: seek is done by empty reads
            or write 0's
         */
      } else {
         Log::error("FatFs: FORBACK not supported");
      }
   }

   void FatFs::FatFsFile::dationUnGetChar(const char x) {
      //ungetc(x, fp);
#warning no unget in FatFsFile?
   }

   void FatFs::FatFsFile::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }



}
