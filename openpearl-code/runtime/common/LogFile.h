/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

#ifndef LOGFILE_INCLUDED
#define LOGFILE_INCLUDED

#include "SystemDationNB.h"

/**
\file

\brief configuration element for a log file

*/

namespace pearlrt {

   /**
      This class provides a connection to a file , which is used
      by the logging facility.
   */
   class LogFile : public SystemDationNB {
      public:
        /**
        Constructor of the LogFile element

        \param provider pointer to the connection provider
        \param filename name of the log file

       \throws IllegalParamSignal, if the file name is invalid, or
             provider is null
        */
 	 LogFile(SystemDationNB * provider, char const* filename);


      private:
	char const * logFileName;  // just a pointer to the given filename
        SystemDationNB * provider;

      /** access capabilities */
      int cap;

      /**
      File* to use
      */
      FILE* fp;
     
public: 
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
      */
      LogFile* dationOpen(const char * fileName, int openParams);

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
       This kind of element does not support reading

      \param destination target area for the read bytes
      \param size number of bytes to read

      \throws InternalDationSignal if called 

      */
      void dationRead(void * destination, size_t size);

      /**
      write method

       delegate send data to the provider

       \param destination adress of the data to be written
       \param size number of bytes to be written

      \throws depending on the connectiopn provider
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
  
      \param doNewLineTranslation enbale/disable the translation
      */
      void translateNewLine(bool doNewLineTranslation);

};
}
#endif

