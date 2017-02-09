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

#include "FatFs.h"
#include "Character.h"
#include "RefChar.h"
#include "Dation.h"
#include "Log.h"
#include "Signals.h"
//#include <time.h>
#include <sys/time.h>

extern "C" {
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
   FatFs::FatFsFile::FatFsFile(FatFs* disc) {
      inUse = false;
      myFatFs = disc;
   }

   FatFs::~FatFs() {
      delete[] object;
   }

   FatFs::FatFs(const char* dev, const int nbrOfFiles) :
      SystemDationNB() {
      /* ctor is called before multitasking starts --> no mutex required */
      FILINFO fileinfo;
      devicePath = (char*)dev;
      mutex.name("FatFs");
      capacity = nbrOfFiles;

#if 0

      // check if path points to normal file,directory or devicefile
      if (f_stat(devicePath, &fileinfo) != FR_OK) {
         //can't get stat -> throw signal
         Log::error("FatFs: could not locate %s", dev);
         throw theIllegalPathSignal;
      }

      if (!(fileinfo.fattrib  & AM_DIR)) {
         Log::error("FatFs: %s is not a directory", dev);
         throw theIllegalPathSignal;
      }

#endif

      // check for trailing /
      if (devicePath[strlen(devicePath) - 1] != '/') {
         Log::error("FatFs: %s needs trailing /", dev);
         throw theIllegalPathSignal;
      }

      // initialize data elements
      usedCapacity = 0;
      // check folder permissions
      cap = IDF | DIRECT | FORWARD;

      if (fileinfo.fattrib & AM_RDO) {
         cap |= IN;
      } else {
         cap |= OUT | IN;
      }

      if (cap & OUT) {
         cap |= (NEW | ANY | PRM | CAN);
      }

      if (cap & IN) {
         cap |= (OLD | ANY);
      }

      Character<80> capString;
      RefCharacter rc(capString);
      rc.clear();

      if (cap & IDF) {
         rc.add("IDF ");
      }

      if (cap & IN) {
         rc.add("IN ");
      }

      if (cap & OUT) {
         rc.add("OUT ");
      }

      if (cap & INOUT) {
         rc.add("INOUT ");
      }

      if (cap & OLD) {
         rc.add("OLD ");
      }

      if (cap & NEW) {
         rc.add("NEW ");
      }

      if (cap & ANY) {
         rc.add("ANY ");
      }

      if (cap & CAN) {
         rc.add("CAN ");
      }

      if (cap & PRM) {
         rc.add("PRM ");
      }

      if (cap & DIRECT) {
         rc.add("DIRECT ");
      }

      if (cap & FORWARD) {
         rc.add("FORWARD ");
      }

      if (cap & FORBACK) {
         rc.add("FORBACK ");
      }

      if (capacity < 1) {
         Log::error("FatFs: need at least 1 channels (demanded was %d)",
                    capacity);
         throw theIllegalParamSignal;
      }

      object = new FatFs::FatFsFile*[capacity];

      if (object == NULL) {
         Log::error("FatFs: could not allocate %d channels", capacity);
         throw theIllegalParamSignal;
      }

      for (int i = 0; i < capacity; i++) {
         object[i] = new FatFsFile(this);

         if (object[i] == NULL) {
            Log::error("FatFs: could not allocate channel %d", i);
            throw theIllegalParamSignal;
         }
      }

      Log::info("FatFs: folder %s provides %s ", devicePath, rc.getCstring());
   }

   int FatFs::capabilities() {
      return cap;
   }

   int FatFs::FatFsFile::capabilities() {
      return 0;  // will never called; the parents capabilities are requested
   }

   FatFs::FatFsFile * FatFs::dationOpen(const char * idfValue,
                                        int openParams) {
      char fileName[40]; // autoamtic filename
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
         throw theIllegalParamSignal;
      }

      // setup objects data
      FatFs::FatFsFile * o = object[f];
      Character<256> completeFilename;
      RefCharacter rc(completeFilename);
      rc.clear();
      rc.add(devicePath);

      if (openParams & ANY && idfValue == 0) {
         // create temp file name
         struct timeval tv;
         gettimeofday(&tv, NULL);
         sprintf(fileName, "AutoFile%ld.%06ld", time(NULL), tv.tv_usec);
         rc.add(fileName);
      } else {
         rc.add(idfValue);
      }

      o->dationOpen(rc.getCstring(), openParams);
      o->inUse = true;
      usedCapacity ++;
      mutex.unlock();
      return o;
   }

   FatFs::FatFsFile* FatFs::FatFsFile::dationOpen(const char * fn,
         int openParams) {
      // setup open mode
      //            IN      OUT      INOUT  precondition
      //  OLD       r        w+       w+    file must exist
      //  NEW       !!       w        r+    file must not exist
      //  ANY       like OLD, if file exists
      //            like NEW, if file does not exist

      rcFn.setWork(completeFileName);
      rcFn.clear();
      rcFn.add(fn);

      // easy case: NEW+IN is ridiculous
      if ((openParams & (NEW | IN)) == (NEW | IN)) {
         Log::error("FatFs: open NEW+IN is ridiculous");
         myFatFs->mutex.unlock();
         throw theIllegalParamSignal;
      }

      BYTE mode = -1;
      FIL fpTest;
      FRESULT result;

      result = f_open(&fpTest, fn, FA_READ | FA_OPEN_EXISTING);

      if (result != FR_OK) {
         if (openParams & OLD || openParams & IN) {
            Log::error("FatFs: not found: %s", fn);
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
         f_close(&fpTest);

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

      result = f_open(&fil, fn, mode);

      if (result != FR_OK) {
         Log::error("FatFs: error opening file %s (%d)", fn, result);
         myFatFs->mutex.unlock();
         throw theOpenFailedSignal;
      }

      return this;
   }

   void FatFs::dationClose(int closeParams) {
   }

   void FatFs::FatFsFile::dationClose(int closeParams) {
      int ret;
      myFatFs->mutex.lock();
      inUse = false;
      myFatFs->usedCapacity --;
      myFatFs->mutex.unlock();
      ret = f_close(&fil);

      if (ret != FR_OK) {
         Log::error("FatFs: error at close (%d)", ret);
         throw theCloseFailedSignal;
      }

      if (closeParams & CAN) {
         ret = f_unlink(rcFn.getCstring());

         if (ret) {
            Log::error("FatFs: file %s error at close/remove (%d)",
                       rcFn.getCstring(), ret);
            throw theCloseFailedSignal;
         }
      }
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

   void FatFs::FatFsFile::dationRead(void * destination, size_t size) {
      FRESULT ret;
      UINT got;
      ret = f_read(&fil, destination, size, &got);

      if (ret != FR_OK) {
         Log::error("FatFs: error at read (%d)", ret);
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
      ret = f_write(&fil, source, size, &written);

      if (ret != FR_OK || written != size) {
         Log::error("FatFs: error at write (error=%d, bytes=%d)", (int)ret,
                    (int) written);
         throw theWritingFailedSignal;
      }

   }

   void FatFs::FatFsFile::dationSeek(const Fixed<31>& p,
                                     const int dationParam) {
      FRESULT ret;

      if (dationParam & Dation::DIRECT) {
         ret = f_lseek(&fil, p.x);

         if (ret != 0) {
            Log::error("FatFs: positioning failed (%d)", ret);
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
   }

   void FatFs::FatFsFile::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }

}
