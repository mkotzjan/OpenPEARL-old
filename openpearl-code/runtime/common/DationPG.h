/*
 [A "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014-2017 Rainer Mueller
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
#ifndef DATIONPG_INCLUDED
#define DATIONPG_INCLUDED
/**
\file

\brief PUT/GET-userdation interface for userdations
       with the class attribute "ALPHIC"

*/
#include "UserDationNB.h"
#include "SystemDationNB.h"
#include "DationDim.h"
#include "SystemDationNBSink.h"
#include "SystemDationNBSource.h"
#include "IOFormats.h"
#include "PutFixed.h"
#include "GetFixed.h"
#include "GetFloat.h"
#include "PutFloat.h"
#include "PutCharacter.h"
#include "GetCharacter.h"
#include "PutBitString.h"
#include "GetBitString.h"

namespace pearlrt {

   /**
   \file

   \brief READ/WRITE-userdation interface for userdations
           with the class attribute "ALPHIC"
   */

   /**
     Defines the methods for the communication of userdations
     (with class attribute "ALPHIC").
     They read/write with the methods PUT/GET (defined by PEARL)
     in external format.
     Some interfaces are inherited from upper classes UserDation.

     The i/o-operations are done via a source/sink object which decouples
     the i/o from the formatting statements.

   PEARL Example PG Dation

   \code
   SYSTEM;
      home: Disc("/home/", 5);

   PROBLEM;
      SPC home DATION INOUT ALL;
      DCL table DATION OUT ALPHIC DIM(*,20)
                FORWARD NOCYCLIC STREAM CREATED(home);

   Start: TASK MAIN
      DCL x FIXED(15) INIT(2);
      OPEN table BY IDF('tmp1'), ANY;
      PUT 'X=', x TO table BY A, F(3), SKIP;
      CLOSE table;
   END;
   \endcode

    C++
   \code
   // SYSTEM
   Disc _home("/home/", 5);
   Device* __home = &_home;

   // PROBLEM
   extern Device* __home;

   // on the basis of the keyword DATION and the keyword ALL,
   //  we know it's a Non Basic Systemdation, so we cast it
   SystemDationNB* _home_ = static_cast<SystemDationNB*>(__home);

   // Userdation table is a DationRW (key datatype specified FIXED(15)),
   //  parent system dation is _home, direction is OUT (write only),
   //  size of a positioning step is 1 byte.
   // positioning stepsize is the size of datatype (byte),
   //  here: sizeof(Fixed<15>)
   // Constructor sets default params for the dation internally
   // if not specified else. Defaults: ANY,PRM
   DationDim2 d2(20);
   DationPG table (_home,
                   d2,
                   Dation::OUT | Dation::FORWAD |
                   Dation::NOCYCLIC | Dation::STREAM);

   Task(start,255,09 {
       pearlrt::Character<5> tmp(5,(char*)"tmp1");
       table.dationOpen(Dation::IDF, &tmp);

       try {
          table.beginSequence();
          table,toA(Character<3>("X="));  // << width may be derived from type
          table.toF(x,3);
          table.toSkip(1);
          table.endSequence();
       } catch (Signal& s) {
          table.endSequence();
          if (!table.updateRst(&s)) {
             throw;
          }
       }

       table.dationClose();
   }
   \endcode

      */

   class DationPG: public UserDationNB, public IOFormats {

   private:

      /**
        internal open function.
        create sink and source objects for the formatting routines
      */
      void internalOpen();

      /**
        internal close function.
      */
      void internalClose();

      SystemDationNBSink  sink;
      SystemDationNBSource   source;

      /** helper method for templated parameters */
      //void doPutChar(int length, RefCharacter * rc);

   public:
      /**
        create a new PUT/GET userdation (class attribute "ALPHIC")

        \param parent pointer to the "parent" systemdation
        \param dationParams possible dation parameters like
               IN,INOUT, OUT, STREAM, ..
        \param dimensions a pointer to a dimension object
      */
      DationPG(SystemDationNB* parent,
               int dationParams,
               DationDim * dimensions);

      /**
      Read from the device/file via source.

      \param destination target area to store the required number of bytes
      \param size number of bytes to read

      \throws * may throw exceptions in case of problems in execution
      */
      void dationRead(void * destination, size_t size);

      /**
      Write to the device/file via Sink.

      \param destination source area to get the required number of bytes
      \param size number of bytes to write

      \throws * may throw exceptions in case of problems in execution
      */
      void dationWrite(void * destination, size_t size);

      /**
      set absolut position on dation/file and discard pending ungot characters

      The method applies only on DIRECT or FORBACK dations.
      FORWARD dations will write 0 bytes or discard input on read.

      \param p the target position of the read/write pointer (conted in bytes)
      \param dationParam specified the dation type (DIRECT,FORWARD,..)
      \throws may throw different exceptions - not defined yet
      */
      void dationSeek(const Fixed<31> & p, const int dationParam);

      /**
       return a character back to the input source
       \param c the character to be returned
       */
      void dationUnGetChar(const char c);

      /**
      check if enough space/data is available for the operation

      \param n number of bytes which are wanted to read or write

      \throws DationIndexBoundSignal if the dation has less space
      */
      void checkCapacity(Fixed<31> n);
   };
}
#endif
