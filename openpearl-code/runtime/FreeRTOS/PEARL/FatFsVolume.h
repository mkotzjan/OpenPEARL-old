#ifndef FATFSVOLUME_INCLUDED
#define FATFSVOLUME_INCLUDED
#include "Mutex.h"
#include "ff.h"

namespace pearlrt {
   /**
   representaion of a volume of the FAT file system library
   */
   class FatFsVolume {
   private:
      Mutex mutex;   /* the volume lock */
      int status;
      const char * path;
      FATFS  fs;     /* the file system object  */
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

      void treatVolumeStatus();
      void setVolumeStatus(int newStatusBit);
      void clearVolumeStatus();
   };
}
#endif

