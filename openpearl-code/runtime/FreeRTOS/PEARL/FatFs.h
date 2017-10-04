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

#ifndef FATFS_INCLUDED
#define FATFS_INCLUDED


/**
\file

\brief FAT filesystem wrapper over Chan's library
*/

#include "FakeTypes.h"

#include "SystemDationNB.h"
#include "Mutex.h"
#include "Character.h"
#include "RefChar.h"
#include "FatFsVolume.h"

namespace pearlrt {

   /**
   \brief generic non-basic systemdation class

   Provide access to files on discs. 
   Differnt discs are distinguished by a volume number, which
   looks like "0:" at the beginning of a file path string.

   The implementation uses the class FatFs as container for FatFsFile
   objects, which perform the i/o-operations.

   One parameter of the Ctor defines the number of open files
   on one FatFs-objects. The FatFsFile objects are allocated in a pool,
   which is created at instantiation of the Disc-object.
   All files are created in an already existing folder on the volume.
   It is NOT checked, where the file name in the open statement contains
   other folder specifications.

   At this point of execution the 'new' operation is allowed.

   */

   class FatFs: public SystemDationNB {
   private:
      static FatFsVolume * volume[FAKE_VOLUMES];
      FatFsVolume        * vol;

   public:
      /**
      A FatFsFile represents a file on a FatFS. This class is responsible
      for all operations with the  operating system.
      */
      class FatFsFile : public SystemDationNB {
      private:
         FakeFIL fil;
         RefCharacter   rcFn;
         Character<64> completeFileName;
         FatFs * 	myFatFs;
         FatFsFile 	* nextUsedFatFsFile;
         static FatFsFile * firstUsedFatFsFile[FAKE_VOLUMES];
         static Mutex volumeLock[FAKE_VOLUMES];


      public:
         /**
            shows whether this pool element is in use
         */
         bool inUse;

         /**
            ctor of working pool element

            \param disc pointer to parent Disc-object
         */
         FatFsFile(FatFs* disc);

         /**
            return capabilities of the file objects

            This is only needed for the folder. Returning 0 is ok.

           \returns 0
         */
         int capabilities();

         /**
          open method

         \param fileName   the file name as C string
         \param openParams  the PEARL parameters for file open
         \returns pointer to working object to do the subsequent
                  dation operations.

         \throws OpenFailedSignal in case of errors
         */
         FatFsFile* dationOpen(const char * fileName, int openParams);

         /**
          close method.

          Close the file and return the working channel back to the pool.

         \param closeParams actions to be done at close (PRM, CAN)
         \throws CloseFailedSignal in case of errors

         */
         void dationClose(int closeParams);

         /**
         read method

         read the required number of bytes

         \param destination target area for the read bytes
         \param size number of bytes to read

         \throws ReadingFailedSignal in case of read errors

         */
         void dationRead(void * destination, size_t size);

         /**
         write method

         write the required number of bytes

         \param destination source area for the bytes to be written
         \param size number of bytes to write

         \throws WritingFailedSignal in case of write errors

         */
         void dationWrite(void * destination, size_t size);

         /**
         set file position

         \param p the desired position (in bytes)
         \param dationParam specifies DIRECT/FORBACK
         \throws PositioningFailedSignal
         */
         void dationSeek(const Fixed<31>&p, const int dationParam);

         /**
          send one character back to the input.

         \param c the character which shall returned to the input

         \throws * depends an the implementation of the SystemDationNB device
         */
         void dationUnGetChar(const char c);

         /**
	 translate newline

         this is empty since linux uses \n for newline

         \param doNewLineTranslation enbale/disable the translation
             (has no effect)
         */
         void translateNewLine(bool doNewLineTranslation);

         /**
           retrieve the file name of the opened file
           \return C-string of the file name
         */
         char * getFileName();

      };

   private:
      /**
      mutex for  class data
      */
      Mutex mutex;

      /** folder access capabilities */
      int cap;

      /**
       number of files which may be simultaneously opened on this
       dation
      */
      int capacity;

      /**
       number of files which are currently opened on this
       dation
      */
      int usedCapacity;


      FatFsFile**  object;

      char * devicePath; //< C-string of the directory

   public:

      /**
       Constructor to setup the system device

       \param dev path to the directory including the volume specification
     (e.g. "0:files/" for the folder 'files' on volume "0:")
       \param nbrOfFiles denotes the number of files opened
              at one time on this folder/disc

       \throws DationParamSignal if dev denotes no folder name
       \throws DationParamSignal if dev does not end with '/'
       \throws DationParamSignal if the required number of
                   channels could not be allocated
      */
      FatFs(const char* dev, const int nbrOfFiles);

      /**
      need destructor to remove array allocation
      */
      ~FatFs();

      /**
         return capabilities of the folder objects

         The capabilities are marked be using the flags from
         Dation. The meaning of the flags are modified.
         E.g IN is means that the folder supports IN.


         \returns capabilities of the real folder
      */
      int capabilities();

      /**
       open method

       A free DiscFile object is searched in the pool.
       The operation is delegated to this DiscFile object.

      \param fileName   the file name as C string
      \param openParams the PEARL parameters for file open

      \returns pointer to working object to do the subsequent
                  dation operations.

      \throws OpenFailedSignal in case of errors
      \throws DationParamSignal in case of errors
      */
      FatFsFile* dationOpen(const char * fileName, int openParams);

      /**
       close method.

       This method is empty.
       All operations are in the DiscFile::dationClose() method.

      \param closeParams actions to be done at close (PRM, CAN)
      \throws CloseFailedSignal in case of errors

      */
      void dationClose(int closeParams);

      /**
      read method

       This method is empty.
       All operations are in the DiscFile::dationClose() method.

      \param destination target area for the read bytes
      \param size number of bytes to read

      \throws ReadingFailedSignal in case of read errors

      */
      void dationRead(void * destination, size_t size);

      /**
      write method

       This method is empty.
       All operations are in the DiscFile::dationClose() method.

      \param destination source area for the bytes to be written
      \param size number of bytes to write

      \throws WritingFailedSignal in case of write errors

      */
      void dationWrite(void * destination, size_t size);

      /**
       send one character back to the input.

      \param c the character which shall returned to the input

      \throws * depends an the implementation of the SystemDationNB device
      */
      void dationUnGetChar(const char c);

      /**
      translate newline

      this is empty since linux uses \n for newline

      \param doNewLineTranslation enbale/disable the translation
             (has no effect)
      */
      void translateNewLine(bool doNewLineTranslation);

      /**
      retrieve the path to the device name, which is needed in the FatFsFile
      object

      \returns the pointer to the device path
      */
      char* getDevicePath();

      /**
      \brief register a volume (e.g. a device) to the FAT-Filesystem library

      The original code of Elm Chan supposes a switch case construction
      to select the required i/o driver for all supported the volumes,
      which are identified by the file path.  

      To safe memory space, the setup in OpenPearl is different.
      The device drivers register themself at the FAT-Filesystem library via
      this method.  Thus it it possible to omit device drivers, which are
      not used in an application.

      \param nbr the volume number (0,1,...)
      \param v pointer to the volume working storage.
               The storage itself is allocated (typically static) 
               in the device drivers module.
      */
      static void registerVolume(int nbr, FatFsVolume * v);
   };
}
#endif
