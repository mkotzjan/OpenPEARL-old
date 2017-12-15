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
#ifndef TFUBUFFER_INCLUDED
#define TFUBUFFER_INCLUDED

#include "Sink.h"
#include "Source.h"
#include "SystemDationNB.h"
#include "Fixed.h"

namespace pearlrt {
   /**
   \file

   \brief TFU buffer operations
   */

   /**
   \addtogroup io_common
   @{
   */

   /**
   the TFU buffer management

   This class provides also the Sink and Source interface, so it is
   easily possible to use the TFUBuffer object as sink and/or source
   in all the formatting routines.
   If no TFU buffer is defined, the sink and source implementations
   work directly on the system dation object.

   */
   class TFUBuffer : public Sink, public Source {
   private:
      SystemDationNB* system;
      int sizeOfRecord;
      char * record;
      int readWritePointer;
      bool containsData;
      int oldRecordNumber;
      int paddingElement;
   public:
      /**
      send the TFU buffer to the output device, if the buffer is not empty
      */
      void flushRecord();

      /** fill the TFU buffer with a complete record from the input device

      \param untilNL if true, read until a \n is detected<br>
                        false, read plain data
      */
      void readRecord(bool untilNL);

      /**
      initialize the buffer management
      */
      TFUBuffer();

      /**
      set the concrete system dation to operate on

      \param sys the system dation which performs and/or the i/o on the
                 device
      */
      void setSystemDation(SystemDationNB* sys);

      /**
      setup the TFU record buffer

      \param recSize the number of bytes of the TFU record
      \param rec pointer to the TFU record buffer
      \param padding the padding element
      */
      void setupRecord(int recSize, char * rec, int padding);

      /**
      fill the tfu buffer with the padding element
      */
      void prepare();

      /**
      check if the tfu buffer mechanisme is used at all.
      \returns true is a tfu buffer was specified<br>
               false else
      */
      bool isUsed();

      /**
      check if there were data transfer operations inside the buffer
      \returns true, if there is some modified data<br>
               false, else
      */
      bool isNotEmpty();

      /**
      mark the tfu buffer to be empty
      */
      void markEmpty();

      /**
      set the read/write pointer inside the tfu buffer to the
      specified location. The position starts counting with 0.

      \param newPos the byte offset in the tfu buffer
      */
      void setPosition(int newPos);

      /**
      write a bunch of data to the TFU buffer or system device

      This method is used in DationRW for output of data

      \param data the pointer to the data to be written
      \param n    the number of bytes to be written
      */
      void write(void * data, size_t n);

      /**
      read a bunch of data from the TFU buffer or system device

      This method is used in DationRW for input of data

      \param data the pointer to the data to be read
      \param n    the number of bytes to be read
      */
      void read(void * data, size_t n);

      /**
      the sink interface: putChar

      This method is used by DationPG to send data to the TFU buffer
      or system dation

      \param c the byte to be written
      */
      void putChar(char c);

      /**
      the source interface: realGetChar

      This method is used by DationPG to get data from the TFU buffer
      or system dation

      \returns the read byte
      */
      char realGetChar();
   };
   /** @} */
}
#endif
