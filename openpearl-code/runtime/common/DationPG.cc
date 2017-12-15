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

/**
\file
\brief implementation of  PG-userdation

*/
#include "Dation.h"
#include "UserDation.h"
#include "DationPG.h"
#include "Signals.h"
#include "Log.h"
#include "DationDim.h"
#include "SystemDationNB.h"
#include "SystemDationNBSink.h"
#include "SystemDationNBSource.h"
#include "IOFormats.h"
#include "Fixed.h"
#include "PutHelper.h"
#include "PutClock.h"
#include "GetClock.h"
#include "PutDuration.h"
#include "GetDuration.h"


namespace pearlrt {

   DationPG::DationPG(SystemDationNB* parent,
                      int params,
                      DationDim * dimensions,
                      void * tfuRecord)
      : UserDationNB(parent, params, dimensions,
                     UserDationNB::ALPHIC) {
      dationStatus = CLOSED;

#warning "wieso hat DationPG kein CYCLIC support?"

      if (params & CYCLIC) {
         Log::error("DationPG: does not support CYCLIC");
         throw theDationParamSignal;
      }

      stepSize = Fixed<31>(1);
      tfuBuffer.setupRecord(dim->getColumns().x, (char*)tfuRecord, ' ');
   }

   void DationPG::dationRead(void * destination, size_t size) {
      char * d = (char*)destination;
      assertOpen();

      if (!(dationParams & (IN | INOUT))) {
         Log::error("DationPG: dation not opened as input");
         throw theDationParamSignal;
      }

      if (!(system->capabilities() & (IN | INOUT))) {
         Log::error("DationPG: device does not support read");
         throw theDationParamSignal;
      }

      // get first character from unget buffer
      *d = source.getChar();

      // read remaining from device
      if (size > 1) {
         work->dationRead(d + 1, size - 1);
      }
   }

   void DationPG::dationWrite(void * destination, size_t size) {
      char * d = (char*)destination;
      assertOpen();

      if (!(dationParams & (OUT | INOUT))) {
         Log::error("DationPG: Only writing is allowed");
         throw theDationParamSignal;
      }

      if (!(system->capabilities() & (OUT | INOUT))) {
         Log::error("DationPG: device does not support write");
         throw theDationParamSignal;
      }

      // send data to sink
      for (size_t i = 0; i < size; i++) {
         sink.putChar(*d);
         d++;
      }
   }

   void DationPG::dationSeek(const Fixed<31> & p, const int dationParam) {
      assertOpen();
      source.forgetUnGetChar();
      work->dationSeek(p, dationParam);
   }


   void DationPG::dationUnGetChar(const char c) {
      source.unGetChar(c);
   }

   void DationPG::internalOpen() {
      tfuBuffer.setSystemDation((SystemDationNB*)work);
      /*
            } else {
               if (dationParams & (OUT | INOUT)) {
                  sink.setSystemDationNB((SystemDationNB*)work);
               }

               if (dationParams & (IN | INOUT)) {
                  source.setSystemDationNB((SystemDationNB*)work);
               }
            }
            setupIOFormats(&sink, &source);
      */
      setupIOFormats(&tfuBuffer, &tfuBuffer);
   }

   void DationPG::internalClose() {
   }

   void DationPG::checkCapacity(Fixed<31> n) {
      // move the read/write pointer.
      // this method will throw an exception if there is too little space
      adv(n);
   }

   size_t DationPG::getNextFormatElement(IOFormatList * formatList,
                                         size_t formatItem,
                                         FormatLoop * ls) {
//printf("getNextFormat(formatList, %zu, loopLevel=%d\n",
//       formatItem, ls->loopLevel);
      // test if we are in an loop and last statment was reached
      if (ls->loopLevel >= 0 &&
            formatItem == ls->loopControl[ls->loopLevel].lastFormat) {

//printf("end loop: level=%d start=%zu loops =%d last =%zu\n",
// ls->loopLevel, ls->loopControl[ls->loopLevel].startFormat,
// ls->loopControl[ls->loopLevel].loops ,
// ls->loopControl[ls->loopLevel].lastFormat);
         ls->loopControl[ls->loopLevel].loops--;

         if (ls->loopControl[ls->loopLevel].loops > 0) {
            // uncompleted inner loop - do at least one more
            // iteration
            formatItem = ls->loopControl[ls->loopLevel].startFormat;

            if (formatList->entry[formatItem].format ==
                  IOFormatEntry::LoopStart) {
               formatItem = getNextFormatElement(formatList,
                                                 formatItem - 1,
                                                 ls);
            }
         } else {
            // current loop level completed -- test next loop
            // level
            ls->loopLevel --;
            formatItem = getNextFormatElement(formatList, formatItem, ls);
         }

         return formatItem;
      }

      // found open loop which is not terminated
//            formatItem ++;
//            if (formatItem >= formatList->nbrOfEntries) {
//               formatItem = 0;
//            }
//         }
//         return formatItem;
//      }

      // take next format from list (restarting on demand)
      formatItem ++;

      if (formatItem >= formatList->nbrOfEntries) {
         formatItem = 0;
      }

      if (formatList->entry[formatItem].format == IOFormatEntry::LoopStart) {
         ls->loopLevel ++;

         if (ls->loopLevel == MAX_LOOP_LEVEL) {
            Log::error("too many nested formats");
            throw theDationFormatRepetitionOverflow;
         }

         ls->loopControl[ls->loopLevel].startFormat = formatItem + 1;
         ls->loopControl[ls->loopLevel].lastFormat = formatItem +
               formatList->entry[formatItem].fp1.intValue;
         ls->loopControl[ls->loopLevel].loops =
            formatList->entry[formatItem].fp2.intValue;

//printf("enter loop: level=%d start=%zu loops =%d last =%zu formatItem=%zu\n",
//                  ls->loopLevel,
//                  ls->loopControl[ls->loopLevel].startFormat,
//                  ls->loopControl[ls->loopLevel].loops ,
//                  ls->loopControl[ls->loopLevel].lastFormat,
//                  formatItem);
         formatItem = getNextFormatElement(formatList, formatItem, ls);
      }

      return formatItem;
      //}
   }


   void DationPG::put(TaskCommon * me,
                      IODataList * dataList, IOFormatList * formatList) {

      size_t formatItem = -1;
      FormatLoop loopStatus;

      loopStatus.loopLevel = -1;

      try {
         beginSequence(me, Dation::OUT);

         for (size_t dataElement = 0;
               dataElement < dataList->nbrOfEntries;
               dataElement++) {

            for (size_t dataIndex = 0;
                 dataIndex < * (dataList->entry[dataElement].numberOfElements);
                 dataIndex++) {
               formatItem = getNextFormatElement(formatList,
                                                 formatItem,
                                                 &loopStatus);

               while (formatList->entry[formatItem].format >=
                      IOFormatEntry::IsPositioning) {
                  toPositioningFormat(me, &formatList->entry[formatItem]);
                  formatItem = getNextFormatElement(formatList,
                                                    formatItem,
                                                    &loopStatus);
               }

               putDataFormat(me, &dataList->entry[dataElement], dataIndex,
                             &formatList->entry[formatItem]);
            }
         }

         formatItem ++;

         // treat pending format elements after last data element
         while (formatItem < formatList->nbrOfEntries) {
            if (formatList->entry[formatItem].format <=
                  IOFormatEntry::IsPositioning) {
               break;
            }

            toPositioningFormat(me, &formatList->entry[formatItem]);
            formatItem++;
         }

         endSequence();
      } catch (Signal &s) {
         if (! updateRst(&s)) {
            endSequence();
            throw;
         }

         endSequence();
      }
   }
   void DationPG::get(TaskCommon * me,
                      IODataList * dataList, IOFormatList * formatList) {

      size_t formatItem = -1;
      FormatLoop loopStatus;

      loopStatus.loopLevel = -1;

      try {
         beginSequence(me, Dation::IN);

         for (size_t dataElement = 0;
               dataElement < dataList->nbrOfEntries;
               dataElement++) {

            for (size_t dataIndex = 0;
                 dataIndex < * (dataList->entry[dataElement].numberOfElements);
                 dataIndex++) {
               formatItem = getNextFormatElement(formatList,
                                                 formatItem,
                                                 &loopStatus);

               while (formatList->entry[formatItem].format >=
                      IOFormatEntry::IsPositioning) {
                  fromPositioningFormat(me, &formatList->entry[formatItem]);
                  formatItem = getNextFormatElement(formatList,
                                                    formatItem,
                                                    &loopStatus);
               }

               getDataFormat(me, &dataList->entry[dataElement], dataIndex,
                             &formatList->entry[formatItem]);
            }
         }

         formatItem ++;

         // treat pending format elements after last data element
         while (formatItem < formatList->nbrOfEntries) {
            if (formatList->entry[formatItem].format <=
                  IOFormatEntry::IsPositioning) {
               break;
            }

            fromPositioningFormat(me, &formatList->entry[formatItem]);
            formatItem++;
         }

         endSequence();
      } catch (Signal &s) {
         if (! updateRst(&s)) {
            endSequence();
            throw;
         }

         endSequence();
      }
   }
}
