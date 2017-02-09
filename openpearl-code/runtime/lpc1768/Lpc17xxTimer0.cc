/*
 [The "BSD license"]
 Copyright (c) 2015 Jonas Meyer
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

/* --------------
  Modifications
  dec 2015: rm : translated into C++
*/

#define _POSIX_TIMERS

#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "Lpc17xxTimer0.h"
#include "chip.h"
#include "FreeRTOSConfig.h"
#include "time_addons.h"
namespace pearlrt {


   uint64_t Lpc17xxTimer0::nsec = 0;
   uint32_t Lpc17xxTimer0::ticksPerSecond = 1e8;
   uint32_t Lpc17xxTimer0::newTicksPerSecond = 0;

   unsigned int Lpc17xxTimer0::ticks2nsec(unsigned int ticks) {
      return (unsigned int)(
                ((uint64_t)ticks * ((uint64_t)1e9))
                / ((uint64_t)ticksPerSecond));
   }

   unsigned int Lpc17xxTimer0::nsec2ticks(unsigned int nsecs) {
      return (unsigned int)(
                (nsecs * (uint64_t)ticksPerSecond)
                / ((uint64_t)1e9));
   }

   /*timerarm prepares the HW to generate an Interrupt
    * no later than the specified time, but as late as
    * possible. No need to check wether alarmstamp is
    * in the past, ClackerTask already does*/
//static int timerarm_cb_systime(int64_t alarmstamp) {
   void Lpc17xxTimer0::setTimeOut(uint64_t alarmstamp) {
      LPC_TIMER0->IR = (1 << 6);

      // calculate the tick number for the timer
      //  calculate the number of ticks requierrd for the delay
      //  add the current value count register (TC)
      //  work modulo the number of ticks per second, if the sum
      //  is in the next period
      uint32_t mystamp = nsec2ticks(alarmstamp);
      mystamp += LPC_TIMER0->TC;
      mystamp %= ticksPerSecond;
      LPC_TIMER0->MR[2] = mystamp;

      if (LPC_TIMER0->TC >= ticksPerSecond) {
         printf("TC >= ticksPerSecond\n");
         return;
      }

      LPC_TIMER0->MCR |= (1 << 6);
      return;
   }

   void Lpc17xxTimer0::gettime(uint64_t *nsectime) {
      *nsectime = nsec;
      *nsectime += ticks2nsec(LPC_TIMER0->TC);
//printf("Timer0: gettime %"PRIu64" \n", *nsectime);
   }

   void Lpc17xxTimer0::settime(const uint64_t *nsectime) {
      nsec = *nsectime;
   }

   void Lpc17xxTimer0::start() {

      ticksPerSecond = Chip_Clock_GetSystemClockRate();
      printf("Timer0: ticksPerSeconds = %u\n", (unsigned)ticksPerSecond);

      LPC_SYSCON->PCONP |= (1 << 1); //powerup Timer 0
      //bits 3:2 = 01 to set PCLK_TIMER0 to run at full speed. reset=00
      LPC_SYSCON->PCLKSEL[0] |= (1 << 2);
      LPC_TIMER0->MR[1] = ticksPerSecond; //1s
      LPC_TIMER0->MR[2] = ~0;

      //reset+interrupt on mr1, interrupt on mr2
      LPC_TIMER0->TC = 0; //clear count register
      LPC_TIMER0->MCR = (0x3 << 3) | (0x01 << 6);
      LPC_TIMER0->CTCR = 0; //Timer0 is running in timer mode.
      LPC_TIMER0->PR = 0; //Prescaler=0;
      LPC_TIMER0->IR = (~0);//clear interrupts
      LPC_TIMER0->TCR = 1; //enable

      NVIC_SetPriority(TIMER0_IRQn, 5); // best prio to get good time base
      NVIC_EnableIRQ(TIMER0_IRQn);
   }

   void Lpc17xxTimer0::registerTimeBase() {
      register_timer_source(setTimeOut, gettime);
   }

   void Lpc17xxTimer0::drift(int drift) {
      // adjust the tick by 50% of the error
      // be aware that drift is given in ns and the tick rate is 10ns
      newTicksPerSecond = ticksPerSecond - drift / 20;
   }

//void TIMER0_IRQHandler() {
   void Lpc17xxTimer0::tick() {
      //decide between second and alarm interrupt
      if (LPC_TIMER0->IR & (1 << 1)) {
         nsec += (uint64_t)1e9;

         if (newTicksPerSecond > 0) {
            ticksPerSecond = newTicksPerSecond;
            newTicksPerSecond = 0;
            LPC_TIMER0->MR[1] = ticksPerSecond; //1s
         }

         LPC_TIMER0->IR = (1 << 1);
      }

      if (LPC_TIMER0->IR & (1 << 2)) {
         resume_timer_task_from_ISR();
         LPC_TIMER0->MCR &= ~(1 << 6);
         LPC_TIMER0->IR = (1 << 2);
      }
   }
}
