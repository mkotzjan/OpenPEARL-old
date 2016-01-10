#include <stdio.h>
#include "TaskList.h"
#include "Task.h"
#include "FreeRTOS.h"
static void dumpStacks() ;
extern "C" {
void vApplicationStackOverflowHook() {
   printf("Stack Overflow\n");
   printf("Task=%s\n",pcTaskGetTaskName(NULL));

   for (;;);
}

void vApplicationMallocFailedHook() {
   printf("malloc failed!\n");
   printf("Task=%s\n",pcTaskGetTaskName(NULL));

   for (;;);
}

void vApplicationIdleHook() {
}
};

