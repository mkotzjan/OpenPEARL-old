/*
 [A "BSD license"]
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

#ifndef SOFTINT_INCLUDED
#define SOFTINT_INCLUDED

#include <signal.h>
#include "Interrupt.h"
/**
\file
*/

namespace pearlrt {
   /** 
   \addtogroup interrupt
   @{
   */

   /**
   \brief pseudo interrupt source
   */

   /**
   This class provides some software interrupt sources

   */
   class SoftInt : public Interrupt {
   private:
      static int isSet;   //< bitmap of signals in use

   public:
      /**
      create a interrupt handler for the given SoftInt


      \param nbr software interrupt number, which must be in the range 1..32
      */
      SoftInt(int nbr);

      /**
      enable interrupt in interrupt emitting device

      the device shall only trigger the treatment, if the interrupt
      is enabled by the application. This method is called from the 
      enable()-method.
      */
      void devEnable();

      /**
      disable interrupt in interrupt emitting device

      the device shall only trigger the treatment, if the interrupt
      is enabled by the application. This method is called from the 
      disable()-method.
      */
      void devDisable();

   };
   /** @} */
}

#endif

