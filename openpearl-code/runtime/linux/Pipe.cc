/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Holger Koelle
 Copyright (c) 2014 Rainer Mueller
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

#include "Pipe.h"
#include "Character.h"
#include "RefChar.h"
#include "Dation.h"
#include "Log.h"
#include "Signals.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace pearlrt {
   char* Pipe::devicePath = NULL;

   Pipe::PipeFile::PipeFile(Pipe* parent) {
      inUse = false;
      myPipe = parent;
   }

   Pipe::~Pipe() {
      delete[] object;

      if (defaultReader) {
         fclose(defaultReader);
         defaultReader = NULL;
      }

      if (removeFile)  {
         unlink(devicePath);
      }
   }

   Pipe::Pipe(const char* dev, const int nbrOfFiles, const char * params) :
      SystemDationNB() {
      /* ctor is called before multitasking starts --> no mutex required */
      struct stat attribut;
      devicePath = (char*)dev;
      mutex.name("Pipe");
      capacity = nbrOfFiles;
      int mode = 0;
      int open1 = 0;
      int gotAttribs;

      // parse params
      if (params) {
         if (strstr(params, "OLD")) {
            mode |= OLD;
         }

         if (strstr(params, "NEW")) {
            mode |= NEW;
         }

         if (strstr(params, "ANY")) {
            mode |= ANY;
         }

         if (strstr(params, "CAN")) {
            mode |= CAN;
         }

         if (strstr(params, "PRM")) {
            mode |= PRM;
         }

         if (strstr(params, "OPEN1")) {
            open1 = true;
         }
      } else {
         mode = (ANY | PRM);
      }

      if (!!(mode & OLD) + !!(mode & NEW) + !!(mode & ANY) ==  0) {
         mode |= ANY; // set default
      }

      if (!!(mode & OLD) + !!(mode & NEW) + !!(mode & ANY) > 1) {
         Log::error("Pipe: %s: one of OLD, NEW and ANY allowed", dev);
         throw theIllegalParamSignal;
      }

      if (!!(mode & PRM) + !!(mode & CAN) > 1) {
         Log::error("Pipe: %s: only one of PRM and CAN allowed", dev);
         throw theIllegalParamSignal;
      }

      removeFile = !!(mode & CAN);
      // initialize data elements
      usedCapacity = 0;
      cap = FORWARD;
      cap |= (OLD | ANY);
      cap |= PRM;
      // check if path points to normal file,directory or devicefile, ..
      gotAttribs = stat(dev, &attribut);

      if (gotAttribs == -1) {
         if (mode & OLD) {
            //can't get stat -> throw signal
            Log::error("Pipe: could not locate %s", dev);
            throw theIllegalPathSignal;
         }

         if (mode & (NEW | ANY)) {
            // create new FIFO
            if (mkfifo(devicePath, 0777) == -1) {
               Log::error("Pipe: could not create fifo %s", devicePath);
               Log::error("   %s", strerror(errno));
               throw theIllegalPathSignal;
            }

            cap |= (IN | OUT);
         }
      } else {
         if (mode & NEW) {
            Log::error("Pipe: %s exists in system", dev);
            throw theIllegalParamSignal;
         }

         if (!(attribut.st_mode & S_IFIFO)) {
            Log::error("Pipe: %s is not a FIFO", dev);
            throw theIllegalPathSignal;
         }

         if (geteuid() == attribut.st_uid) {
            //  am owner of the folder
            if (attribut.st_mode & S_IRUSR) {
               cap |= IN;
            }

            if (attribut.st_mode & S_IWUSR) {
               cap |= OUT;
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
         }

         // permissions of OTHERs apply also
         if (attribut.st_mode & S_IROTH) {
            cap |= IN;
         }

         if (attribut.st_mode & S_IWOTH) {
            cap |= OUT;
         }
      }

      // set cancel capability if pipe file descriptor is writeable
      if (cap & OUT) {
         cap |= CAN;
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
         Log::error("Pipe: need at least 1 channels (demanded was %d)",
                    capacity);
         throw theIllegalParamSignal;
      }

      object = new Pipe::PipeFile*[capacity];

      if (object == NULL) {
         Log::error("Pipe: could not allocate %d channels", capacity);
         throw theIllegalParamSignal;
      }

      for (int i = 0; i < capacity; i++) {
         object[i] = new PipeFile(this);

         if (object[i] == NULL) {
            Log::error("Pipe: could not allocate channel %d", i);
            throw theIllegalParamSignal;
         }
      }

      Log::info("Pipe: %s provides %s ", devicePath, rc.getCstring());
      defaultReader = NULL;

      if (open1) {
         defaultReader = fopen(devicePath, "r+");

         if (defaultReader == 0) {
            Log::error("Pipe: %s: failed to open default reader", devicePath);
            throw theOpenFailedSignal;
         } else {
            Log::info("Pipe: %s: default reader opened", devicePath);
         }
      }
   }

   int Pipe::capabilities() {
      return cap;
   }

   int Pipe::PipeFile::capabilities() {
      return 0;
   }

   Pipe::PipeFile *  Pipe::dationOpen(const char * idfValue, int openParams) {
      mutex.lock();

      if (usedCapacity >=  capacity) {
         Log::error("Pipe: more than %d files open.", capacity);
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
      if (openParams & IDF) {
         Log::error("Pipe: no IDF allowed");
         mutex.unlock();
         throw theIllegalParamSignal;
      }

      // setup objects data
      Pipe::PipeFile * o = object[f];
      o->dationOpen(NULL, openParams);
      o->inUse = true;
      usedCapacity ++;
      mutex.unlock();
      return o;
   }

   Pipe::PipeFile* Pipe::PipeFile::dationOpen(const char * fn,
         int openParams) {
      // setup open mode
      //            IN      OUT      INOUT  precondition
      //  OLD       r        w+       w+    file must exist
      //  NEW       not possible for pipes. The device is created in the system
      //            device ctor
      //  ANY       like OLD, if file exists
      //            like NEW, if file does not exist (see note above!)

      // easy case: NEW is ridiculous for pipes
      if (openParams & NEW) {
         Log::error("Pipe: %s: open NEW is ridiculous", devicePath);
         myPipe->mutex.unlock();
         throw theIllegalParamSignal;
      }

      const char * mode = 0;  // illegal combination as preset

      // the device file exists!
      // else we would not reach this point
      if (openParams & (OLD | ANY)) {
         if (openParams & IN) {
            mode = "r";
         }

         if (openParams & OUT) {
            mode = "w";
         }
      }

      if (mode == NULL) {
         Log::error("Pipe: confusing open parameters %x", openParams);
         myPipe->mutex.unlock();
         throw theIllegalParamSignal;
      }

      fp = fopen(devicePath, mode);

      if (fp == 0) {
         Log::error("Pipe: error opening pipe %s (%s)",
                    devicePath, strerror(ferror(fp)));
         myPipe->mutex.unlock();
         throw theOpenFailedSignal;
      }

      return this;
   }

   void Pipe::dationClose(int closeParams) {
   }

   void Pipe::PipeFile::dationClose(int closeParams) {
      int ret;
      myPipe->mutex.lock();
      inUse = false;
      myPipe->usedCapacity --;
      myPipe->mutex.unlock();
      ret = fclose(fp);

      if (ret != 0) {
         Log::error("Pipe: error at close (%s)", strerror(ferror(fp)));
         throw theCloseFailedSignal;
      }

      if (closeParams & CAN) {
         Log::error("Pipe: file %s CAN not allowed", devicePath);
         throw theIllegalParamSignal;
      }
   }

   void Pipe::dationRead(void * destination, size_t size) {
   }
   void Pipe::dationWrite(void * destination, size_t size) {
   }
   void Pipe::dationUnGetChar(const char x) {
   }

   void Pipe::Pipe::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }

   void Pipe::PipeFile::dationRead(void * destination, size_t size) {
      int ret;
//   allow read after write does not apply at pipes; they are unidirectional
//      clearerr(fp);
//      fseek(fp, 0, SEEK_CUR);   // allow read after write
      errno = 0;
      ret = fread(destination, size, 1, fp);

      if (ret < 1) {
         if (feof(fp)) {
            Log::error("Pipe: error across EOF");
            throw theDationEOFSignal;
         }

         Log::error("Pipe: error at read (%s)", strerror(errno));
         throw theReadingFailedSignal;
      }
   }


   void Pipe::PipeFile::dationWrite(void * source, size_t size) {
      int ret;
      fseek(fp, 0, SEEK_CUR);      // allow write after read
      errno = 0;
      ret = fwrite(source, size, 1, fp);

      if (ret < 1) {
         Log::error("Pipe: error at write (%s)", strerror(errno));
         throw theWritingFailedSignal;
      }

      ret = fflush(fp);

      if (ret != 0) {
         Log::error("Pipe: error at fflush (%s)", strerror(errno));
      }
   }

   void Pipe::PipeFile::dationUnGetChar(const char x) {
      ungetc(x, fp);
   }

   void Pipe::PipeFile::translateNewLine(bool doNewLineTranslation) {
      // do nothing
   }
}
