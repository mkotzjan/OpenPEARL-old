#include "chip.h"
#include "clock_17xx_40xx.h"

/* System oscillator rate and RTC oscillator rate */
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

static void clockInit() {
	/* Disconnect the Main PLL if it is connected already */
	if (Chip_Clock_IsMainPLLConnected()) {
		Chip_Clock_DisablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_CONNECT);
	}

	/* Disable the PLL if it is enabled */
	if (Chip_Clock_IsMainPLLEnabled()) {
		Chip_Clock_DisablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE);
	}

	/* Enable the crystal */
	if (!Chip_Clock_IsCrystalEnabled())
		Chip_Clock_EnableCrystal();
	while(!Chip_Clock_IsCrystalEnabled()) {}

	/* Set PLL0 Source to Crystal Oscillator */
	Chip_Clock_SetCPUClockDiv(0);
	Chip_Clock_SetMainPLLSource(SYSCTL_PLLCLKSRC_MAINOSC);

	/* FCCO = ((15+1) * 2 * 12MHz) / (0+1) = 384MHz */
	Chip_Clock_SetupPLL(SYSCTL_MAIN_PLL, 15, 0);

	Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE);

	/* 384MHz / (3+1) = 96MHz */
	Chip_Clock_SetCPUClockDiv(3);
	while (!Chip_Clock_IsMainPLLLocked()) {} /* Wait for the PLL to Lock */

	Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_CONNECT);


}

void SystemInit() {
	clockInit();
	SystemCoreClockUpdate();
}

