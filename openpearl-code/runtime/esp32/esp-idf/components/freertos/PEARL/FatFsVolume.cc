#include "ff.h"
#include "ff_errors.h"
#include "FatFsVolume.h"
#include "Mutex.h"
#include "Log.h"

namespace pearlrt {

   FatFsVolume::FatFsVolume(const char * p) {
      status = 0;
      path = p;
   }

   void FatFsVolume::lock() {
      mutex.lock();
   }

   void FatFsVolume::unlock() {
      mutex.unlock();
   }

   void FatFsVolume::setVolumeStatus(int s) {
      status |= s;
   }

   void FatFsVolume::clearVolumeStatus() {
      status = 0;
   }

   void FatFsVolume::treatVolumeStatus() {
      // this function is called while the volume is locked!
      FRESULT result;
      FATFS * _fs = (FATFS*)&fs;

      if ((status & (IsMounted | WasRemoved)) == (IsMounted | WasRemoved)) {
         // device removed while mounted --> force unmount
         f_mount(_fs, 0, 1);  // unmount
         Log::info("unmounted");
         status &= ~(IsMounted | WasRemoved);
      }

      if (status & WasInserted) {
         result = f_mount(_fs, path, 1);

         if (result != FR_OK) {
            Log::error("could not mount disk >%s< (%s)",
                       path, f_strerror(result));
         } else {

            status = IsMounted;
            char label[24];

            result = f_getlabel(path, label, NULL);

            if (result != FR_OK) {
               pearlrt::Log::error("could not read volume label");
            } else {
               pearlrt::Log::info("volume >%s<", label);
            }

            pearlrt::Log::info("disk >%s< is ready.", path);
         }
      }

   }

}
