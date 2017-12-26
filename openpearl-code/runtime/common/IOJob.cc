#include "IOJob.h"

namespace pearlrt {

   LoopControl::LoopControl(int nbrOfItems, bool cyclic) {
      loopLevel = -1;   // we are outside of all loops
      item = -1;
      this->numberOfItems = nbrOfItems;
      workCyclic = cyclic;
   }

   int LoopControl::next()  {
//printf("next Format( item: %d, loopLevel=%d\n",
//       item, loopLevel);
      // test if we are in an loop and last statment was reached
      while (loopLevel >= 0 &&
             item == loopControl[loopLevel].lastItem) {

//printf("end format loop: level=%d start=%d loops =%d last =%d\n",
// loopLevel, loopControl[loopLevel].startItem,
// loopControl[loopLevel].loops ,
// loopControl[loopLevel].lastItem);
         loopControl[loopLevel].loops--;

         if (loopControl[loopLevel].loops > 0) {
            // uncompleted inner loop - do at least one more
            // iteration
            item = loopControl[loopLevel].startItem;
            loopControl[loopLevel].offsetIncrement +=
              loopControl[loopLevel].sizeOfBody;
//printf("next --> %zu\n", item);

            return item;
         } else {
            // current loop level completed -- test next loop
            // level
            loopLevel --;
         }
      }

      // take next element from list (restarting on demand)
      item ++;

      if (workCyclic && item >= numberOfItems) {
         item = 0;
      }

//printf("next --> %zu\n", item);
      return item;
   }

   int LoopControl::enter(int nbrOfItems, int nbrOfRepetitions,
                          size_t sizeOfBody) {
      loopLevel ++;

      if (loopLevel == MAX_LOOP_LEVEL) {
         Log::error("too many nested formats");
         throw theDationFormatRepetitionOverflow;
      }

      if (nbrOfRepetitions <= 0) {
         Log::error("number of repetitions uts be >0");
         throw theDationFormatRepetitionValue;
      }

      loopControl[loopLevel].startItem = item + 1;
      loopControl[loopLevel].lastItem = item + nbrOfItems;
      loopControl[loopLevel].loops = nbrOfRepetitions;
      loopControl[loopLevel].sizeOfBody = sizeOfBody;
      loopControl[loopLevel].offsetIncrement = 0;

//printf("enter loop: level=%d start=%d loops =%d last =%d size=%zu offsetIncr = %zu Item=%d\n",
//                  loopLevel,
//                  loopControl[loopLevel].startItem,
//                  loopControl[loopLevel].loops ,
//                  loopControl[loopLevel].lastItem,
//                  loopControl[loopLevel].sizeOfBody,
//                  loopControl[loopLevel].offsetIncrement,
//                  item);
      item ++;  // skip enter statement
      return item;
   }

   size_t LoopControl::getOffset() {
       size_t result=0;

       if (loopLevel >= 0) {
           result = loopControl[loopLevel].offsetIncrement;
       }
//printf("\n getOffset=%zu\n", result);
       return result;
   }
}
