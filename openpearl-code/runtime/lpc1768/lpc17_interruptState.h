/**
\file 

\brief test the interrupt state

This method works for all ARM-Cortex-M processors

*/

#include "chip.h"

/**
check the current interrupt state

\returns 0, if not in interrupt service
         1, if interrupt is active
*/
static inline int lpc17_isInterrupt()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ;
}

