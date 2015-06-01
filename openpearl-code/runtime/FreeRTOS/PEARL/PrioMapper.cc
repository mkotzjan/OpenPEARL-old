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
 priorities to FreeRTOS priorities.

 */

#include <stdio.h>
#include <stdlib.h>

#include "Log.h"
#include "Fixed.h"
#include "Signals.h"
#include "PrioMapper.h"
#include "FreeRTOS.h"

#if configMAX_PRIORITIES < 260
#error PrioMapper: configMAX_PRIORITIES too small
#endif

namespace pearlrt {

   PrioMapper* PrioMapper::instance = NULL;


   PrioMapper::PrioMapper() {
      max = configMAX_PRIORITIES - 4;  // timer task has configMAX_PRIORITIES-1
      min = 1;                       // idle task has priority 0

      if (max < 256) {
         Log::error((char*) "PrioMapper: configMAX_PRIORITIES too small");
         //exit(1);
      }
   }

   PrioMapper* PrioMapper::getInstance() {
      if (instance == NULL) {
         instance = new PrioMapper();
      }

      return instance;
   }

   int PrioMapper::fromPearl(const Fixed<15> p) {
      unsigned int sys = max - p.x;

      return sys;
   }

}
