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

/**
\file

\brief Mutex implementation with pthread_mutex

*/
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <string.h>

#include "Mutex.h"
#include "Log.h"
//#define IGNORELIST "Log ? TaskMonitor"

namespace pearlrt {
   Mutex::Mutex() {
      pthread_mutex_init(&mutex, NULL);
   }

   Mutex::~Mutex() {
      int ret;
      ret = pthread_mutex_trylock(&mutex);

      if (ret == 0) {
         // was free
         ret = pthread_mutex_unlock(&mutex);
      }  else {
         printf("\n@destroy: mutex %s is locked\n", id);
      }

      ret = pthread_mutex_destroy(&mutex);

      if (ret != 0) {
         fprintf(stderr, "mutex destroy (%s): errno=%d %s\n",
                 id, ret, strerror(ret));
      }
   }

   void Mutex::lock() {
      int ret;
//     if (strstr(IGNORELIST, id) == 0) Log::debug("Mutex::lock(%s)", id);
      ret = pthread_mutex_lock(&mutex);

      if (ret != 0) {
         Log::error("mutex %s: lock : errno=%d %s",
                    id, ret, strerror(ret));
      }

//      if (strstr(IGNORELIST, id) == 0) Log::debug(" ... locked (%s)", id);
   }

   void Mutex::unlock() {
      int ret;
//      if (strstr(IGNORELIST, id) == 0) Log::debug("Mutex::unlock(%s)", id);
      ret = pthread_mutex_unlock(&mutex);

      if (ret != 0) {
         Log::error("mutex unlock (%s): errno=%d %s",
                    id, ret, strerror(ret));
      }
   }
}
