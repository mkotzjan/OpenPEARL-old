/*
 [A "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014-2016 Rainer Mueller
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

#ifndef STDOUT_INCLUDED
#define STDOUT_INCLUDED


/**
\file

\brief generic non-basic systemdation class for writing
       to standard stream stdout
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

   Usage:
   \verbatim
   SYSTEM;
      stdOut : StdOut;
   PROBLEM;
      SPC stdOut DATION SYSTEM OUT ALL;

      anyTask: TASK;
         DCL console DATION OUT FORWARD DIM(*,80) STREAM CREATED(stdOut);
         OPEN console; ! only RST allowed on StdOut
         PUT 'PEARL' TO console BY A,SKIP;
         CLOSE console;
      END;
   \endverbatim

   */
   class StdOut: public SystemDationNB {

   private:
      /**
      mutex for  class data
      */
      Mutex mutex;

      /** access capabilities */
      int cap;

      /** flag, whether device is in use */
      bool inUse;

      /**
      File* to use
      */
      FILE* fp;

      static int declaredDations;

   public:

      /**
       Constructor to setup the system device

       PEARL attributes: FORWARD IN or OUT ALPHIC or type

      */
      StdOut();

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

       Identify the requested StdOut object and return a
       pointer to this object.

      \param fileName   the file name as C string (must be NULL)
      \param openParams the PEARL parameters for file open

      \returns pointer to working object to do the subsequent
                  dation operations.

      \throws OpenFailedSignal in case of errors
      \throws DationParamSignal in case of errors
      */
      StdOut* dationOpen(const char * fileName, int openParams);

      /**
       close method.

       This method is empty.

      \param closeParams actions to be done at close (PRM, CAN)
      \throws CloseFailedSignal in case of errors

      */
      void dationClose(int closeParams);

      /**
      read method

      \param destination target area for the read bytes
      \param size number of bytes to read

      \throws DationNotSupportedSignal in any case

      */
      void dationRead(void * destination, size_t size);

      /**
      write method


      \param destination source area for the bytes to be written
      \param size number of bytes to write

      \throws WritingFailedSignal in case of write errors

      */
      void dationWrite(void * destination, size_t size);

      /**
       send one character back to the input.

      \param c the character which shall returned to the input

      \throws DationNotSupportedSignal in any case
      */
      void dationUnGetChar(const char c);

      /** check if at least one dation of the given type is defined
      */
//     static bool isDefined(const int streamNumber);

      /**
      translate newline

      this is empty since linux uses \n for newline

      \param doNewLineTranslation enbale/disable the translation
             (has no effect)
      */
      void translateNewLine(bool doNewLineTranslation);

   };
   /** @} */
}
#endif
