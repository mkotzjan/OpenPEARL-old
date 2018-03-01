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

/*--------------------------------------------
Modifications:
  Dec 2015: rm : SystemInit sets the CPU clock to 100MHz
                 This is called from the startup-routine
                 just before main is invoked
-----------------------------------------------*/

#include "chip.h"

static void pllfeed(int pllnum) {
   LPC_SYSCON->PLL[pllnum].PLLFEED = 0xAA;
   LPC_SYSCON->PLL[pllnum].PLLFEED = 0x55;
}

void SystemInit() {
   /* Sets the CPU clock to 100Mhz from the external 12Mhz
    * crystal, following the instructions of the NXP UM10360
    * Manual, 4.5.13 */

   /* Disconnect the Main PLL if it is connected already
    * Which is the same as enabling, disconnected PLL for
    * all intents and purposes*/
   LPC_SYSCON->PLL[0].PLLCON = (1 << 1);
   pllfeed(0);

   /* Disable the PLL*/
   LPC_SYSCON->PLL[0].PLLCON = 0;
   pllfeed(0);
   /* CPU Clock divider = 1 to improve speed */
   LPC_SYSCON->CCLKSEL = 0;
   /* Enable the crystal, wait for lock*/
   LPC_SYSCON->SCS = (1 << 5);

   while (!(LPC_SYSCON->SCS & (1 << 6)));

   /* Pick the external Oscillator */
   LPC_SYSCON->CLKSRCSEL = (1 << 0);
   /* FCCO = ((24+1) * 2 * (12MHz/2)) = 300MHz */
   LPC_SYSCON->PLL[0].PLLCFG = (1 << 16) | (24 << 0);
   pllfeed(0);
   /* Enable the PLL */
   LPC_SYSCON->PLL[0].PLLCON = (1 << 0);
   pllfeed(0);

   /*Wait for PLL lock*/
   while (!(LPC_SYSCON->PLL[0].PLLSTAT & (1 << 26))) {}

   /* CPUClock Divider: 300Mhz/3=100Mhz */
   LPC_SYSCON->CCLKSEL = 2;

   /*Connect the PLL*/
   LPC_SYSCON->PLL[0].PLLCON = (1 << 0) | (1 << 1);
   pllfeed(0);

   /* Put all the peripheral clocks to their CCLK/4 reset value */
   LPC_SYSCON->PCLKSEL[0] = 0;
   LPC_SYSCON->PCLKSEL[1] = 0;

   SystemCoreClockUpdate();
}


