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
#ifndef DATIONRW_INCLUDED
#define DATIONRW_INCLUDED
/**
\file

\brief READ/WRITE-userdation interface for userdations
       with the class attribute "TYPE"

*/
#include "UserDationNB.h"
#include "SystemDationNB.h"
#include "DationDim.h"
#include "Fixed.h"




namespace pearlrt {
   /**
   \addtogroup io_common
   @{
   */

   /**
   \brief READ/WRITE-userdation interface for userdations
           with the class attribute "TYPE"

     Defines the methods for the communication of userdations
     (with class attribute "TYPE").
     They read/write with the methods READ/WRITE (defined by PEARL)
     in binary format.

     The i/o-operations are done via a source/sink object which decouples
     the i/o from the formatting statements.

     The complete operation is treated by a list of data elements and a list
     of format elements. See IODataList and IOFormatList about details.
     For expression results intermediate variable must be defined locally and
     used in the lists. The evaluation of the expressions must be done after
     creation of the lists and the invocation of the write- or read-method.

     The data list and format list must not contain any loop constructs.

     Bit slices are not supported on input. They would need a temporary
     bit variable and an assignment after the read statement. But we have no
     indication, if the read statement was terminated by a catched signal
     before the bit slice was in order to be read. 


   PEARL Example RW Dation

   \code
   SYSTEM;
      home: Disc("/home/", 5);

   PROBLEM;
      SPC home DATION SYSTEM INOUT ALL;
      DCL table DATION OUT FIXED(15) DIM(10,20)
                DIRECT CYCLIC NOSTREAM CREATED(home);

   Start: TASK MAIN
      DCL x FIXED(15);
      DCL y FIXED(15);

      OPEN table BY IDF('tmp1'), ANY;

      ! ...
      WRITE i*100+j TO logbuch BY POS(i,j);
      ! ...

      CLOSE table;

   END;
   \endcode

    C++
   \code
   // SYSTEM
   Disc s_home("/home/", 5);
   Device* d_home = & s_home;

   // PROBLEM
   extern Device* d_home;

   // on the basis of the keyword DATION and the keyword ALL,
   //  we know it's a Non Basic Systemdation, so we cast it
   SystemDationNB* _home = static_cast<SystemDationNB*>(d_home);

   // Userdation table is a DationRW (key datatype specified FIXED(15)),
   //  parent system dation is _home, direction is OUT (write only),
   //  size of a positioning step is 1 byte.
   // positioning stepsize is the size of datatype (byte),
   //  here: sizeof(Fixed<15>)
   // Constructor sets default params for the dation internally
   // if not specified else. Defaults: ANY,PRM
   DationDim2 d2(10,20);
   DationRW table (_home,
                   d2,
                   Dation::OUT | Dation::DIRECT |
                   Dation::CYCLIC | Dation::NOSTREAM,
                   sizeof(Fixed<15>));

   Task(start,255,09 {
       pearlrt::Character<5> tmp(5,(char*)"tmp1");
       table.dationOpen(Dation::IDF, &tmp);
    
       {
          // declare local variables for expression results
          // ...
          // create data and format lists (see IODataList and IOFormatList) 
          // ...
          // evaluate expressions in try-catch-blocks and modify
          // the corresponding list entry in case of a catched signal
          table.write(me, ioDataList, ioFormatList);
      }

      table.dationClose();
   }
   \endcode

      */

   class DationRW: public UserDationNB {

   private:

      /**
        internal open function. build path to device, performs
        pre and post Open and creates the FILE descriptor (dev)

      */
      void internalOpen();

      /**
        internal close function. Close dev and delete it
        (if closeParam CAN is set)

      */
      void internalClose();

      /**
      fill a space with nil bytes

      \param n number of bytes to fill
      */
      void zeroFill(const Fixed<31> n);

   public:




      /**
        create a new READ/WRITE userdation (class attribute "TYPE")

        \param parent pointer to the "parent" systemdation
        \param dationParams possible dation parameters like
               IN,INOUT, OUT, STREAM, ..
        \param dimensions a pointer to a dimension object
        \param stepsize size (in bytes) of steps for positioning the dation.
               This is the size of the data structure which will be used
               in this dation. The size is calculated in bytes.
        \param tfubuffer pointer to the TFU buffer, if TFU is set, else NULL
      */
      DationRW(SystemDationNB* parent,
               int dationParams,
               DationDim * dimensions,
               const Fixed<15> stepsize,
               void * tfubuffer = NULL);

      /**
        Method for reading from a rw-userdation

        \param data pointer to the variable
        \param size number of bytes of the variable to read

        \note throws various exceptions
      */
      void dationRead(void* data, size_t size);

      /**
        Method for writing data to a rw-userdation

        \param data pointer to the variable
        \param size number of bytes of the variable to write

        \note throws various exceptions
      */
      void dationWrite(void* data, size_t size);

      /**
      set absolut position on dation/file

      This method must be overwritten by systemdations which are SEEKABLE

      The method applies only on DIRECT or FORBACK dations.
      FORWARD dations will write 0 bytes or discard input on read.

      \param p the target position of the read/write pointer (counted in bytes)
      \param dationParam specified the dation type (DIRECT,FORWARD,..)
      \throws may throw different exceptions - not defined yet
      */
      void dationSeek(const Fixed<31> & p, const int dationParam);

      /**
       return a character back to the input device
       \param c the character to be returned
       */
      void dationUnGetChar(const char c);

      /**
      send data via the IOJob interface

      \param me the pointer to the calling task
      \param dataList an array of data elements
      \param formatList an array of positioning formats (incl. RST)
      */
      void write(TaskCommon*me,
                 IODataList * dataList, IOFormatList * formatList);

      /**
      read data via the IOJob interface

      \param me the pointer to the calling task
      \param dataList an array of data elements
      \param formatList an array of positioning formats (incl. RST)
      */
      void read(TaskCommon*me,
                IODataList * dataList, IOFormatList * formatList);
   };
   /** @} */
}
#endif
