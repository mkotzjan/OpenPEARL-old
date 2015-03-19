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

#ifndef STDSTREAM_INCLUDED
#define STDSTREAM_INCLUDED


/**
\file

\brief generic non-basic systemdation class for reading and writing
       file from/to stanadrd streams (stdin, stdout, stderr)
*/

#include "SystemDationNB.h"
#include "Mutex.h"
#include "Character.h"
#include "RefChar.h"
#include <unistd.h>

namespace pearlrt {

   /**
   \brief generic non-basic systemdation class

   With this class it is possible to generate linux stream
   devices.

   The parameter of the Ctor defines which stream should be used.

   Usage:
   \verbatim
   SYSTEM;
      stdOut : StdStream(1); ! 0=stdin, 1=stdout, 2=stderr
   PROBLEM;
      SPC stdOut DATION OUT ALL;

      anyTask: TASK;
         DCL console DATION OUT FORWARD DIM(*,80) STREAM CREATED(stdOut);
         OPEN console; ! only RST allowed on StdStream
         PUT 'PEARL' TO console BY A,SKIP;
         CLOSE console;
      END;
   \endverbatim

   */

   class StdStream: public SystemDationNB {

   private:
      /**
      mutex for  class data
      */
      Mutex mutex;

      /** access capabilities */
      int cap;

      /** flag, whether deviceis in use */
      bool inUse;

      /**
       number of files which may be simultaneously opened on this
       dation
      */
      int capacity;

      /**
      File* to use
      */
      FILE* fp;
      
      static int declaredDations;

   public:

      /**
       Constructor to setup the system device

       PEARL attributes: FORWARD IN or OUT ALPHIC or type

       \param streamNumber number  of the system stream
                        (0=stdin, 1=stdout, 2=stderr)

       \throws IllegalParamSignal, if the required stream
                   is invalid
      */
      StdStream(const int streamNumber);

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

       Identify the requested StdStream object and return a
       pointer to this object.

      \param fileName   the file name as C string (must be NULL)
      \param openParams the PEARL parameters for file open

      \returns pointer to working object to do the subsequent
                  dation operations.

      \throws OpenFailedSignal in case of errors
      \throws IllegalParamsSignal in case of errors
      \throws NoFileInfoSignal in case of errors
      */
      StdStream* dationOpen(const char * fileName, int openParams);

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

     /** check if at least one dation of the given type is defined 
     */
     static bool isDefined(const int streamNumber);

   };
}
#endif
