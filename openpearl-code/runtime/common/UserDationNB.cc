/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
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

#include "UserDationNB.h"
#include "Signals.h"
#include "Log.h"
#include "Dation.h"
#include "DationDim1.h"
#include "DationDim2.h"
#include "DationDim3.h"

/**
\file

\brief implementation of UserDation interface

*/

namespace pearlrt {

   const Fixed<31> UserDationNB::one = (Fixed<31>)1;

   UserDationNB::UserDationNB(SystemDationNB * parent,
                          int & params,
                          DationDim * dimensions,
                          DationType dt): dationType(dt) {
      system = parent;

      if (dimensions == NULL) {
         Log::error("UserDationNB: no dimension passed");
         throw theIllegalParamSignal;
      }

      dim = dimensions;

      // verify needed parameters
      if (!!(params & IN) +
            !!(params & INOUT) +
            !!(params & OUT) != 1) {
         Log::error("UserDationNB: no or more than 1 direction specified");
         throw theIllegalParamSignal;
      }

      if (!!(params & FORWARD) +
            !!(params & FORBACK) +
            !!(params & DIRECT) != 1) {
         Log::error(
            "UserDationNB: no or more than 1 positioning attribute specified");
         throw theIllegalParamSignal;
      }

      if (!((params & STREAM) | (params & NOSTREAM))) {
         params |= STREAM;  // set default
      } else if (!((params & STREAM) ^ (params & NOSTREAM))) {
         Log::error("UserDationNB: ether STREAM or NOSTREAM required");
         throw theIllegalParamSignal;
      }

      if (!((params & CYCLIC) | (params & NOCYCL))) {
         params |= NOCYCL;  // set default
      } else if (!((params & CYCLIC) ^ (params & NOCYCL))) {
         Log::error("UserDationNB: ether CYCLIC or NOCYCL required");
         throw theIllegalParamSignal;
      }

      if (params & CYCLIC && !dim->isBounded()) {
         Log::error("UserDationNB: CYCLIC requires bounded dimension");
         throw theIllegalParamSignal;
      }

      if (params & CYCLIC && !(params & DIRECT)) {
         Log::error("UserDationNB: CYCLIC requires DIRECT");
         throw theIllegalParamSignal;
      }

      // verify compatibility of system device capabilities with dation
      // parameters
      if ((system->capabilities() & DIRECTIONMASK & params) !=
            (params & DIRECTIONMASK)) {
         Log::error(
            "UserDationNB: required direction not supported by system dation");
         throw theIllegalParamSignal;
      }

      if ((system->capabilities() & POSITIONINGMASK & params) !=
            (params & POSITIONINGMASK)) {
         Log::error(
            "UserDationNB: required positioning are "
            "not supported by system dation");
         throw theIllegalParamSignal;
      }

      dationParams = params;
      dim->setDationParams(dationParams);
   }

   void UserDationNB::closeSystemDation(int dationParams) {
       work-> dationClose(dationParams);
   }

   void UserDationNB::pos(Fixed<31> element) {
      assertOpenDirect();
      // adjust value range to 0...
      element = element - one;

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: POS needs DIM");
         throw theNotAllowedSignal;
         break;

      case 1:
         ;
         {
            DationDim1 * d1 = (DationDim1*) dim;
            d1->pos(element);
         }
         break;

      case 2:
         ;
         {
            DationDim2 * d2 = (DationDim2*) dim;
            d2->pos(d2->getRow(), element);
         }
         break;

      case 3:
         ;
         {
            DationDim3 * d3 = (DationDim3*) dim;
            d3->pos(d3->getPage(), d3->getRow(), element);
         }
         break;
      }

      /* logical position is set -- now set the file position */
      // curretly we have one dimensional system devices
      Fixed<31> tempPos;
      tempPos = dim->getIndex() * stepSize;
      //work->dationSeek(tempPos, dationParams);
      dationSeek(tempPos, dationParams);
   }

   void UserDationNB::pos(Fixed<31> row, Fixed<31> element) {
      assertOpenDirect();
      // adjust value range to 0...
      row = row - one;
      element = element - one;

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: POS needs DIM");
         throw theNotAllowedSignal;
         break;

      case 1:
         Log::error("UserDationNB: POS(x,y) needs two DIMs");
         throw theNotAllowedSignal;

      case 2: {
         DationDim2 * d2 = (DationDim2*)dim;
         d2->pos(row, element);
      }
      break;

      case 3: {
         DationDim3 * d3 = (DationDim3*)dim;
         d3->pos(d3->getPage(), row, element);
      }
      break;
      }

      /* logical position is set -- now set the file position */
      // curretly we have one dimensional system devices
      Fixed<31> tempPos;
      tempPos = dim->getIndex() * stepSize;
      //work->dationSeek(tempPos, dationParams);
      dationSeek(tempPos, dationParams);
   }

   void UserDationNB::pos(Fixed<31> page,
                        Fixed<31> row,
                        Fixed<31> element) {
      assertOpenDirect();
      // adjust value range to 0...
      element = element - one;
      row = row - one;
      page = page - one;

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: POS needs DIM");
         throw theNotAllowedSignal;
         break;

      case 1:
      case 2:
         Log::error("UserDationNB: POS(x,y,z) needs three DIMs");
         throw theNotAllowedSignal;

      case 3: {
         DationDim3 * d3 = (DationDim3*)dim;
         d3->pos(page, row, element);
      }
      break;
      }

      /* logical position is set -- now set the file position */
      // curretly we have one dimensional system devices
      Fixed<31> tempPos;
      tempPos = dim->getIndex() * stepSize;
      //work->dationSeek(tempPos, dationParams);
      dationSeek(tempPos, dationParams);
   }

   void UserDationNB::sop(Fixed<31>* element) {
      assertOpenDirect();
      *element = dim->getColumn() + one;
   }

   void UserDationNB::sop(Fixed<31> * row, Fixed<31>* element) {
      Fixed<31>  dummy; // for higher dimension
      assertOpenDirect();

      if (dim->getDimensions() < 2) {
         Log::error("UserDationNB: SOP(x,y) needs two DIMs");
         throw theNotAllowedSignal;
      }

      *element = dim->getColumn() + one;
      *row = dim->getRow() + one;
   }

   void UserDationNB::sop(Fixed<31> * page,
                        Fixed<31> * row,
                        Fixed<31>* element) {
      assertOpenDirect();

      if (dim->getDimensions() < 3) {
         Log::error("UserdationNB: SOP needs DIM");
         throw theNotAllowedSignal;
      }

      *element = dim->getColumn() + one;
      *row = dim->getRow() + one;
      *page = dim->getPage() + one;
   }

   Fixed<31> UserDationNB::adv(Fixed<31> element) {
      Fixed<31> oldPos;
      oldPos = dim->getIndex();

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: ADV needs DIM");
         throw theNotAllowedSignal;
         break;

      case 1: {
         DationDim1 * d1 = (DationDim1*)dim;
         d1->adv(element);
      }
      break;

      case 2: {
         DationDim2 * d2 = (DationDim2*)dim;
         d2->adv(0, element);
      }
      break;

      case 3: {
         DationDim3 * d3 = (DationDim3*)dim;
         d3->adv(0, 0, element);
      }
      break;
      }

      /* logical position is set --  */
      Fixed<31> diff;
      diff = dim->getIndex() - oldPos;
      return diff;
   }


   Fixed<31> UserDationNB::adv(Fixed<31> row, Fixed<31> element) {
      Fixed<31> count;
      assertOpen();
      Fixed<31> oldPos;
      oldPos = dim->getIndex();

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: ADV needs DIM");
         throw theNotAllowedSignal;
         break;

      case 1:
         Log::error("UserDationNB: ADV(x,y) needs two DIMs");
         throw theNotAllowedSignal;
         break;

      case 2: {
         DationDim2 * d2 = (DationDim2*)dim;
         d2->adv(row, element);
      }
      break;

      case 3: {
         DationDim3 * d3 = (DationDim3*)dim;
         d3->adv(0, row, element);
      }
      break;
      }

      /* logical position is set  */
      Fixed<31> diff;
      diff = dim->getIndex() - oldPos;
      return diff;
   }

   Fixed<31> UserDationNB::adv(Fixed<31> page,
                             Fixed<31> row,
                             Fixed<31> element) {
      assertOpen();
      Fixed<31> oldPos;
      oldPos = dim->getIndex();

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: ADV needs DIM");
         throw theNotAllowedSignal;
         break;

      case 1:
      case 2:
         Log::error("UserDationNB: ADV(x,y,z) needs three DIMs");
         throw theNotAllowedSignal;

      case 3: {
         DationDim3 * d3 = (DationDim3*)dim;
         d3->adv(page, row, element);
      }
      break;
      }

      /* logical position is set -- now set the file position */
      // curretly we have one dimensional system devices
      Fixed<31> diff;
      diff = dim->getIndex() - oldPos;
      return diff;
   }

   void UserDationNB::toAdv(Fixed<31> element) {
      assertOpen();

      if (dationParams & Dation::DIRECT)  {
         Fixed<31> tempPos;
         adv(element);
         tempPos = dim->getIndex();
         //work->dationSeek(tempPos * stepSize, dationParams);
         dationSeek(tempPos * stepSize, dationParams);
      } else if (dationParams & Dation::FORWARD)  {
         Fixed<31> diff = adv(element);

         if (dationType == ALPHIC) {
            fill(element, ' ');
         } else {
            fill(diff * stepSize, 0);
         }
      }
   }

   void UserDationNB::toAdv(Fixed<31> row, Fixed<31> element) {
      assertOpen();

      if (dationParams & Dation::DIRECT)  {
         adv(row, element);
         //work->dationSeek(dim->getIndex() * stepSize, dationParams);
         dationSeek(dim->getIndex() * stepSize, dationParams);
      } else if (dationParams & Dation::FORWARD)  {
         if (dationType == ALPHIC) {
            Fixed<31> oldCol, oldRow;
            sop(&oldRow, &oldCol);
            adv(row, element);
            row = dim->getRow() - oldRow;
            element  = dim->getColumn() - oldCol;
            fill(row, '\n');
            fill(element, ' ');
         } else {
            Fixed<31> diff;
            diff = adv(row, element);
            fill(diff * stepSize, 0);
         }
      }
   }

   void UserDationNB::toAdv(Fixed<31> page,
                          Fixed<31> row,
                          Fixed<31> element) {
      assertOpen();

      if (dationParams & Dation::DIRECT)  {
         adv(page, row, element);
         //work->dationSeek(dim->getIndex() * stepSize, dationParams);
         dationSeek(dim->getIndex() * stepSize, dationParams);
      } else if (dationParams & Dation::FORWARD)  {
         if (dationType == ALPHIC) {
            Fixed<31> oldCol, oldRow, oldPage;
            sop(&oldPage, &oldRow, &oldCol);
            adv(page, row, element);
            page = dim->getPage() - oldPage;
            row = dim->getRow() - oldRow;
            element  = dim->getColumn() - oldCol;
            fill(page, '\f');
            fill(row, '\n');
            fill(element, ' ');
         } else {
            Fixed<31> diff;
            diff = adv(page, row, element);
            fill(diff * stepSize, 0);
         }
      }
   }

   void UserDationNB::toX(const Fixed<31> n) {
      assertOpenDirectOrForward();
      toAdv(n);
   }

   void UserDationNB::toSkip(const Fixed<31> n) {
      Fixed<31> count;
      assertOpenDirectOrForward();

      if (dim->getDimensions() < 2) {
         Log::error("UserDation: Skip needs >1 dimension");
         throw theInternalDationSignal;
      }

      count = adv(n, -dim->getColumn());

      if (dationParams & Dation::FORWARD) {
         if (dationType == ALPHIC) {
            fill(n, '\n');
         } else {
            fill(count * stepSize, 0);
         }
      }

      if (currentTask) {
         Log::info("UserDation::toSkip: scheduleCallback() invocation\n");
         currentTask->scheduleCallback(false);
      }
   }

   void UserDationNB::toPage(const Fixed<31> n) {
      Fixed<31> count;
      assertOpenDirectOrForward();

      if (dim->getDimensions() < 3) {
         Log::error("DationRW: Page needs >2 dimension");
         throw theInternalDationSignal;
      }

      count = adv(n, -dim->getRow(), -dim->getColumn());

      if (dationParams & Dation::FORWARD) {
         if (dationType == ALPHIC) {
            fill(n, '\f');
         } else {
            fill(count * stepSize, 0);
         }
      }
   }

   void UserDationNB::fromAdv(Fixed<31> element) {
      assertOpen();

      if (dationParams & Dation::DIRECT)  {
         Fixed<31> tempPos;
         adv(element);
         tempPos = dim->getIndex();
         //work->dationSeek(tempPos * stepSize, dationParams);
         dationSeek(tempPos * stepSize, dationParams);
      } else if (dationParams & Dation::FORWARD)  {
         Fixed<31> diff = adv(element);

         if (dationType == ALPHIC) {
            skipAny(element);
         } else {
            skipAny(diff * stepSize);
         }
      }
   }

   void UserDationNB::fromAdv(Fixed<31> row, Fixed<31> element) {
      assertOpen();

      if (dationParams & Dation::DIRECT)  {
         adv(row, element);
         //work->dationSeek(dim->getIndex() * stepSize, dationParams);
         dationSeek(dim->getIndex() * stepSize, dationParams);
      } else if (dationParams & Dation::FORWARD)  {
         if (dationType == ALPHIC) {
            Fixed<31> oldCol, oldRow;
            sop(&oldRow, &oldCol);
            adv(row, element);
            row = dim->getRow() - oldRow;
            element  = dim->getColumn() - oldCol;
            skipUntil(row, '\n');
            skipX(element);
         } else {
            Fixed<31> diff;
            diff = adv(row, element);
            skipAny(diff * stepSize);
         }
      }
   }

   void UserDationNB::fromAdv(Fixed<31> page,
                            Fixed<31> row,
                            Fixed<31> element) {
      assertOpen();

      if (dationParams & Dation::DIRECT)  {
         adv(page, row, element);
         //work->dationSeek(dim->getIndex() * stepSize, dationParams);
         dationSeek(dim->getIndex() * stepSize, dationParams);
      } else if (dationParams & Dation::FORWARD)  {
         if (dationType == ALPHIC) {
            Fixed<31> oldCol, oldRow, oldPage;
            sop(&oldPage, &oldRow, &oldCol);
            adv(page, row, element);
            page = dim->getPage() - oldPage;
            row = dim->getRow() - oldRow;
            element  = dim->getColumn() - oldCol;
            skipUntil(page, '\f');
            skipUntil(row, '\n');
            skipX(element);
         } else {
            Fixed<31> diff;
            diff = adv(page, row, element);
            skipAny(diff * stepSize);
         }
      }
   }

   void UserDationNB::fromX(const Fixed<31> n) {
      assertOpenDirectOrForward();
      fromAdv(n);
   }

   void UserDationNB::fromSkip(const Fixed<31> n) {
      Fixed<31> count;
      assertOpenDirectOrForward();

      if (dim->getDimensions() < 2) {
         Log::error("UserDation: Skip needs >1 dimension");
         throw theInternalDationSignal;
      }

      count = adv(n, -dim->getColumn());

      if (dationParams & Dation::FORWARD) {
         if (dationType == ALPHIC) {
            skipUntil(n, '\n');
         } else {
            skipAny(count * stepSize);
         }
      }

      if (currentTask) {
         Log::info("UserDation::fromSkip: scheduleCallback() invocation\n");
         currentTask->scheduleCallback(false);
      }
   }

   void UserDationNB::fromPage(const Fixed<31> n) {
      Fixed<31> count;
      assertOpenDirectOrForward();

      if (dim->getDimensions() < 3) {
         Log::error("DationRW: Page needs >2 dimension");
         throw theInternalDationSignal;
      }

      count = adv(n, -dim->getRow(), -dim->getColumn());

      if (dationParams & Dation::FORWARD) {
         if (dationType == ALPHIC) {
            skipUntil(n, '\f');
         } else {
            skipAny(count * stepSize);
         }
      }
   }

#ifdef MOVED
   void UserDation::dationOpen(int p, Fixed<31>* rst) {
      // Character<1> dummy;
      dationOpen(p, (Character<1>*)NULL, rst);
   }
#endif
   void UserDationNB::internalDationOpen(int p,
                                       RefCharacter * rc) {
      // enshure default open parameter
      if ((p & (ANY | OLD | NEW)) == 0) {
         p |= ANY;
      }

      // enshure default close parameter
      if ((p & CLOSEMASK) == 0) {
         p |= PRM;
      }

      if (dationStatus == OPENED) {
         Log::error((char*)"Dation is already open");
         throw theNotAllowedSignal;
      }

      if ((system->capabilities() & IDF) && !(p & IDF) && !(p & ANY)) {
         Log::error("UserDation: system dation requires IDF");
         Log::error("!(p&IDF) %d p&ANY) %d ", !(p & IDF), p & ANY);
         throw theIllegalParamSignal;
      }

      if (!(system->capabilities() & IDF) && (p & IDF)) {
         Log::error("UserDation: system dation does not support IDF");
         throw theIllegalParamSignal;
      }

      if ((system->capabilities() & OPENMASK & p) !=
            (p & OPENMASK)) {
         Log::error("UserDation: open parameter not supported "
                    "by system device");
         throw theIllegalParamSignal;
      }

      if (!(p & IDF) && (!!(p & NEW) + !!(p & OLD)) > 0) {
         Log::error("UserDation: OLD/NEW requires IDF");
         throw theIllegalParamSignal;
      }

      if ((!!(p & CAN) + !!(p & PRM)) > 1) {
         Log::error("UserDation: ether CAN or PRM allowed");
         throw theIllegalParamSignal;
      }

      if (p & OPENMASK) {
         // save params in dationparams
         dationParams &= ~ OPENMASK;
         dationParams |= (p & OPENMASK);
      }

      // open system dation
      if (p & IDF) {
         // pass filename if specified by IDF
         work = system->dationOpen(rc->getCstring(), dationParams);
      } else {
         // no filename specified by IDF --> pass NULL as name
         work = system->dationOpen(NULL, dationParams);
      }

      // do dation (RW/PG) specific stuff
      internalOpen();
      dationStatus = OPENED;
      dim->reset();
      return;
   }

#ifdef MOVED
   void UserDation::dationClose(const int p, Fixed<31>* rst) {
      Fixed<31>* intRst = NULL;

      try {
         if (p & RST) {
            if (! rst) {
               Log::error("UserDation: RST is set but no variable given");
               throw theIllegalParamSignal;
            }

            intRst  = rst;
         }

         assertOpen();

         if (p & CLOSEMASK) {
            if (!!(p & PRM) && !!(p & CAN)) {
               Log::error("UserDation: ether CAN or PRM allowed");
               throw theIllegalParamSignal;
            }

            // superseed previous settings
            dationParams &= ~CLOSEMASK;
            dationParams |= p;
         }

         work -> dationClose(dationParams);
         dationStatus = CLOSED;
      } catch (Signal &  s) {
         if (intRst != NULL) {
            *intRst = (Fixed<31>)s.whichRST();
         } else {
            throw;
         }
      }
   }
#endif

   /* does nothing, if n <= 0
      write fillChar n times to the output
   */
   void UserDationNB::fill(const Fixed<31> n, char fillChar) {
      Fixed<31> h = n;

      while (h.x > 0) {
         //work->dationWrite(&fillChar, 1);
         dationWrite(&fillChar, 1);
         h.x --;
      }
   }

   /* does nothing, if n <= 0
      read input until n bytes are read
   */
   void UserDationNB::skipAny(const Fixed<31> n) {
      Fixed<31> h = n;
      char dummy;

      while (h.x > 0) {
         //work->dationRead(&dummy, 1);
         dationRead(&dummy, 1);
         h.x --;
      }
   }

   /* does nothing if n <= 0
      discards  n input characters
      does not skip over '\n' or '\f'
   */
   void UserDationNB::skipX(const Fixed<31> n) {
      Fixed<31> h = n;
      char dummy;

      while (h.x > 0) {
         //work->dationRead(&dummy, 1);
         dationRead(&dummy, 1);
         h.x --;

         if (dummy == '\n' || dummy == '\f') {
            //work->dationUnGetChar(dummy);
            dationUnGetChar(dummy);
            return;
         }
      }
   }

   /* does nothing if n <= 0
      discards input until targetChar is read n times
      does not skip higher level fill characters than the expected

     '\n' does not skip \f
   */
   void UserDationNB::skipUntil(const Fixed<31> n, char targetChar) {
      Fixed<31> h = n;
      char dummy;

      while (h.x > 0) {
         //work->dationRead(&dummy, 1);
         dationRead(&dummy, 1);

         if (dummy == targetChar) {
            h.x --;
         } else if (dummy == '\f' && targetChar == '\n') {
            //work->dationUnGetChar(dummy);
            dationUnGetChar(dummy);
            return;
         }
      }
   }


#ifdef MOVED

   void UserDation::assertOpen() {
      if (dationStatus == CLOSED) {
         Log::error("dation open required");
         throw theNotAllowedSignal;
      }
   }
#endif

   void UserDationNB::assertOpenDirect() {
      assertOpen();

      if ((dationParams & DIRECT) == 0) {
         Log::error("DIRECT dation required");
         throw thePositioningForbiddenSignal;
      }
   }
   void UserDationNB::assertOpenDirectOrForward() {
      assertOpen();

      if ((dationParams & (DIRECT | FORWARD)) == 0) {
         Log::error("DIRECT or FORWARD dation required");
         throw theDationNotSupportedSignal;
      }
   }
}
