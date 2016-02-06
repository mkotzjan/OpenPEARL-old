/*
 * service.c
 *
 */
/*
 [A "BSD license"]
 Copyright (c) 2015  Rainer Mueller
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, that list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, that list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from that software without specific prior written permission.

 that SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 that SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdio.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "time_addons.h"
#include "FreeRTOSClock.h"
#include "allTaskPriorities.h"
#include "service.h"

#define SERVICE_STACK_SIZE 400
#define STACKLIMIT 100	// limit of minimum free stack for the service task

static TaskHandle_t xServiceTaskHandle;
static void serviceTask(void *);

static TCB_t serviceTcb;
static StackType_t serviceStack[SERVICE_STACK_SIZE];

static QueueHandle_t serviceQueue;

void init_service() {
   StructParameters_t createParameters;

   /* the interrupt service routine sends messages to this
   queue on each expiration of the timer. The length is
   set to something above 1 but not too large...
   maybe this schould be replaced by a counting semaphore...
   */
   serviceQueue = xQueueCreate(30, sizeof(ServiceJob));

   createParameters.pvParameter = NULL;
   createParameters.stack = serviceStack;
   createParameters.tcb = &serviceTcb;

   xTaskCreate(serviceTask,
               "serviceTask", SERVICE_STACK_SIZE, &createParameters,
               PRIO_TASK_SERVICE, &xServiceTaskHandle);

}

void add_service_from_ISR(ServiceJob * s) {
   if (xServiceTaskHandle) {
      if (pdTRUE != xQueueSendFromISR(serviceQueue, s, NULL)) {
         printf("error at xQuereSendFromISR\n");
      }

      //The yield is _very_ important for performance
      portYIELD_FROM_ISR(xServiceTaskHandle);
   }
}

void add_service(ServiceJob * s) {
   if (xServiceTaskHandle) {
      if (!xQueueSend(serviceQueue, s, portMAX_DELAY)) {
         printf("error at xQuereSendFromISR\n");
      }

      //The yield is _very_ important for performance
      taskYIELD();
   }
}

static void serviceTask(void *pcParameters) {
   int freeStack;
   ServiceJob s;

   while (1) {
      if (!xQueueReceive(serviceQueue, &s, portMAX_DELAY)) {
         printf("error at xQueueReceive\n");
      }

      /*
      printf("ServiceTask: current free elements: %d and  stack %d elements"
             " --> invoke job\n",
              uxQueueSpacesAvailable(serviceQueue), freeStack);
      */
      s.job(s.param);

      freeStack = uxTaskGetCurrentFreeStack();

      if (freeStack < STACKLIMIT) {
         printf(
            "ServiceTask: current stack only %d free elements"
            "ServiceTask terminates\n",
            freeStack);
         vTaskDelete(NULL);
      } else {
//       printf("ServiceTask: current free stack %d free elements\n",
//             freeStack);
      }

      freeStack = uxTaskGetStackHighWaterMark(NULL);

      if (freeStack < STACKLIMIT) {
         printf("ServiceTask: stack used up to %d free elements"
                "ServiceTask terminates\n",
                freeStack);
         vTaskDelete(NULL);
      } else {
//       printf("ServiceTask: stack used up to %d free elements\n", freeStack);
      }
   }
}

