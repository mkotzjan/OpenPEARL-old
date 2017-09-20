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

\brief representation of a volume of the FAT file system library
*/


#ifndef FATFSVOLUME_INCLUDED
#define FATFSVOLUME_INCLUDED
#include "Mutex.h"

#include "FakeTypes.h"

namespace pearlrt {
   /**
   representation of a volume of the FAT file system library

   This class is used internally by the volume device driver modules,
   and the FatFs-class 
   */
   class FatFsVolume {
   private:
      Mutex mutex;   /* the volume lock */
      int status;
      const char * path;
      FakeFATFS  fs;     /* the file system object  */
   public:
      /**
      The Chan's FatFs library is not thread safe.
      There is a mechanism for proper locking, but this is not
      suitable for OpenPEARL, since the mutex would be allocated
      and returned at each mount/unmount operation.

      \param p path of the volume (e.g. "1:/");
      */
      FatFsVolume(const char * p);

      /** lock the volume */
      void lock();

      /** lock the volume */
      void unlock();

      /**
      the status of the volume.
      The status may change suddenly from outside, when a
      removable media is inserted or removed.
      */
      enum Status { IsMounted = 1, WasRemoved = 2, WasInserted = 4};

      /**
      \brief perform required operations
             if the status of the volume has changed.

      The status of the volume may be a comination of IsMounted, WasRemoved
      and WasInserted..
      The removal of a volume is detected by a backgroud process, which 
      calls setVolumeStatus() to WasRemoved. This method performs the 
      u_mount of the volume in the FatFs-library.

      If the volume is detected to became inserted, the status is set by the
      background task to WasInserted and the volume becomes mounted again.
      */
      void treatVolumeStatus();

      /**
      \brief mark a status change

      The new detected status WasRemoved or WasInserted is just registered
      in the status variable. 

      \param newStatusBit  the new detected state of the volume
      */
      void setVolumeStatus(int newStatusBit);
  
      /**
      clear all bits in the volume status variable
      */
      void clearVolumeStatus();
   };
}
#endif

