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

/**
\file

\brief BOLT implementation for posix threads using simultaneous
locking pattern

*/

#define __STDC_LIMIT_MACROS   // enable UINT32_MAX-macro
// must be set before stdint.h
#include <stdint.h>

#include "TaskCommon.h"
#include "Bolt.h"
#include "Signals.h"
#include "Log.h"

namespace pearlrt {

   PriorityQueue Bolt::waiters;

   Bolt::Bolt(const char * n) {
      name = n;
      Log::debug("Bolt %s created", n);
      nbrOfEnterOperations = 0;
   }

   const char * Bolt::getName(void) {
      return name;
   }

   int Bolt::getState(void) {
      return state;
   }

   char* Bolt::getStateName(void) {
      switch (state) {
         default:       return((char*)"???");
         case FREE:     return((char*)"free");
         case ENTERED:  return((char*)"entered");
         case RESERVED: return((char*)"reserved");
      }
   }


   void Bolt::setState(int newState) {
      state = newState;
   }

   int Bolt::check(BlockReason r, BlockData::BlockReasons::BlockBolt *bd) {
      int wouldBlock = 0;
      int i;

      for (i = 0; i < bd->nbolts; i++) {
         if (bd->bolts[i]->getState() == RESERVED || 
             (bd->bolts[i]->getState() == ENTERED && r == RESERVE)) {
            wouldBlock = 1;
         }

         Log::debug("   check::bolt: %s is %d nbrOfEnterOperations=%d",
                    bd->bolts[i]->getName(), (int)bd->bolts[i]->getState(),
                    (int)bd->bolts[i]->getNbrOfEnterOperations());
      }

      return wouldBlock;
   }

   void Bolt::enter(TaskCommon* me,
                    int nbrOfBolts,
                    Bolt** bolts) {
        enterOrReserve(me, ENTER, ENTERED, nbrOfBolts, bolts);
   }

   void Bolt::reserve(TaskCommon* me,
                    int nbrOfBolts,
                    Bolt** bolts) {
        enterOrReserve(me, RESERVE , RESERVED, nbrOfBolts, bolts);
   }

   uint32_t  Bolt::getNbrOfEnterOperations() {
       return nbrOfEnterOperations;
   }

   void Bolt::decrementEnter() {
       nbrOfEnterOperations --;
   }

   void Bolt::incrementEnter() {
     if (nbrOfEnterOperations == UINT32_MAX) {
        // this may happen if more 2^32 tasks are waiting -- not
        // propable in the near future !
        Log::error("too many ENTER operations on bolt %s", name);
        TaskCommon::mutexUnlock();
        throw theInternalTaskSignal;
     }
     nbrOfEnterOperations ++;
   }

   void Bolt::enterOrReserve(TaskCommon* me,
                    BlockReason operation,
                    int newState,
                    int nbrOfBolts,
                    Bolt** bolts) {
      int i;
      int wouldBlock = 0;
      BlockData bd;

      bd.reason = operation;
      bd.u.bolt.nbolts = nbrOfBolts;
      bd.u.bolt.bolts = bolts;

      TaskCommon::mutexLock();
      if (operation == ENTER) {
         Log::info("ENTER from task %s for %d bolts", me->getName(),
                nbrOfBolts);
      } else {
         Log::info("RESERVE from task %s for %d bolts", me->getName(),
                nbrOfBolts);
      }
      me->scheduleCallback(true);
      wouldBlock = check(operation, &(bd.u.bolt));

      if (! wouldBlock) {
         for (i = 0; i < nbrOfBolts; i++) {
            bolts[i]->setState(newState);
            if (operation == ENTER) {
               bolts[i]->incrementEnter();
            }
         }

         // critical region end
         TaskCommon::mutexUnlock();
      } else {
         Log::info("   task: %s going to blocked", me->getName());
         waiters.insert(me);
         // critival region ends in block()
         me->block(&bd);
      }
   }

   void Bolt::leave(TaskCommon* me,
                    int nbrOfBolts,
                    Bolt** bolts) {
       leaveOrFree(me, ENTERED, nbrOfBolts, bolts);
   }

   void Bolt::free(TaskCommon* me,
                    int nbrOfBolts,
                    Bolt** bolts) {
       leaveOrFree(me, RESERVED, nbrOfBolts, bolts);
   }

   void Bolt::leaveOrFree(TaskCommon* me,
                    int oldState,
                    int nbrOfBolts,
                    Bolt** bolts) {
      BlockData bd;
      int i;
      int wouldBlock;
      int reserveIsWaiting;
      TaskCommon * t;

      // start critical region - end after done all possible unblocking
      TaskCommon::mutexLock();
      if (oldState == ENTERED) {
         Log::debug("LEAVE from task %s for %d bolts", me->getName(),
                 nbrOfBolts);
      } else {
         Log::debug("FREE from task %s for %d bolts", me->getName(),
                 nbrOfBolts);
      }

      for (i = 0; i < nbrOfBolts; i++) {
         if (bolts[i]->getState() != oldState) {
            Log::error("   bolt: %s has wrong state (%d)",
                   bolts[i]->getName(),
                   bolts[i]->getState());
            TaskCommon::mutexUnlock();
            throw theBoltStateSignal;
         }
         if (bolts[i]->getState() == ENTERED) {
            bolts[i]->decrementEnter();
            if (bolts[i]->getNbrOfEnterOperations() == 0) {
               bolts[i]->setState(FREE);
               Log::debug("   bolt: %s is now free",
                    bolts[i]->getName());
            }
         } else {
            // RESERVED
            bolts[i]->setState(FREE);
            Log::debug("   bolt: %s is now free",
                    bolts[i]->getName());
         }
      }

      reserveIsWaiting = 0;
      // pass 1 - test for task waiting for RESERVE
      for (t = waiters.getHead(); t != 0; t = waiters.getNext(t)) {
         t->getBlockingRequest(&bd);
         wouldBlock = check(bd.reason, &(bd.u.bolt));

         if (bd.reason == RESERVE)  {
            if ( !wouldBlock) {
               for (i = 0; i < bd.u.bolt.nbolts; i++) {
                  bd.u.bolt.bolts[i]->setState(RESERVED);
               }
               waiters.remove(t);
               t->unblock();
               Log::info("   unblocking: %s", t->getName());
            } else {
              reserveIsWaiting = 1;
            }
         }
      }

      // perform pass 2 only if no task waits with RESERVE
      if (!reserveIsWaiting) {
         for (t = waiters.getHead(); t != 0; t = waiters.getNext(t)) {
            t->getBlockingRequest(&bd);
            wouldBlock = check(bd.reason, &(bd.u.bolt));

            if (bd.reason == ENTER)  {
               if ( !wouldBlock) {
                  for (i = 0; i < bd.u.bolt.nbolts; i++) {
                     bd.u.bolt.bolts[i]->setState(ENTERED);
                     bd.u.bolt.bolts[i]->incrementEnter();
                  }

                  waiters.remove(t);
                  t->unblock();
                  Log::info("   unblocking: %s", t->getName());
               }
            }
         }
      }

      TaskCommon::mutexUnlock();
   }


   void Bolt::removeFromWaitQueue(TaskCommon * t) {
      waiters.remove(t);
   }

   void Bolt::addToWaitQueue(TaskCommon * t) {
      BlockData bd;
      int wouldBlock;

      t->getBlockingRequest(&bd);
      wouldBlock = check(bd.reason, &(bd.u.bolt));

      if (!wouldBlock)  {
         for (int i = 0; i < bd.u.bolt.nbolts; i++) {
            bd.u.bolt.bolts[i]->setState(bd.reason);
         }

         waiters.remove(t);
         t->unblock();
         Log::debug("   unblocking: %s", t->getName());
      }  else {
         waiters.insert(t);
      }

   }

   void Bolt::updateWaitQueue(TaskCommon * t) {
      waiters.remove(t);
      waiters.insert(t);
   }
}
