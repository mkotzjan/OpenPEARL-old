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

#include "SystemConfig.h"
#include "systeminit.h"
#include "chip.h"

static void realinit_CpuClock();
static void realinit_ClockRTC();
static void realinit_ClockMonotonicRealtime();

__attribute__((used)) void software_init_hook(){
	systeminit(ClockMonotonicRealtime);
}

void systeminit(enum systeminit sysinit){
	static unsigned int initialized = 0;

	if(initialized&(1<<sysinit))
		return;

	switch(sysinit){
	case CpuClock:
		realinit_CpuClock();
		break;
	case ClockRTC:
		systeminit(CpuClock);
		realinit_ClockRTC();
		break;
	case ClockMonotonicRealtime:
		systeminit(ClockRTC);
		realinit_ClockMonotonicRealtime();
		break;
	}
	initialized|=(1<<sysinit);
}

static void pllfeed(int pllnum) {
	LPC_SYSCON->PLL[pllnum].PLLFEED = 0xAA;
	LPC_SYSCON->PLL[pllnum].PLLFEED = 0x55;
}

static void realinit_CpuClock() {
	/* Sets the CPU clock to 100Mhz from the external 12Mhz
	 * crystal, following the instructions of the NXP UM10360
	 * Manual, 4.5.13 */

	/* Disconnect the Main PLL if it is connected already
	 * Which is the same as enabling, disconnected PLL for
	 * all intents and purposes*/
	LPC_SYSCON->PLL[0].PLLCON = (1<<1);pllfeed(0);
	/* Disable the PLL*/
	LPC_SYSCON->PLL[0].PLLCON = 0;pllfeed(0);
	/* CPU Clock divider = 1 to improve speed */
	LPC_SYSCON->CCLKSEL = 0;
	/* Enable the crystal, wait for lock*/
	LPC_SYSCON->SCS=(1<<5);
	while(!(LPC_SYSCON->SCS&(1<<6)));
	/* Pick the external Oscillator */
	LPC_SYSCON->CLKSRCSEL = (1<<0);
	/* FCCO = ((24+1) * 2 * (12MHz/2)) = 300MHz */
	LPC_SYSCON->PLL[0].PLLCFG = (1<<16)|(24<<0);pllfeed(0);
	/* Enable the PLL */
	LPC_SYSCON->PLL[0].PLLCON = (1<<0);pllfeed(0);
	/*Wait for PLL lock*/
	while (!(LPC_SYSCON->PLL[0].PLLSTAT&(1<<26))) {} /* Wait for the PLL to Lock */
	/* CPUClock Divider: 300Mhz/3=100Mhz */
	LPC_SYSCON->CCLKSEL = 2;

	/*Connect the PLL*/
	LPC_SYSCON->PLL[0].PLLCON = (1<<0)|(1<<1);pllfeed(0);

	/* Put all the peripheral clocks to their CCLK/4 reset value */
	LPC_SYSCON->PCLKSEL[0] = 0;
	LPC_SYSCON->PCLKSEL[1] = 0;

	SystemCoreClockUpdate();
}

extern void systeminit_rtc_settime(unsigned int fallbackstamp);
static void realinit_ClockRTC(){
	Chip_RTC_Init(LPC_RTC);
	Chip_RTC_Enable(LPC_RTC, ENABLE);
	systeminit_rtc_settime(UNIXSTAMP);
}

static void realinit_ClockMonotonicRealtime(){
	LPC_RTC->CIIR = 1; //interrupt on second increment
	//TODO: double check
	LPC_RTC->RTC_AUXEN = 0; //disable the oscillator fail flag

	LPC_SYSCON->PCONP|=(1<<1);//powerup Timer0
	LPC_SYSCON->PCLKSEL[0]|=(1<<2);//bits 3:2 = 01 to set PCLK_TIMER0 to run at full speed. reset=00
	LPC_TIMER0->CTCR=0;//Timer0 is running in timer mode.
	LPC_TIMER0->TCR=3;//reset and disable timer0
	LPC_TIMER0->PR=0;//Prescaler=0;

	LPC_RTC->ILR = 3; //clear interrupt flags
//	NVIC->ISER[0] = (1<<RTC_IRQn);
	NVIC_EnableIRQ(RTC_IRQn);
}

void systeminit_USBClock(){
	/* Disconnect and disable PLL*/
	LPC_SYSCON->PLL[1].PLLCON = 0;pllfeed(1);
	/* USBClock M=4, P=2  */
	LPC_SYSCON->PLL[1].PLLCFG = (2<<5)|(4<<0);pllfeed(1);
	/* Enable and connect the PLL */
	LPC_SYSCON->PLL[1].PLLCON = (1<<0)|(1<<1);pllfeed(1);
	/*Wait for PLL lock*/
	while (!(LPC_SYSCON->PLL[1].PLLSTAT&(1<<10))) {} /* Wait for the PLL to Lock */
}

