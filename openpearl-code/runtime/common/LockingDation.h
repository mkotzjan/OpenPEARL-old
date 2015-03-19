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
   class LockingDation;
}
#ifndef LOCKINGDATION_INCLUDED
#define LOCKINGDATION_INCLUDED

/**
\file

\brief class which provides mutual exclusion locking for dation operations

*/

#include "Mutex.h"
#include "Dation.h"
#include "Signals.h"
#include "TaskCommon.h"

namespace pearlrt {
   /**
     This class provides mutual exclusion locking for dation operations.

     Single PEARL I/O-statements are mapped to multiple C++ methods call.
     These C++ statements should not become interrupted by others tasks
     working on the same dation.
     Therefore a locking mechanism is needed.

     The compiler generates a code structure as follows.

     PEARL-Code:
     \code
     WRITE a,b,c TO mydation BY POS(1,2,3),RST(x);
     \endcode
     C++:
     \code
     try {
        mydation.beginSequence(me);
           mydation.pos(1,2,3);
           mydation.rst(x);
           mydation.write(&a,sizeof(a));
           mydation.write(&b,sizeof(b));
           mydation.write(&c,sizeof(c));
        mydation.endSequence();
     } catch (Signal &s ) {
        mydation.endSequence();
        if (!mydation.updateRst(&s)) {
          throw;
        }
     }
     \endcode
     All exceptions in the try catch block will be handed towards
     the updateRst()-method. If the rst-formatting element was set,
     the exception is treated, else it is rethrown.
     The call of endSequence() in the catch handler enshures proper
     unlocking of the dation object.

     In the example above, problems in the pos()-method will rethrow
     the exception, problems after the rst-call will treat the
     exception by setting the rst-variable.

     A simple mutex variable is defined and some methods to lock
     and unlock this variable according the dation operations.

     Furthermore, this class is responsible to cooperate with Task.h
     to realize suspend/continue/.. while dation operations are in progress.
     The current task working with this dation is specified by 'me' at
     the call of beginSequence().
   */
   class LockingDation : public Dation {
   private:
      Mutex mutex;
      Fixed<15> * rstValue;
   protected:
      /** pointer to the task, which performs an i/o-operation on this
          dation
      */
      TaskCommon* currentTask;

   public:
      /**
      ctor presets the attributes
      */
      LockingDation();

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
