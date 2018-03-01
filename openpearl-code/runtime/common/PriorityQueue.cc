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

#include "PriorityQueue.h"
#include "TaskCommon.h"
#include "compare.h"

namespace pearlrt {

   PriorityQueue::PriorityQueue() {
      head = 0;
   }

   void PriorityQueue::insert(TaskCommon * x) {
      TaskCommon *n;
      Fixed<15> prio = x->getPrio();

      if (head == 0 || (head->getPrio() > prio).getBoolean()) {
         x->setNext(head);
         head = x;
      } else {
         for (TaskCommon* i = head; i != 0; i = i->getNext()) {
            n = i->getNext();

            if ((n == 0) ||
                  (n != 0 && (i->getPrio() <= prio).getBoolean()
                   && (prio < n->getPrio()).getBoolean())) {
               x->setNext(n);
               i->setNext(x);
               break; // abort loop
            }
         }
      }
   }

   void PriorityQueue::remove(TaskCommon * x) {
      if (head == x) {
         // remove first element
         head = x->getNext();
         x->setNext(0);
      } else {
         for (TaskCommon * i = head; i != 0; i = i->getNext()) {
            if (i->getNext() == x) {
               i->setNext(x->getNext());
               x->setNext(0);
               break;   // abort loop
            }
         }
      }
   }


   TaskCommon* PriorityQueue::getNext(TaskCommon * x) {
      return x->getNext();
   }

   TaskCommon* PriorityQueue::getHead() {
      return (head);
   }
}

