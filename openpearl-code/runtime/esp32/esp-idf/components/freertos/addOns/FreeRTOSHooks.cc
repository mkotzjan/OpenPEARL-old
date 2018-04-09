#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "TaskList.h"
#include "Task.h"

static void dumpStacks() ;

extern "C" {
   void vApplicationStackOverflowHook() {
      printf("Stack Overflow\n");
      printf("Task=%s\n", pcTaskGetTaskName(NULL));

      for (;;);
   }

   void vApplicationMallocFailedHook() {
      printf("malloc failed!\n");
      printf("Task=%s\n", pcTaskGetTaskName(NULL));

      for (;;);
   }

   void vApplicationIdleHook() {
      /* do nothing - just be prepared if somebody sets the switch in the
         FreeRTOSConfig.h
      */
   }
};

