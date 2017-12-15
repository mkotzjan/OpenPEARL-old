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

#ifndef PRIOMAPPER_INCLUDED
#define PRIOMAPPER_INCLUDED

/**
\file

\brief priority mapper

this module provides the mapping of PEARL priorities to system priorities
*/


namespace pearlrt {

   /**
   \brief priority mapper

   Mapper object to map PEARL task priorities on the linux priority scheduling
   priorities.
   The range of PEARL priorities is from 1 to 255, where 1 is best and
   255 is lowest priority.
   The priority range in Linux is from 1 to 99 using the SCHED_RR scheduler.
   The best Linux priority is reserved for the operating system
   wrapper (os.cc).

   The priorities are mapped linear from PEARL(=1,2,3,...,255)  to
   SCHED_RR (=98,97,96,..,1.

   The PEARL priorities which are not mappable (97..254) produce a PEARL
   signal.

   PEARL Priority 255  is mapped to SCHED_RR priority = 1.

   The mechanism is realized as a singleton.
   */
   class PrioMapper {
   private:
      PrioMapper();
      int min; 			///< min system prio
      int max; 			///< max system prio
      static PrioMapper* instance; 	///< the one and only instance

   public:
      /**
         get access to the mapper (Singleton pattern)
         \returns pointer to the (one and only) Prio-Object
      */
      static PrioMapper* getInstance();

      /**
       transform PEARL priority to system priority

       \param p the PEARL priority running from 1 (best) to 255 (least)

       \returns the mapped system priority
       \throws PriorityNotMapableSignal if the requested priority is not
               available on the target system
      */
      int fromPearl(const Fixed<15> p);

      /**
       return the best priority - just better than all PEARL tasks
       \returns the system priority
      */
      int getSystemPrio();

      /** show the remaining PEARL priorities on the log as INFO
      */
      void logPriorities();
   };
}

#endif


