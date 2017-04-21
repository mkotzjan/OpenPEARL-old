/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014-2014 Rainer Mueller
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

#ifndef PIPE_INCLUDED
#define PIPE_INCLUDED


/**
\file

\brief generic non-basic systemdation class for reading and writing
       file from/to named pipes
*/

#include "SystemDationNB.h"
#include "Mutex.h"
#include "Character.h"
#include "RefChar.h"
#include <unistd.h>

namespace pearlrt {
   /**
   \addtogroup io_linux_driver
   @{
   */

   /**
   \brief generic non-basic systemdation class

   With this class it is possible to generate named pipe
   system devices. They are useful for testing the dation behavior
   and for data exchange with other applications.

   The implementation uses the class Pipe as container for PipeFile
   objects, which perform the i/o-operations.

   One parameter of the Ctor defines the number of open dations
   on one Pipe-object. The PipeFile objects are allocated in a pool,
   which is created at instantiation of the Pipe-object.
   At this point of execution the 'new' operation is allowed.

   */

   class Pipe: public SystemDationNB {

   public:
      /**
      A PipeFile represents a file on a Pipe. This class is responsible
      for all operations with the  operating system.
      */
      class PipeFile : public SystemDationNB {
      private:
         FILE * fp;
         Pipe * 	myPipe;

      public:
         /**
            shows whether this pool element is in use
         */
         bool inUse;

         /**
            ctor of working pool element

            \param disc pointer to parent Pipe-object
         */
         PipeFile(Pipe* disc);

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
         \throws IllegalParamsSignal in case of errors
         */
         PipeFile* dationOpen(const char * fileName, int openParams);

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

      };
   private:
      /**
      mutex for parent data
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


      PipeFile**  object;

      /*
         all pipe channels work on the same file name
          ==> static class member
      */
      static char * devicePath; //< C-string of the fifo file name

      FILE* defaultReader;  //< default reader if required
      bool removeFile;      //< remove flag if set in ctor params
   public:

      /**
       Constructor to setup the system device.

       The default parameters are: ANY, PRM

       The paremeters OLD, NEW, ANY, PRM and CAN work like their definition
       at dations.

       The parameter OPEN1 forces to open one reading end of the pipe to
       avoid blocking of write operations as long as no user defined reader
       is preset. The blocking at filled pipe will still happen.

       \param name name of the pipe
       \param nbrOfFiles denotes the number of files opened
              at one time on this folder/disc
       \param params denote the construction details like NEW,OLD,ANY,
                CAN, PRM, OPEN1.
               The parameters are searched in the given string.
               Additional characters are ignored.
               The comparison is case sensitiv!

       \throws IllegalPathSignal, if the folder attributes
                     are not readable (folder not existent)
       \throws IllegalPathSignal, if dev denotes no folder name
       \throws IllegalPathSignal, if dev does not end with '/'
       \throws IllegalParamSignal, if the required number of
                   channels could not be allocated
      */
      Pipe(const char* name, const int nbrOfFiles, const char* params = NULL);

      /**
      need destructor to remove array allocation are
      */
      ~Pipe();

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

       A free PipeFile object is searched in the pool.
       The operation is delegated to this PipeFile object.

      \param fileName   the file name as C string
      \param openParams the PEARL parameters for file open

      \returns pointer to working object to do the subsequent
                  dation operations.

      \throws OpenFailedSignal in case of errors
      \throws IllegalParamsSignal in case of errors
      \throws IllegalPathSignal in case of errors
      */
      PipeFile* dationOpen(const char * fileName, int openParams);

      /**
       close method.

       This method is empty.
       All operations are in the PipeFile::dationClose() method.

      \param closeParams actions to be done at close (PRM, CAN)
      \throws CloseFailedSignal in case of errors

      */
      void dationClose(int closeParams);

      /**
      read method

       This method is empty.
       All operations are in the PipeFile::dationClose() method.

      \param destination target area for the read bytes
      \param size number of bytes to read

      \throws ReadingFailedSignal in case of read errors

      */
      void dationRead(void * destination, size_t size);

      /**
      write method

       This method is empty.
       All operations are in the PipeFile::dationClose() method.

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
  
      this is emptyi since linux uses \n for newline
      
      \param doNewLineTranslation enbale/disable the translation
             (has no effect)
      */
      void translateNewLine(bool doNewLineTranslation);

   };
   /** @} */
}
#endif
