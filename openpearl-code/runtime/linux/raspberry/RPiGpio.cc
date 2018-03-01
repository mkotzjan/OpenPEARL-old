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

#include "RPiGpio.h"
#include "Log.h"
#include "Signals.h"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "../../../configuration/include/autoconf.h"


namespace pearlrt {

#define BLOCK_SIZE (4*1024)

// gpio registers offsets
#define FSEL_OFFSET                 0   // 0x0000
#define SET_OFFSET                  7   // 0x001c / 4
#define CLR_OFFSET                  10  // 0x0028 / 4
#define PINLEVEL_OFFSET             13  // 0x0034 / 4
#define EVENT_DETECT_OFFSET         16  // 0x0040 / 4
#define RISING_ED_OFFSET            19  // 0x004c / 4
#define FALLING_ED_OFFSET           22  // 0x0058 / 4
#define HIGH_DETECT_OFFSET          25  // 0x0064 / 4
#define LOW_DETECT_OFFSET           28  // 0x0070 / 4
#define PULLUPDN_OFFSET             37  // 0x0094 / 4
#define PULLUPDNCLK_OFFSET          38  // 0x0098 / 4


// Pull-up/pull-down flags
#define PUD_OFF  0
#define PUD_DOWN 1
#define PUD_UP   2


// the pull-up/pull-down configuration needs a short time to settle
// this happens ion device setup during startup -- a delay
// loop is the best fitting method
   static void short_wait(void) {
      int i;

      for (i = 0; i < 150; i++) { // wait 150 cycles
         asm volatile("nop");
      }
   }

   RPiGpio* RPiGpio::instance() {
      static RPiGpio* _instance = 0;

      if (!_instance) {
         _instance = new RPiGpio();
      }

      return _instance;
   }

   RPiGpio::RPiGpio() {
      int mem_fd;

      availableBits = 0;
#if defined(CONFIG_LINUX_Raspberry_Version_V1B)
      // version 1 model B
      // available GPIO bits on RPi
      // Rev 2 P1 : 2,3,4,7,8,9,10,11,14,15,17,18,21,22,23,24,25,27
      //       bit31-+                                     +-bit0
      //     mask:   0000 1011 1110 0110 1100 1111 1001 1100
      // Rev 2 P5:  28,29,30,31
      //     mask:   1111 0000 0000 0000 0000 0000 0000 0000
      // used by: serial port: 14,15
      //             0000 0000 0000 0000 1100 0000 0000 0000
      availableBits = 0x0fBE60F9c;
#elif defined(CONFIG_LINUX_Raspberry_Version_V2B)
      // version 2 model b
      // bit 2-27 are available
      // used by: serial port: 14,15
      //             0000 0000 0000 0000 1100 0000 0000 0000
      availableBits = 0x0fff3ffc;
#elif defined(CONFIG_LINUX_Raspberry_Version_V3B)
      // version 3 model b
      // bit 2-27 are available
      // used by: serial port: 14,15
      //             0000 0000 0000 0000 1100 0000 0000 0000
      availableBits = 0x0fff3ffc;
#else
#error "Raspberry Pi version not set"
#endif

      // try /dev/gpiomem  - this does not require root privs
      if ((mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC)) > 0) {
         gpio_map = (uint32_t *)mmap(NULL, BLOCK_SIZE,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED, mem_fd, 0);

         if ((void*)gpio_map == MAP_FAILED) {
            Log::error("could not map GPIO registers");
            throw theInternalDationSignal;
         }
      } else {
         Log::error("could not open /dev/gpiomem");
         throw theInternalDationSignal;
      }

      // after mmap, we do not need the devce file open
      close(mem_fd);
   }


   void RPiGpio::useBits(int start, int width,
                         RPiGpioMode direction, RPiGpioPud pud) {
      // simple implemention for gpio bits 2-27
      int gpio;

      for (gpio = start; gpio > start - width; gpio--)  {
         if ((availableBits & (1 << gpio)) == 0) {
            Log::error("RPiGpio: bit %d is multiple used or not available",
                       gpio);
            throw theDationParamSignal;
         }

         availableBits &= ~(1 << gpio);
      }

      for (gpio = start; gpio > start - width; gpio--)  {
         int offset = FSEL_OFFSET + (gpio / 10);
         int shift = (gpio % 10) * 3;

         int clk_offset = PULLUPDNCLK_OFFSET;

         switch (pud) {
         case NONE:
            *(gpio_map + PULLUPDN_OFFSET) &= ~3;
            break;

         case DOWN:
            *(gpio_map + PULLUPDN_OFFSET) =
               (*(gpio_map + PULLUPDN_OFFSET) & ~3) | PUD_DOWN;
            break;

         case UP:
            *(gpio_map + PULLUPDN_OFFSET) =
               (*(gpio_map + PULLUPDN_OFFSET) & ~3) | PUD_UP;

            break;
         }

         short_wait();
         *(gpio_map + clk_offset) = 1 << (gpio % 32);
         short_wait();
         *(gpio_map + PULLUPDN_OFFSET) &= ~3;
         *(gpio_map + clk_offset) = 0;

         if (direction == DIGOUT) {
            *(gpio_map + offset) =
               (*(gpio_map + offset) & ~(7 << shift)) | (1 << shift);
         } else {  // direction == INPUT
            *(gpio_map + offset) =
               (*(gpio_map + offset) & ~(7 << shift));
         }
      }
   }


   void RPiGpio::writeBits(int start, int width, int32_t data) {
      // the GPIO-Block does not provide an output data register
      //  clear all zero bits  in data[start..start-width] first
      //  set all set bits  in data[start..start-width] afterwords
      //
      // simple implemention for gpio bits 0..31

      int maskSet = 0;
      int maskClr = 0;

      for (int gpio = start; gpio > start - width; gpio--) {
         if (data & 0x80000000) { // value == HIGH
            maskSet |= 1 << gpio;
         } else {     // value = LOW
            maskClr |= 1 << gpio;
         }

         data <<= 1; // next bit
      }

      if (maskClr) {
         *(gpio_map + CLR_OFFSET) = maskClr;
      }

      if (maskSet) {
         *(gpio_map + SET_OFFSET) = maskSet;
      }
   }

   int32_t RPiGpio::readBits(int start, int width) {
      // simple implementation for gpios < 32
      int32_t value;

      // create a mask for the valid bits to return
      uint32_t mask = 0xffffffff;
      mask >>= width;
      mask = ~mask;

      value = *(gpio_map + PINLEVEL_OFFSET);
      value <<= 31 - start;   // bits are numbered 31 .. 0
      value &= mask;

      return value;
   }

}
