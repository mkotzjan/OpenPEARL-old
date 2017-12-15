/*
 [A "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014-2017 Rainer Mueller
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
#include "compare.h"
#include "Task.h"
/**
\file

\brief implementation of UserDation interface

*/

namespace pearlrt {

   const Fixed<31> UserDationNB::one = (Fixed<31>)1;
   const Fixed<31> UserDationNB::zero = (Fixed<31>)0;

   UserDationNB::UserDationNB(SystemDationNB * parent,
                              int & params,
                              DationDim * dimensions,
                              DationType dt)
      : dationType(dt) {
      system = parent;
      systemDation = parent;  // set this reference also for SystemDation

      if (dimensions == NULL) {
         Log::error("UserDationNB: no dimension passed");
         throw theInternalDationSignal;
      }

      dim = dimensions;

      // verify needed parameters
      if (!!(params & IN) +
            !!(params & INOUT) +
            !!(params & OUT) != 1) {
         Log::error("UserDationNB: no or more than 1 direction specified");
         throw theInternalDationSignal;
      }

      if (!!(params & FORWARD) +
            !!(params & FORBACK) +
            !!(params & DIRECT) != 1) {
         Log::error(
            "UserDationNB: no or more than 1 positioning attribute specified");
         throw theInternalDationSignal;
      }

      if (!((params & STREAM) | (params & NOSTREAM))) {
         params |= STREAM;  // set default
      } else if (!((params & STREAM) ^ (params & NOSTREAM))) {
         Log::error("UserDationNB: ether STREAM or NOSTREAM required");
         throw theInternalDationSignal;
      }

      if (!((params & CYCLIC) | (params & NOCYCL))) {
         params |= NOCYCL;  // set default
      } else if (!((params & CYCLIC) ^ (params & NOCYCL))) {
         Log::error("UserDationNB: ether CYCLIC or NOCYCL required");
         throw theInternalDationSignal;
      }

      if (params & CYCLIC && !dim->isBounded()) {
         Log::error("UserDationNB: CYCLIC requires bounded dimension");
         throw theInternalDationSignal;
      }

      if (params & CYCLIC && !(params & DIRECT)) {
         Log::error("UserDationNB: CYCLIC requires DIRECT");
         throw theInternalDationSignal;
      }

      // verify compatibility of system device capabilities with dation
      // parameters
      if ((system->capabilities() & DIRECTIONMASK & params) !=
            (params & DIRECTIONMASK)) {
         Log::error(
            "UserDationNB: required direction not supported by system dation");
         throw theInternalDationSignal;
      }

      if ((system->capabilities() & POSITIONINGMASK & params) !=
            (params & POSITIONINGMASK)) {
         Log::error(
            "UserDationNB: required positioning are "
            "not supported by system dation");
         throw theInternalDationSignal;
      }

      dationParams = params;
      dim->setDationParams(dationParams);

      // perform NewLine translation only for ALPHIC dations
      system->translateNewLine(dationType == ALPHIC);

      if (dationType == ALPHIC) {
         paddingElement = ' ';
      } else {
         paddingElement = 0;
      }
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
         throw theInternalDationSignal;
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

      doTfuAndSeekStuff();
   }

   void UserDationNB::pos(Fixed<31> row, Fixed<31> element) {
//printf("pos(%d, %d) dp=%x\n", row.x, element.x,dationParams);
      assertOpenDirect();
      // adjust value range to 0...
      row = row - one;
      element = element - one;

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: POS needs DIM");
         throw theInternalDationSignal;
         break;

      case 1:
         Log::error("UserDationNB: POS(x,y) needs two DIMs");
         throw theInternalDationSignal;

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

      doTfuAndSeekStuff();

   }

   void UserDationNB::pos(Fixed<31> page,
                          Fixed<31> row,
                          Fixed<31> element) {
      assertOpenDirect();
      // adjust value range to 0...
//printf("pos %d/%d/%d\n", page.x, row.x, element.x);
      element = element - one;
      row = row - one;
      page = page - one;

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: POS needs DIM");
         throw theInternalDationSignal;
         break;

      case 1:
      case 2:
         Log::error("UserDationNB: POS(x,y,z) needs three DIMs");
         throw theInternalDationSignal;

      case 3: {
         DationDim3 * d3 = (DationDim3*)dim;
         d3->pos(page, row, element);
      }
      break;
      }

      doTfuAndSeekStuff();
   }

   void UserDationNB::doTfuAndSeekStuff()  {
//printf("doTfuAndSeekStuff\n");
//printf("tfuBuffer used/notEmpty/dir  %d / %d / %d \n", tfuBuffer.isUsed(),
//       tfuBuffer.isNotEmpty(),currentDirection);
      /* logical position is set -- now set the file position */
      // we have one dimensional system devices
      if (tfuBuffer.isUsed()) {
         // flush and reinitialize TFU buffer for DIRECT and FORWARD
         // dations, if the new position is outside of the current record
         if (tfuBuffer.isNotEmpty() && currentDirection == Dation::OUT) {
            tfuBuffer.flushRecord();
            tfuBuffer.prepare();
         }

         // with TFU we must place the physical file pointer to the
         // beginning of the record
         Fixed<31> tempPos;
         tempPos = (dim->getIndex() - dim->getColumn()) * stepSize;
//printf("getIndex=%d  col=%d stepSize=%d\n",
//    dim->getIndex().x,dim->getColumn().x,stepSize.x);
//printf("tempPos=%d\n", tempPos.x);
         work->dationSeek(tempPos, dationParams);
//         if (currentDirection == Dation::IN) {
//            tfuBuffer.prepare();
//            tfuBuffer.readRecord(dationType==ALPHIC);
//         }

         tfuBuffer.setPosition((dim->getColumn()*stepSize).x);
//printf("tfuBuffer.pos=%d\n", (dim->getColumn()*stepSize).x);

      } else {
         Fixed<31> tempPos;
         tempPos = dim->getIndex() * stepSize;
         work->dationSeek(tempPos, dationParams);
      }
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
         throw theInternalDationSignal;
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
         throw theInternalDationSignal;
      }

      *element = dim->getColumn() + one;
      *row = dim->getRow() + one;
      *page = dim->getPage() + one;
   }

   void UserDationNB::internalSop(Fixed<31> * page,
                                  Fixed<31> * row,
                                  Fixed<31>* element) {
      *element = dim->getColumn() + one;
      *row = dim->getRow() + one;
      *page = dim->getPage() + one;
//      printf("internalSop-> %d/%d/%d\n", page->x, row->x, element->x);
   }

   Fixed<31> UserDationNB::adv(Fixed<31> element) {
      Fixed<31> oldPos;
      oldPos = dim->getIndex();
//printf("userdationNB:adv(%d)\n", element.x);

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: ADV needs DIM");
         throw theInternalDationSignal;
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
         throw theInternalDationSignal;
         break;

      case 1:
         Log::error("UserDationNB: ADV(x,y) needs two DIMs");
         throw theInternalDationSignal;
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
         throw theInternalDationSignal;
         break;

      case 1:
      case 2:
         Log::error("UserDationNB: ADV(x,y,z) needs three DIMs");
         throw theInternalDationSignal;

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

   Fixed<31> UserDationNB::internalAdv(Fixed<31> page,
                                       Fixed<31> row,
                                       Fixed<31> element) {
      assertOpen();
      Fixed<31> oldPos;
      oldPos = dim->getIndex();

      switch (dim->getDimensions()) {
      case 0:
         Log::error("UserDationNB: ADV needs DIM");
         throw theInternalDationSignal;
         break;

      case 1: {
         DationDim1 * d1 = (DationDim1*)dim;
         d1->adv(element);
         break;
      }

      case 2: {
         DationDim2 * d2 = (DationDim2*)dim;
         d2->adv(row, element);
         break;
      }

      case 3: {
         DationDim3 * d3 = (DationDim3*)dim;
         d3->adv(page, row, element);
         break;
      }
      }

      /* logical position is set -- now set the file position */
      // currently we have one dimensional system devices
      Fixed<31> diff;
      diff = dim->getIndex() - oldPos;
      return diff;
   }

   void UserDationNB::toAdv(Fixed<31> element) {
      toAdv(zero, zero, element);
   }

   void UserDationNB::toAdv(Fixed<31> row, Fixed<31> element) {
      toAdv(zero, row, element);
   }

   void UserDationNB::toAdv(Fixed<31> page,
                            Fixed<31> row,
                            Fixed<31> element) {
      Fixed<31> oldCol, oldRow, oldPage;
      Fixed<31> diff;

      assertOpen();

//printf("\n\ntoAdv: p=%d, r=%d e=%d\n", page.x, row.x, element.x);
      // do virtual positioning first
      internalSop(&oldPage, &oldRow, &oldCol);
//printf("  old: p=%d, r=%d e=%d\n", oldPage.x, oldRow.x, oldCol.x);
      diff = internalAdv(page, row, element);


      if (tfuBuffer.isUsed()) {
         //calculate new position
         // add 1, since dim-> counts with starting 0,
         //         and UserDationNB starts with 1
         page = dim->getPage() - oldPage + one;
         row = dim->getRow() - oldRow + one;
         element  = dim->getColumn() - oldCol + one;

         // now: page,row,element contains the difference to the
         // previous position
//printf("diff: p=%d, r=%d e=%d\n", page.x, row.x, element.x);
//printf("  new: p=%d, r=%d e=%d\n",
//         dim->getPage().x+1, dim->getRow().x+1, dim->getColumn().x+1);

         // flush and reinitialize TFU buffer for DIRECT and FORWARD
         // dations, if the new position is outside of the current record
         if (tfuBuffer.isNotEmpty() &&
               (row.x != 0 || page.x != 0)) {
            tfuBuffer.flushRecord();
            tfuBuffer.prepare();

            if (dationParams & Dation::FORWARD && dationType == ALPHIC) {
               fill(1, '\n');
            }
         }

         if ((dationParams & Dation::DIRECT) &&
               (row.x != 0 || page.x != 0)) {
            work->dationSeek(dim->getIndex() * stepSize, dationParams);
            // update position on physical device
            // with TFU we must set the location to the beginning
            // of the adressed record and the position in the record
            // in the TFU object
//printf("userdation @ 1\n");
            sop(&oldPage, &oldRow, &oldCol);
            pos(oldPage, oldRow, one);
            tfuBuffer.setPosition(dim->getColumn().x);
         }

         // now treat the lines and pages for forward dation
         if (dationParams & Dation::FORWARD) {
            // calculate number of complete rows to be written
//printf("toAdv: p/r/c: %d/%d/%d", page.x, row.x, element.x);
            row = row + dim->getRows() * page;

//printf("  --> rows %d\n", row.x);
            while ((row >= one).getBoolean()) {
               if (currentDirection == Dation::OUT) {
                  tfuBuffer.flushRecord();

                  if (dationParams & Dation::FORWARD && dationType == ALPHIC) {
                     fill(1, '\n');
                  }
               } else {
                  tfuBuffer.readRecord(dationType == ALPHIC);
               }

               row = row - one;
            }

//printf("  --> col %d\n", dim->getColumn().x);
            tfuBuffer.setPosition(dim->getColumn().x);
         }
      } else {
         // no TFU buffer active
         if (dationParams & Dation::DIRECT)  {
            work->dationSeek(dim->getIndex() * stepSize, dationParams);
         }

         if (dationParams & Dation::FORWARD)  {
            if (dationType == ALPHIC) {
//printf("toAdv: %d/%d/%d fillers\n", page.x, row.x, element.x);
               row = row + page * dim->getRows();
//printf("   --> %d/%d fillers\n", row.x, element.x);
               //     fill(page, '\f');
               fill(row, '\n');
               fill(element, ' ');
            } else {
               fill(diff * stepSize, 0);
            }
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
         Log::error("UserDationNB: SKIP needs >1 dimension");
         throw theInternalDationSignal;
      }

      toAdv(zero, n, -dim->getColumn());

      if (currentTask) {
//         Log::info("UserDationNB::toSkip: scheduleCallback() invocation\n");
         currentTask->scheduleCallback(false);
      }
   }

   void UserDationNB::toPage(const Fixed<31> n) {
      Fixed<31> count;
      assertOpenDirectOrForward();

      if (dim->getDimensions() < 3) {
         Log::error("UserDationNB: Page needs >2 dimension");
         throw theInternalDationSignal;
      }

      toAdv(n, -dim->getRow(), -dim->getColumn());
   }

   void UserDationNB::fromAdv(Fixed<31> element) {
      fromAdv(zero, zero, element);
   }

   void UserDationNB::fromAdv(Fixed<31> row, Fixed<31> element) {
      fromAdv(zero, row, element);
   }

   void UserDationNB::fromAdv(Fixed<31> page,
                              Fixed<31> row,
                              Fixed<31> element) {
      Fixed<31> oldPage, oldRow, oldCol;

      // number of byte to skip for forward non ALPHIC dations
      Fixed<31> diff;

      assertOpen();

//printf("\n\nfromAdv: p=%d, r=%d c=%d\n", page.x, row.x, element.x);
      // do virtual positioning first
      internalSop(&oldPage, &oldRow, &oldCol);
//printf("  old: p=%d, r=%d c=%d\n", oldPage.x, oldRow.x, oldCol.x);
      diff = internalAdv(page, row, element);
      // add 1, since dim-> counts with starting 0,
      //         and UserDationNB starts with 1
      page = dim->getPage() - oldPage + one;
      row = dim->getRow() - oldRow + one;
      element  = dim->getColumn() - oldCol + one;
//printf("  new: p=%d, r=%d c=%d\n",
//      dim->getPage().x+1, dim->getRow().x+1, dim->getColumn().x+1);

      if (tfuBuffer.isUsed()) {
         if (dationParams & Dation::DIRECT) {
            work->dationSeek(dim->getIndex() * stepSize, dationParams);
            // update position on physical device
            // with TFU we must set the location to the beginning
            // of the adressed record and the position in the record
            // in the TFU object
            sop(&oldPage, &oldRow, &oldCol);
            pos(oldPage, oldRow, one);
            tfuBuffer.setPosition(dim->getColumn().x);
         }

         // now treat the lines and pages for forward dation
         if (dationParams & Dation::FORWARD) {
            // calculate number of complete rows to be read
//printf("fromAdv: p/r/c: %d/%d/%d", page.x, row.x, element.x);
            row = row + dim->getRows() * page;

            if (! tfuBuffer.isNotEmpty()) {
               // if the current positions record ist not read
               // we must consider to read one record more
               row = row + one;

            }

//printf("  --> rows %d\n", row.x);
            while ((row > zero).getBoolean()) {
               tfuBuffer.readRecord(dationType == ALPHIC);
               row = row - one;
            }

//printf("  --> col %d\n", dim->getColumn().x);
            tfuBuffer.setPosition((dim->getColumn()*stepSize).x);
         }
      } else {
         // no TFU buffer active
         if (dationParams & Dation::DIRECT)  {
            adv(page, row, element);
            work->dationSeek(dim->getIndex() * stepSize, dationParams);
         } else if (dationParams & Dation::FORWARD)  {
            if (dationType == ALPHIC) {
//printf("fromAdv ALPHIC: p/r/c: %d/%d/%d\n", page.x, row.x, element.x);
//               Fixed<31> oldCol, oldRow, oldPage;
//               internalSop(&oldPage, &oldRow, &oldCol);
//               internalAdv(page, row, element);

//               page = dim->getPage() - oldPage + one;
//               row = dim->getRow() - oldRow + one;
//               element  = dim->getColumn() - oldCol + one;
//printf("fromADV ALPHIC delta: %d:%d:%d \n", page.x, row.x, element.x);
//               skipUntil(page, '\f');

               // pages are mapped to a fixed number of lines
               row = row + dim->getRows() * page;
//printf("fromADV ALPHIC delta: %d:%d:%d \n", page.x, row.x, element.x);

               skipUntil(row, '\n');

               // skip characters in line, if target position
               // is NOT beginning of line
               if ((element > zero).getBoolean()) {
                  skipX(element);
               }
            } else {
//printf("skip any %d bytes\n", diff.x);
               skipAny(diff * stepSize);
            }
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
         Log::error("UserDationNB: Skip needs >1 dimension");
         throw theInternalDationSignal;
      }

      fromAdv(zero, n, -dim->getColumn());

      if (currentTask) {
         currentTask->scheduleCallback(false);
      }
   }

   void UserDationNB::fromPage(const Fixed<31> n) {
      Fixed<31> count;
      assertOpenDirectOrForward();

      if (dim->getDimensions() < 3) {
         Log::error("UserDationNB: Page needs >2 dimension");
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

   void UserDationNB::eof() {
      Fixed<31> endOfFilePos;
      int dimensions;
      Fixed<31> p, r, c;

      assertOpenDirectOrForward();
      endOfFilePos = work->dationEof();

      if (dationParams & Dation::DIRECT) {
         dimensions = dim->getDimensions();
         dim->reset();   // set virtual position to zero

         switch (dimensions) {
         case 0:
            Log::error("UserDationNB: EOF needs DIM");
            throw theInternalDationSignal;
            break;

         case 1: {
            DationDim1 * d1 = (DationDim1*)dim;
            d1->adv(endOfFilePos);
         }
         break;

         case 2: {
            DationDim2 * d2 = (DationDim2*)dim;
            r = endOfFilePos / dim->getColumns();
            c = endOfFilePos - (r * dim->getColumns());
            d2->adv(r, c);
         }
         break;

         case 3: {
            DationDim3 * d3 = (DationDim3*)dim;
            p = endOfFilePos /
                (dim->getRows() * dim->getColumns());
            endOfFilePos = endOfFilePos -
                           p * (dim->getRows() * dim->getColumns());
            r = endOfFilePos / dim->getColumns();
            c = endOfFilePos - (r * dim->getColumns());
            d3->adv(p, r, c);
         }
         break;
         }
      }
   }

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
         Log::error((char*)"UserDationNB: Dation is already open");
         throw theOpenFailedSignal;
      }

      if ((system->capabilities() & IDF) && !(p & IDF) && !(p & ANY)) {
         Log::error("UserDationNB: system dation requires IDF");
         throw theDationParamSignal;
      }

      if (!(system->capabilities() & IDF) && (p & IDF)) {
         Log::error("UserDationNB: system dation does not support IDF");
         throw theDationParamSignal;
      }

      if ((system->capabilities() & OPENMASK & p) !=
            (p & OPENMASK)) {
         Log::error("UserDationNB: open parameter not supported "
                    "by system device");
         throw theDationParamSignal;
      }

      if (!(p & IDF) && (!!(p & NEW) + !!(p & OLD)) > 0) {
         Log::error("UserDationNB: OLD/NEW requires IDF");
         throw theDationParamSignal;
      }

      if ((!!(p & CAN) + !!(p & PRM)) > 1) {
         Log::error("UserDationNB: ether CAN or PRM allowed");
         throw theInternalDationSignal;
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

   /* does nothing, if n <= 0
      write fillChar n times to the output
   */
   void UserDationNB::fill(const Fixed<31> n, char fillChar) {
      Fixed<31> h = n;

      while (h.x > 0) {
         work->dationWrite(&fillChar, 1);
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
         work->dationRead(&dummy, 1);
         //dationRead(&dummy, 1);
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
         work->dationRead(&dummy, 1);
         //dationRead(&dummy, 1);
         h.x --;

         if (dummy == '\n' || dummy == '\f') {
            work->dationUnGetChar(dummy);
            //dationUnGetChar(dummy);
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
//printf("skip until %d*%x\n", n.x,targetChar);

      while (h.x > 0) {
         //work->dationRead(&dummy, 1);
         //dummy = getChar();
         //dationRead(&dummy, 1);
         dummy = tfuBuffer.getChar();

//printf("got: %x\n", dummy);
         if (dummy == targetChar) {
            h.x --;
         } else if (dummy == '\f' && targetChar == '\n') {
            //work->dationUnGetChar(dummy);
            //dationUnGetChar(dummy);
            tfuBuffer.unGetChar(dummy);
            return;
         }
      }
   }


   void UserDationNB::assertOpenDirect() {
      assertOpen();

      if ((dationParams & DIRECT) == 0) {
         Log::error("UserDationNB: DIRECT dation required");
         throw theInternalDationSignal;
      }
   }
   void UserDationNB::assertOpenDirectOrForward() {
      assertOpen();

      if ((dationParams & (DIRECT | FORWARD)) == 0) {
         Log::error("UserDationNB: DIRECT or FORWARD dation required");
         throw theDationNotSupportedSignal;
      }
   }


   void UserDationNB::beginSequenceHook(TaskCommon * me) {
      struct BlockData bd;

      if (tfuBuffer.isUsed()) {
         tfuBuffer.markEmpty();

         if (currentDirection == Dation::OUT) {
            tfuBuffer.prepare();
         }
      }

      if (system->allowMultipleIORequests()) {
         // register current task in system dation
         TaskCommon::mutexLock(); // we need access to the global task lock
         bd.reason = IO;
         bd.u.io.dation = (SystemDation*)this;
         system->registerWaitingTask(me, currentDirection);

         // release the user dation lock, if multipl io is supported
         mutex.unlock();

         // aquired global task lock, since unblock expects unlocks this
         me->block(&bd);    // block releases the global task lock
      }

   }

   void UserDationNB::endSequenceHook() {
      if (currentDirection == Dation::OUT) {
         if (tfuBuffer.isUsed() && tfuBuffer.isNotEmpty()) {
            if (dationParams & Dation::FORWARD) {
               // write TFU buffer to system dation
               tfuBuffer.flushRecord();

               if (dationType == ALPHIC) {
                  fill(1, '\n');
               }

               // adjust dation position
//printf("internal ADV FORWARD by %d elements\n",
//    (dim->getColumns()-dim->getColumn()).x);
               internalAdv(zero, zero, dim->getColumns() - dim->getColumn());
               dim->gotoNextRecord();
            }

            if (dationParams & Dation::DIRECT) {
               tfuBuffer.flushRecord();

               // adjust dation position
               // not necessary, if last element in row was written
               if ((dim->getColumn() > zero).getBoolean()) {
//printf("internal ADV by %d elements\n",
//    (dim->getColumns()-dim->getColumn()).x);
                  internalAdv(zero, zero,
                              dim->getColumns() - dim->getColumn());
                  dim->gotoNextRecord();
               }

               doTfuAndSeekStuff();
            }

//   printf("TFU buffer flushed\n");
         }
      }

      if (currentDirection == Dation::IN) {
         if (tfuBuffer.isUsed()) {
            // forget unread input data
            // adjust dation position to beginning of next record
            if ((dim->getColumn() > zero).getBoolean()) {
//printf("endSequence-FORWARD-TFU: internal ADV by %d elements\n",
//    (dim->getColumn()).x);
               internalAdv(zero, zero, dim->getColumns() - dim->getColumn());
            }

            dim->gotoNextRecord();

            if (dationParams & Dation::DIRECT) {
               doTfuAndSeekStuff();
            }
         }
      }

      // if multiple IO-requests are allowed
      // the task unblocking is done by the system dation
      // there is no need to release the user dations mutex

      if (!systemDation->allowMultipleIORequests()) {
         // release the mutex of the user dation
         // this must not be done, when multiple I/O-operations
         // are allowed. In this case the concrete system dation
         // already released the mutex
         mutex.unlock();
      }

   }

   int UserDationNB::toPositioningFormat(TaskCommon * me,
                                         IOFormatEntry * fmtEntry) {
      int returnValue = 0;

      switch (fmtEntry->format) {
      default:
         printf("unsupported format %d\n", fmtEntry->format);
         break;

      case IOFormatEntry::X:
         toX(*fmtEntry->fp1.constF31Ptr);
         break;

      case IOFormatEntry::SKIP:
         toSkip(*fmtEntry->fp1.constF31Ptr);
         break;

      case IOFormatEntry::RST:
         rst(fmtEntry->fp1.voidPtr,
             fmtEntry->fp2.intValue);
         break;

      case IOFormatEntry::InduceFormat:
         Signal::throwSignalByRst(fmtEntry->fp1.intValue);
         break;
      }

      return returnValue;
   }

   int UserDationNB::fromPositioningFormat(TaskCommon * me,
                                           IOFormatEntry * fmtEntry) {
      int returnValue = 0;

      switch (fmtEntry->format) {
      default:
         printf("unsupported format %d\n", fmtEntry->format);
         break;

      case IOFormatEntry::X:
         fromX(*fmtEntry->fp1.constF31Ptr);
         break;

      case IOFormatEntry::SKIP:
         fromSkip(*fmtEntry->fp1.constF31Ptr);
         break;

      case IOFormatEntry::RST:
         rst(fmtEntry->fp1.voidPtr,
             fmtEntry->fp2.intValue);
         break;

      case IOFormatEntry::InduceFormat:
         Signal::throwSignalByRst(fmtEntry->fp1.intValue);
         break;
      }

      return returnValue;
   }

}
