/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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

#ifndef CONTROL_INCLUDED
#define CONTROL_INCLUDED

#include "Fixed.h"


/**
\file

\brief operations to control the base system

*/

namespace pearlrt {
   /**
   Provide operations to control the base system, like set the exit-code.

   These static methods are intended to enable the pearl application
   to have access to some internal features of the system.
   */
   class Control {
   private:
      Control();		// no ctor available
      static int exitCode;      // the exit code of the application
   public:
      /**
        set the exit code of the application.

        The application will return the latest set value of this method call.
      The preset value is 0, which means 'no error'.

      \param x the new exit code
           */
      static void setExitCode(const Fixed<15> x);

      /**
        get the exit code of the application.

        \returns the value of the desired exit code.
      */
      static int getExitCode();
   };
}

#endif

