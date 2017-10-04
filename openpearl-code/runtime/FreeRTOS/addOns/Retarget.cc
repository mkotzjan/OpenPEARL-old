/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

#include "chip.h"
#include "cmsis.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Retarget.h"
#include <stdio.h>
#include "lpc17_interruptState.h"

#include "SystemConsole.h"


extern "C" {
   static pearlrt::GenericUart * console = 0;
   static bool useInterruptFlag = false;

   struct StdInOutJob {
      int len;
      char * data;
   };
   static struct StdInOutJob *stdOutJob = NULL;
   static struct StdInOutJob *stdInJob = NULL;

   int __attribute__((used)) _write(int fd, char * ptr, int len) {
      if (! console) {
         console = pearlrt::SystemConsole::getInstance();
      }

      if (useInterruptFlag && !lpc17_isInterrupt()) {
         // output in interrupt  mode
         struct StdInOutJob job;

         job.len = len;
         job.data = ptr;

         // wait until job from other thread has finished
         while (!stdOutJob) {};

         stdOutJob = &job;

         console->triggerOutput();

         // wait until this job has finished
         while (!stdOutJob) {};

         return len;
      } else {
         // output in polling mode
         int n = len;
         console->interruptEnable(false);

         while (len > 0) {
            console->sendCharPolling(*ptr);

            if (*ptr == '\n') {
               console->sendCharPolling('\r');
            }

            ptr ++;
            len --;
         }

         console->interruptEnable(true);
         return (n);
      }
   }

   int __attribute__((used)) _read(int fd, char * ptr, int len) {
      if (! console) {
         console = pearlrt::SystemConsole::getInstance();
      }

      if (useInterruptFlag) {
         // input in interrupt mode
         struct StdInOutJob job;

         job.len = len;
         job.data = ptr;

         while (!stdInJob) {}; // wait until job from other thread has finished

         stdInJob = &job;

         while (!stdInJob) {}; // wait until this job has finished

         return len;
      } else {
         // input in polling mode
         int received = 0;
         console->interruptEnable(false);

         while (received < len) {
            *ptr =  console->readCharPolling();

            // backspace and del
            if (*ptr == '\b' || *ptr == 0x7f) {
               if (received > 0) {
                  received --;
                  console->sendCharPolling('\b'); // local echo
                  console->sendCharPolling(' '); // local echo
                  console->sendCharPolling('\b'); // local echo
                  ptr --;
               }

               // test for newline
               // \r is not transmitted by lpc21isp in terminal mode
            } else if (*ptr == '\n') {
               console->sendCharPolling(*ptr); // local echo
               *ptr = '\n';
               console->sendCharPolling(*ptr); // local echo
               *(ptr + 1) = 0;
               received ++;
               break;
            } else {
               console->sendCharPolling(*ptr); // local echo
               ptr ++;
               received ++;
            }
         }

         console->interruptEnable(true);
         return (received);
      }
   }

   void _exit(int x) {

       useInterruptFlag = false;
      _write(1, (char*)"*** end. ***\n", 13);
      while (1);  // never return
   }


   static volatile uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
   static volatile int exceptNumber = 0;

   static void prvGetRegistersFromStack(uint32_t* faultStack) {
      r0 = faultStack[0];
      r1 = faultStack[1];
      r2 = faultStack[2];
      r3 = faultStack[3];
      r12 = faultStack[4];
      lr = faultStack[5];
      pc = faultStack[6];
      psr = faultStack[7];
   }

   static int handleControlStateReg;

   void lpc17_default_handler(void) __attribute__((naked));
   void NMI_Handler(void) __attribute__((naked));
   void HardFault_Handler(void) __attribute__((naked));
   void MemManage_Handler(void) __attribute__((naked));
   void BusFault_Handler(void) __attribute__((naked));
   void UsageFault_Handler(void) __attribute__((naked));

   void lpc17_default_handler(void) {
   // NVIC_INT_CTRL_CONSTis 0xe000ed04		\n"

       exceptNumber = * (volatile uint32_t*) 0xe000ed04;
       handleControlStateReg = * (volatile uint32_t*) 0xe000ed24;
    /*     asm volatile (
            " ldr r3, #e000ed04			\n"
            " ldr r2, [r3, #0]			\n"
      	     " ldr r3, exceptNumber			\n"
      	" str r2, [r3,#0]			\n"
       );

      asm volatile(
         " tst lr,#4			\n"
         " ite eq			\n"
         " mrseq r0, msp			\n"
         " mrsne r0, psp			\n"
         " ldr r1, [r0, #24]		\n"
         " ldr r2, handler2_address_const	\n"
         " bx r2				\n"
         " handler2_address_const: .word prvGetRegistersFromStack\n"
      );
*/
      useInterruptFlag = false;

//      printf("*** Default handler (vector index = %d)\n*** halt ***.\n", exceptNumber & 0x3ff);
      printf("*** Default handler (InterruptControl State Reg = %04X)\n"
        "   handleControlStatusReg = %04X\n"
        "   vector = %d\n"
	"*** halt ***.\n", exceptNumber,
                    handleControlStateReg,
                    exceptNumber&0x0ff);
   //   printf(" Registers: \n r0: %08x r1: %08x r2: %08x r3: %08x\n"
   //          "r12: %08x lr: %08x pc: %08x psr: %08x\n",
   //          r0, r1, r2, r3, r12, lr, pc, psr);

       if ( (exceptNumber & 0x0ff) == 3) {
          printf("usage fault status: %02X\n", *(int16_t*)0xe000ed2a);
       }
       while (1);  // never return

   }

   void NMI_Handler(void) {
       printf("NMI Handler *** halt *** \n");
       while(1);
   }

   void HardFault_Handler(void) {
       printf("*** HardFault Handler ***\n"
              "  Hard Fault Status:\t\t%08X\n"
              "  Bus Fault Status:\t\t%02X\n"
              "  MemManage Fault  Status:\t%02X\n"
              "  Usage Fault Status:\t\t%04X\n"
          "*** halt *** \n",
          SCB->HFSR, 
       //*(uint32_t*)(0xe000ed2c), // hard fault status register
       *(uint8_t*)(0xe000ed29), // bus fault status register
       *(uint8_t*)(0xe000ed28), // memmanage fault status register
       *(uint16_t*)(0xe000ed2a)); // usage fault status register
       if (*(uint8_t*)(0xe000ed29) & 0x080) {
          printf("  Bus Fault at: %08X\n", SCB->BFAR);
       }
       while(1);
   }
   void MemManage_Handler(void) {
       printf("MemManage Handler *** halt *** \n");
       while(1);
   }

   void BusFault_Handler(void) {
       printf("BusFault Handler *** halt *** \n");
       while(1);
   }

   void UsageFault_Handler(void) {
       printf("UsageFault Handler *** halt *** \n");
       while(1);
   }


};

namespace pearlrt {
   void Retarget::useInterrupts(bool use) {
      useInterruptFlag = use;
   }

   bool Retarget::getNextStdOutChar(char * nextTransmitChar) {
      if (stdOutJob) {
         if (stdOutJob->len > 0) {
            stdOutJob->len --;
            *nextTransmitChar = *(stdOutJob->data);
            stdOutJob->data ++;

            if (stdOutJob->len == 0) {
               stdOutJob = 0;
            }

            return true;
         }
      }

      return false;
   }

   bool Retarget::addNextStdInChar(char ch) {
      if (stdInJob) {
         if (stdInJob->len > 0) {
            stdInJob->len --;
            *(stdInJob->data) = ch;
            stdInJob->data ++;

            if (stdInJob->len == 0) {
               stdInJob = 0;
            }

            return true;
         }
      }

      return false;
   }
}

