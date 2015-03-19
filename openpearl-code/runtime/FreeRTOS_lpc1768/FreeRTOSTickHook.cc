#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSTickHook.h"
#include "Clock.h"

void vApplicationTickHook(void) {
   pearlrt::Clock::nextTick();
}
