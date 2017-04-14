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
#ifndef STRINGDATIONCONVERT_INCLUDED
#define STRINGDATIONCONVERT_INCLUDED
/**
\file

\brief a kind of dation upon a string for the CONVERT statement

*/
#include "RefCharSink.h"
#include "RefCharSource.h"
#include "Rst.h"
#include "IOFormats.h"

namespace pearlrt {
   /**
     Defines the methods for the communication of userdations
     (with class attribute "ALPHIC").
     They read/write with the methods PUT/GET (defined by PEARL)
     in external format.
     Some interfaces are inherited from upper classes UserDation.

     The i/o-operations are done via a source/sink object which decouples
     the i/o from the formatting statements.

   PEARL Example

   \code
   Start: TASK MAIN
      DCL x FIXED(15) INIT(2);
      DCL string CHAR(30);
      CONVERT 'X=', x TO string BY A, F(3);
      CONVERT  x FROM string BY X(3), F(3);
   END;
   \endcode

    C++
   \code

   Task(start,255,09 {
    ...    
    {  // CONVERT TO start 
        pearlrt::RefCharacter rc(string);
                                    // true indicates output
        pearlrt::StringDationConvert strDation(rc, true);
            
        try {
           strDation.toA(Character<3>("X="));
           strDation.toF(x,3);
        } catch (Signal& s) {
           if (!strDation.updateRst(&s)) {
              throw;
           }
        }
    } // convert end

    {  // CONVERT FROM start 
        pearlrt::RefCharacter rc(string);
                                   // false indicates input
        pearlrt::StringDationConvert strDation(rc, false);
            
        try {
           strDation.fromX(3);
           strDation.fromF(x,3);
        } catch (Signal& s) {
           if (!strDation.updateRst(&s)) {
              throw;
           }
        }
    } // convert end
   \endcode

   */
   class StringDationConvert: public IOFormats, public Rst {

   private:
      RefCharacter* string;

      RefCharSink   sink;
      RefCharSource source;

      Fixed<31> currentPosition;
      bool isOutput;

   public:
      /**
        create a StringDationConvert object for a CONVERT statement

        \param string pointer to the RefCharacter object containing 
                      the soure/target string
        \param isOutput flag to select source/sink
      */
      StringDationConvert(RefCharacter* string, bool isOutput);

      /**
       return a character back to the input source
       \param c the character to be returned
       */
      void dationUnGetChar(const char c);

     /**
      check if enough space/data is available for the operation

      \param n number of bytes which are wanted to read or write

      \throws CharCCCCC if the string has less space
      */
      void checkCapacity(Fixed<31> n);

      /**
      X-format for output

      \param n number of spaces to write
      */
      void toX(Fixed<31> n);

      /**
      X-format for input

      \param n number of characters to skip
      */
      void fromX(Fixed<31> n);

      /**
      ADV-format for in-/output

      \param n delta to new  position
      */
      void adv(Fixed<31> n);

      /**
      POS-format for in-/output

      \param n new position
      */
      void pos(Fixed<31> n);

      /**
      SOP-format for in/output

      \param n current postion
      */
      void sop(Fixed<31> &n);

   };
}
#endif
