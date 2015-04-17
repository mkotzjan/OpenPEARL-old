/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
 Copyright (c) 2013-2014 Holger Koelle
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

namespace pearlrt {
   class UserDation;
}
#ifndef USERDATION_INCLUDED
#define USERDATION_INCLUDED

/**
\file

\brief class which provides mutual exclusion locking for dation operations

*/

#include "Mutex.h"
#include "SystemDation.h"
#include "Signals.h"
#include "TaskCommon.h"
#include "RefChar.h"

namespace pearlrt {
   /**
     This class provides mutual exclusion locking for dation operations.

     Single PEARL I/O-statements are mapped to multiple C++ methods call.
     These C++ statements should not become interrupted by others tasks
     working on the same dation.
     Therefore a locking mechanism is needed.

     The dation operations may throw exceptions. The mutual excluded
     block must be wrapped in a try atch block.

     All exceptions in the try catch block will be handed towards
     the updateRst()-method. If the rst-formatting element was set,
     the exception is treated, else it is rethrown.
     The call of endSequence() in the catch handler enshures proper
     unlocking of the dation object.

     A simple mutex variable is defined and some methods to lock
     and unlock this variable according the dation operations.

     Furthermore, this class is responsible to cooperate with Task.h
     to realize suspend/continue/.. while dation operations are in progress.
     The current task working with this dation is specified by 'me' at
     the call of beginSequence().
   */
   class UserDation : public Dation {
   private:
      Mutex mutex;
      Fixed<15> * rstValue;
   protected:
      /** pointer to the task, which performs an i/o-operation on this
          dation
      */
      TaskCommon* currentTask;

      /** dation parameters and attribute from from declaration.
          This variable contains a collection of bits, which are
          defined in Dation.h
      */
//      int dationParams;

   public:
      /**
      ctor presets the attributes
      */
      UserDation();

      /**
        internal open function. build path to device, performs
        pre and post Open and creates the FILE descriptor (dev)

        \note have to be implemented in system
               specific Dation implementation
      */

//      virtual void internalOpen() = 0;

      /**
        internal close function. Close dev and delete it
        (if closeParam CAN is set)

        \note have to be implemented in system
              specific Dation implementation

      */
 //     virtual void internalClose() = 0;

   private:
      /**
       Implementation of the internal Open-interface.

       \param p open parameters (with default setting
       \param rc RefChar with file name

       \note throws various exceptions if no RST-Variable is set
      */
      virtual void internalDationOpen(int p,
                              RefCharacter* rc) = 0;

   public:
      /**
        Implementation of the Open-interface.

        \param p open parameters as given
       \param rst pointer to rst-variable; required, if RST is set in p

        \note throws various exceptions if no RST-Variable is set
      */
      void dationOpen(int p = 0,
                      Fixed<31> * rst = 0) ;

      /**
        Implementation of the Open-interface.

        If nether OLD,NEW,ANY or PRM,CAN is given the default is set,
        which is ANY + PRM


        \param p open parameters as given
        \param idf file name (used if IDF is set in p)
       \param rst pointer to rst-variable; required, if RST is set in p

        \note throws various exceptions if no RST-Variable is set
      */
      template <size_t S>
      void dationOpen(int p = 0,
                      Character<S> * idf = (Character<S>*)0,
                      Fixed<31> * rst = 0) {
         try {
            if (p & RST) {
               if (! rst) {
                  Log::error("UserDation: RST is set but no"
                             " variable given");
                  throw theIllegalParamSignal;
               }
            }

            if ((!!(p & Dation::IDF)) != (idf != 0)) {
               Log::error("UserDation: ether both or non of IDF and filename");
               throw theIllegalParamSignal;
            }

            if (S > 64) {
               Log::error("filename exceeds 64 characters");
               throw theIllegalParamSignal;
            }

            RefCharacter rc;
            Character<64>  fileName;
            rc.setWork(fileName);
            rc.clear();

            if (p & Dation::IDF) {
               rc.add(*idf);
            }

            internalDationOpen(p, &rc);
         } catch (Signal & s) {
            if (rst) {
               *rst = s.whichRST();
            } else {
               throw;
            }
         }
      }


      /**
        Implementation of the Close-interface, which is inherited
        from UserDation Basic-class

       \param p close parameters if given, else 0
       \param rst pointer to rst-variabla; required, if RST is set in p

        \note throws various exceptions if no RST-Variable is set

      */
      void dationClose(const int  p = 0, Fixed<31> * rst = 0);

      /**
      interface to close the BASIC or NON-Basic system dation
      \param dationParams contains tge dation parameters
      \note may throw exceptions
      */
      virtual void closeSystemDation(int dationParams) = 0;

   protected:
      /** assert dation properties

       \throw NotAllowedSignal if condition is not met
      */
      void assertOpen();

   public:
      /**
      Aquired the mutex to enshure atomic operation on the dation.

      This method may throw an exception in case of problems with the mutex
      operations.

      This method clears the rstValue attribute.

      \param me pointer to the task which performs the i/o.
                    May be NULL for testing purpose. Then no suspend and
                    terminate is done during the i/o-operation
      */
      void beginSequence(TaskCommon * me);

      /**
      Free the mutex to mark the end of the atomic operation on the dation.

      This method may throw an exception in case of problems with the mutex
      operations.
      */
      void endSequence();

      /**
        set the rst variable
        \param rstVar the variable, which should be set is case of
                      exception after this call
      */
      void rst(Fixed<15> & rstVar);

      /**
         update the RST variable if set
         \param s pointer to the sigbal which was caught
         \returns true, if RST-variable was defined
         \returns false, if RST-variable was NOT defined
      */
      bool updateRst(Signal * s);
public:
      /**
       suspend

       suspend doing the io freeing the mutex
      */
      void suspend();

      /**
       cont

       continue doing the io requesting the mutex
      */
      void cont();

   };
}
#endif
