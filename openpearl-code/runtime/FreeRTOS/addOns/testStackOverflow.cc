#include "Signals.h"
#include "Log.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/** minimum number of required free stack elements (words of 4 bytes)
  in an application thread
*/

#define STACKLIMIT 250

extern "C" {

   void __cyg_profile_func_enter(void*this_fn, void* call_site)
   __attribute__((no_instrument_function));

   void __cyg_profile_func_exit(void*this_fn, void* call_site)
   __attribute__((no_instrument_function));
}


void __cyg_profile_func_enter(void* this_fn, void*call_site) {
   int x;
   x = xTaskGetSchedulerState();

   if (x > 1) {
      x = uxTaskGetCurrentFreeStack();

      if (x < STACKLIMIT) {
         // we do not know the current task
         // just print the message about stack usage.
         // the termination message will contain the task name
         pearlrt::Log::error("current free stack only %d free elements", x);
         throw pearlrt::theStackOverflowSignal;
      }

      x = uxTaskGetStackHighWaterMark(NULL);

      if (x < STACKLIMIT) {
         // we do not know the current task
         // just print the message about stack usage.
         // the termination message will contain the task name
         pearlrt::Log::error("stack used up to %d free elements", x);
         throw pearlrt::theStackOverflowSignal;
      }
   }
}

void __cyg_profile_func_exit(void* this_fn, void*call_site) {
   /* do nothing -- just satisfy the symbol  */
}

