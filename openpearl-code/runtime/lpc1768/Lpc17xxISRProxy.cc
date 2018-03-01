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

#include <stdio.h>
#include "Lpc17xxISRProxy.h"

namespace pearlrt {

   /**
   macro to create the
   <ul>
   <li> storage variable for the function pointer,
   <li> the interrupt service routine
   <li> the setter method to update the function pointer
   </ul>

   \param ISR_NAME ist the name of the isr as specified in startup.S
   */
#define PLAIN_PROXY(ISR_NAME)				\
   static ISR ptr##ISR_NAME=NULL;			\
extern "C" {						\
   void ISR_NAME(void) {				\
      if (ptr##ISR_NAME) {				\
	(*ptr##ISR_NAME)();				\
      } else {						\
	printf(#ISR_NAME " not set but invoked\n");	\
      }							\
   }							\
};							\
   void Lpc17xxISRProxy::set_##ISR_NAME(ISR isr) {	\
	ptr##ISR_NAME = isr;				\
   }

   PLAIN_PROXY(USB_IRQHandler)


}


