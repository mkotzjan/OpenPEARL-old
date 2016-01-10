#include "Signals.h"
#include "Log.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

extern "C" {
void __cyg_profile_func_enter(void*this_fn, void* call_site)
     __attribute__((no_instrument_function)); 

void __cyg_profile_func_exit(void*this_fn, void* call_site)
     __attribute__((no_instrument_function));

void __cyg_profile_func_enter(void* this_fn, void*call_site) {
   int x;
   x = xTaskGetSchedulerState();
   if (x > 1) {
      x = uxTaskGetCurrentFreeStack();
      printf("%s: entering %p stack at %p free elements %d ",
       pcTaskGetTaskName(NULL), (int*)this_fn, &x, x);
      x = uxTaskGetStackHighWaterMark(NULL);
      printf("  High Watermark: %d\n",x);
   }
}
void __cyg_profile_func_exit(void* this_fn, void*call_site) {
//   printf("  exiting %p\n", (int*)this_fn);
}
}

