/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Rainer Mueller
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

/**
\file

\brief  priority mapper

This module provides the mapping algorithm from PEARL
priorities to system priorities.

*/

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>

#include "Log.h"
#include "Fixed.h"
#include "Signals.h"
#include "PrioMapper.h"

namespace pearlrt {

   PrioMapper* PrioMapper::instance = NULL;

   PrioMapper::PrioMapper() {
      min = sched_get_priority_min(SCHED_RR);
      max = sched_get_priority_max(SCHED_RR);

      if (max == -1 || min == -1) {
         Log::error((char*)"error obtaining system priority range");
         exit(1);
      }
   }

   PrioMapper* PrioMapper::getInstance() {
      if (instance == NULL) {
         instance = new PrioMapper();
      }

      return instance;
   }

   int PrioMapper::fromPearl(const Fixed<15> p) {
      // max is reserved for the os-thread
      // min is reserved for PEARL-Prio 255 (no prio specified)
      int sys = max - p.x ;

      if (p.x == 255) {
         sys = min;
      } else if (sys <= min) {
         Log::error((char*)"the requested priority is not available");
         throw thePriorityNotMapableSignal;
      }

      return sys;
   }

   int PrioMapper::getSystemPrio() {
      return max;
   }

   void PrioMapper::logPriorities() {
     // mapping
     // Sched_fifo: 1..99 or min..max
     // reserved:   1 -> mapped to PEARL priority 255
     //            99 -> reserved for activities inside the runtime system
     //            --- 2 priorities  are reserved
     //  remaining priorities: max-2
     Log::info("available PEARL priorities: 1,..., %d, 255", max-2);
   }

}  // end of name space
