/*
 [A "BSD license"]
 Copyright (c) 2013-2014 Holger Koelle
 Copyright (c) 2014-17 Rainer Mueller
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
\brief Implementation of generic non-Basic Linux Systemdation

*/

#include "Disc.h"
#include "Character.h"
#include "RefChar.h"
#include "Dation.h"
#include "Log.h"
#include "Signals.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

namespace pearlrt {
   Disc::DiscFile::DiscFile(Disc* disc) {
      inUse = false;
      myDisc = disc;
   }

   Disc::~Disc() {
      delete[] object;
   }

   Disc::Disc(const char* dev, const int nbrOfFiles) :
      SystemDationNB() {
      /* ctor is called before multitasking starts --> no mutex required */
      struct stat attribut;
      devicePath = (char*)dev;
      mutex.name("Disc");
      capacity = nbrOfFiles;

      // check if path points to normal file,directory or devicefile
      if (stat(devicePath, &attribut) == -1) {
         //can't get stat -> throw signal
         Log::error("Disc: could not locate %s", dev);
         throw theDationParamSignal;
      }

      if (!(attribut.st_mode & S_IFDIR)) {
         Log::error("Disc: %s is not a directory", dev);
         throw theDationParamSignal;
      }

      // check for trailing /
      if (devicePath[strlen(devicePath) - 1] != '/') {
         Log::error("Disc: %s needs trailing /", dev);
         throw theDationParamSignal;
      }

      // initialize data elements
      usedCapacity = 0;
      // check folder permissions
      cap = IDF | DIRECT | FORWARD;

      if (geteuid() == attribut.st_uid) {
         //  am owner of the folder
         if (attribut.st_mode & S_IRUSR) {
            cap |= IN;
         }

         if (attribut.st_mode & S_IWUSR) {
            cap |= OUT;
         }

         if (attribut.st_mode & (S_IRUSR | S_IWUSR)) {
            cap |= INOUT;
         }
      }

      if (getegid() == attribut.st_gid) {
         //  am in the same group as the folder
         if (attribut.st_mode & S_IRGRP) {
            cap |= IN;
         }

         if (attribut.st_mode & S_IWGRP) {
            cap |= OUT;
         }

         if (attribut.st_mode & (S_IRGRP | S_IWGRP)) {
            cap |= INOUT;
         }
      }

      // permissions of OTHERs apply also
      if (attribut.st_mode & S_IROTH) {
         cap |= IN;
      }

      if (attribut.st_mode & S_IWOTH) {
         cap |= OUT;
      }

      if (attribut.st_mode & (S_IROTH | S_IWOTH)) {
         cap |= INOUT;
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
         Log::error("Disc: need at least 1 channels (demanded was %d)",
                    capacity);
         throw theDationParamSignal;
      }

      object = new Disc::DiscFile*[capacity];

      if (object == NULL) {
         Log::error("Disc: could not allocate %d channels", capacity);
         throw theDationParamSignal;
      }

      for (int i = 0; i < capacity; i++) {
         object[i] = new DiscFile(this);

         if (object[i] == NULL) {
            Log::error("Disc: could not allocate channel %d", i);
            throw theDationParamSignal;
         }
      }

      Log::info("Disc: folder %s provides %s ", devicePath, rc.getCstring());
   }

   int Disc::capabilities() {
      return cap;
   }

   int Disc::DiscFile::capabilities() {
      return 0;  // will never called; the parents capabilities are requested
   }

   Disc::DiscFile *  Disc::dationOpen(const char * idfValue, int openParams) {
      char fileName[40]; // autoamtic filename
      mutex.lock();

      if (usedCapacity >=  capacity) {
         Log::error("Disc: more than %d files open.", capacity);
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
         Log::error("Disc: open need filename");
         mutex.unlock();
         throw theDationParamSignal;
      }

      // setup objects data
      Disc::DiscFile * o = object[f];
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

   Disc::DiscFile* Disc::DiscFile::dationOpen(const char * fn,
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
         Log::error("Disc: open NEW+IN is ridiculous");
         myDisc->mutex.unlock();
         throw theDationParamSignal;
      }

      const char * mode = 0;  // illegal combination as preset

      FILE * fpTest = fopen(fn, "r");

      if (fpTest == NULL) {
         if (openParams & OLD || openParams & IN) {
            Log::error("Disc: not found: %s", fn);
            myDisc->mutex.unlock();
            throw theOpenFailedSignal;
         }

         if (openParams & (NEW | ANY) && openParams & INOUT) {
            mode = "w+";
         }

         if (openParams & (NEW | ANY) && openParams & OUT) {
            mode = "w";
         }
      } else {
         // file exists
         fclose(fpTest);

         if (openParams & NEW) {
            Log::error("Disc: exists: %s", fn);
            myDisc->mutex.unlock();
            throw theOpenFailedSignal;
         }

         if (openParams & (OLD | ANY)) {
            if (openParams & IN) {
               mode = "r";
            }

            if (openParams & (OUT | INOUT)) {
               mode = "r+";
            }
         }
      }

      if (mode == NULL) {
         Log::error("Disc: confusing open parameters %x", openParams);
         myDisc->mutex.unlock();
         throw theOpenFailedSignal;
      }

      fp = fopen(fn, mode);

      if (fp == 0) {
         Log::error("Disc: error opening file %s (%s)",
                    fn, strerror(errno));
         myDisc->mutex.unlock();
         throw theOpenFailedSignal;
      }

      return this;
   }

   void Disc::dationClose(int closeParams) {
   }

   void Disc::DiscFile::dationClose(int closeParams) {
      int ret;
      myDisc->mutex.lock();
      inUse = false;
      myDisc->usedCapacity --;
      myDisc->mutex.unlock();
      ret = fclose(fp);

      if (ret != 0) {
         Log::error("Disc: error at close (%s)", strerror(ferror(fp)));
         throw theCloseFailedSignal;
      }

      if (closeParams & CAN) {
         ret = remove(rcFn.getCstring());

         if (ret) {
            Log::error("Disc: file %s error at close/remove (%s)",
                       rcFn.getCstring(), strerror(ferror(fp)));
            throw theCloseFailedSignal;
         }
      }
   }

   void Disc::dationRead(void * destination, size_t size) {
   }
   void Disc::dationWrite(void * destination, size_t size) {
   }
   void Disc::dationUnGetChar(const char x) {
   }

   void Disc::Disc::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }

   void Disc::DiscFile::dationRead(void * destination, size_t size) {
      int ret;
      clearerr(fp);
      fseek(fp, 0, SEEK_CUR);   // allow read after write
      errno = 0;
      ret = fread(destination, size, 1, fp);

      if (ret < 1) {
         if (feof(fp)) {
            Log::error("Disc: error across EOF");
            throw theDationEOFSignal;
         }

         Log::error("Disc: error at read (%s)", strerror(errno));
         throw theReadingFailedSignal;
      }
   }


   void Disc::DiscFile::dationWrite(void * source, size_t size) {
      int ret;
      fseek(fp, 0, SEEK_CUR);      // allow write after read
      errno = 0;
      ret = fwrite(source, size, 1, fp);

      if (ret < 1) {
         Log::error("Disc: error at write (%s)", strerror(errno));
         throw theWritingFailedSignal;
      }
   }

   void Disc::DiscFile::dationSeek(const Fixed<31>& p, const int dationParam) {
      if (dationParam & Dation::DIRECT) {
         if (fseeko(fp, p.x, SEEK_SET) != 0) {
            Log::error("Disc: positioning failed (%s)",
                       strerror(ferror(fp)));
            throw thePositioningFailedSignal;
         }
      } else if (dationParam & Dation::FORWARD) {
         /* do nothing: seek is done by empty reads
            or write 0's
         */
      } else {
         Log::error("Disc: FORBACK not supported");
      }
   }
   void Disc::DiscFile::dationUnGetChar(const char x) {
      ungetc(x, fp);
   }

   Fixed<31> Disc::DiscFile::dationEof() {
       off_t location;

       if (fseeko(fp, 0, SEEK_END) != 0) {
          Log::error("Disc: positioning failed (%s)",
                     strerror(ferror(fp)));
          throw thePositioningFailedSignal;
       }
       location = ftello(fp);
       if (location == -1) {
          Log::error("Disc: read position failed (%s)",
                     strerror(ferror(fp)));
          throw thePositioningFailedSignal;
       }
       Fixed<31> fileLocation(location);
       return fileLocation;
   } 

   void Disc::DiscFile::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }

}
