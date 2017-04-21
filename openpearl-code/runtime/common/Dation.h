/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
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

#ifndef DATION_INCLUDED
#define DATION_INCLUDED

/**
\file

\brief dation interface

*/


#include "Device.h"
#include "Fixed.h"

namespace pearlrt {

   /**
   \addtogroup io_common_helper
   @{
   */

   /**
   \brief general interface for all types of dations.

          This class defines the interface for the basic operations
          dationRead, dationWrite and dationSeek and common attributes,
          which have either to be implemented in all kinds of dations
          or are all the same in all kinds of dations.

          dationOpen, dationClose differ between system
          dations and user dations
   */
   class Dation: public Device {

   public:

      /**
       Direction Attribute.

       Defines the possible transfer of a direction to/from the dation

       IN: Read from Dation
       OUT: write to dation
       INOUT: read and write

      Possible open params for file based dations.

      Describe how to handle the station by open

      ANY: Open the dation by idf. if no idf create tmp file
      NEW: Create a dation by idf. if idf is missing or dation
      already exists->exception
      OLD: Open the dation by idf. if idf is missing or dation
      doesn't exist->exception

      \note default is ANY

      Possible close params for file based dations.

      Describe how to handle the station after close

      PRM: Dation has to be saved after closing
      CAN: Delete the Dation after closing

      \note default is PRM
      */
      enum DationParams {
         /** direction: only input */
         IN = 0x0001,
         /** direction: only output */
         OUT = 0x0002,
         /** direction: input and output */
         INOUT = 0x0004,
         /** open with filename */
         IDF = 0x0008,
         /** open behavior: may exist, create if not */
         ANY = 0x0010, 
         /** open behavior: must not exist, create new */
         NEW = 0x0020,
         /** open behavior: must exist */
         OLD = 0x0040,
         /** close behavior: do not delete  */
         PRM = 0x0080,
         /** close behavior: delete  */
         CAN = 0x0100,
         /** support direct positioning on the device */
         DIRECT = 0x0200,
         /** supports sequential forward read/write access */
         FORWARD = 0x0400,
         /** supports sequential reading and backward sequential
         positioning 
         \note not implemented
         */
         FORBACK = 0x0800,

         /** has RST variable set */
         RST = 0x1000,

         /** dimension handling:
          automatical rewind at the end of the specified dimension
         */
         CYCLIC = 0x2000,

         /** dimension handling:
          no automatical rewind at the end of the specified dimension (default)
         */
         NOCYCL = 0x4000,

         /** dimension handling:
          silent ignore the end of an internal dimension limit (default)
         */
         STREAM = 0x08000,

         /** dimension handling:
          do not ignore the end of an internal dimension limit
         */
         NOSTREAM = 0x10000
      };

      /**
      mask to select the direction concerning bits from
      a parameter or capability value
      */
      static const int DIRECTIONMASK = IN | OUT | INOUT;

      /**
      mask to select the opening concerning bits from
      a parameter or capability value
      */
      static const int OPENMASK = IDF | ANY | OLD | NEW | CAN | PRM;

      /**
      mask to select the closing concerning bits from
      a parameter or capability value
      */
      static const int CLOSEMASK = PRM | CAN;


      /**
      mask to select the positioning concerning bits from
      a parameter or capability value
      */
      static const int POSITIONINGMASK = DIRECT | FORWARD | FORBACK;

      /**
      mask to select the dim handling concerning bits from
      a parameter or capability value
      */
      static const int DIMMASK = STREAM | CYCLIC | NOSTREAM | NOCYCL;

      /**
       Status of the dation
      */
      enum DationStatus {
         /** dation is open */
         OPENED = 1, 
         /** dation is closed */
         CLOSED = 2
      };


      /**
      Classinternal status variables for open and
      close statements

      there are different sections within this variable.

      via OPENMASK, CLOSEMASK, POSITIONINGMASK  they can be selected
      */
      /* DationParams*/ int dationParams;

      /** Classinternal stat variable for dation stat.
          should prevent from open a already opened dation
      */
      DationStatus dationStatus;


      /**
      Read from the device/file.

      \param destination target area to store the required number of bytes
      \param size number of bytes to read

      \throws * may throw exceptions in case of problems in execution
      */
      virtual void dationRead(void * destination, size_t size) = 0;

      /**
      Write to the device/file.

      \param destination source area to get the required number of bytes
      \param size number of bytes to write

      \throws * may throw exceptions in case of problems in execution
      */
      virtual void dationWrite(void * destination, size_t size) = 0;

      /**
      set absolut position on dation/file

      The method applies only on DIRECT or FORBACK dations.
      FORWARD dations will write 0 bytes or discard input on read.

      \param p the target position of the read/write pointer (conted in bytes)
      \param dationParam specified the dation type (DIRECT,FORWARD,..)
      \throws may throw different exceptions - not defined yet
      */
      virtual void dationSeek(const Fixed<31> & p, const int dationParam) ;

   };
   /** @} */
}
#endif
