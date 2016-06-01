
#include "RPiGpio.h"
#include "Log.h"
#include "Signals.h"
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace pearlrt {

#define BLOCK_SIZE (4*1024)
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

#define HIGH 1
#define LOW  0

#define PUD_OFF  0
#define PUD_DOWN 1
#define PUD_UP   2

#define SETUP_OK           0
#define SETUP_DEVMEM_FAIL  1
#define SETUP_MALLOC_FAIL  2
#define SETUP_MMAP_FAIL    3
#define SETUP_CPUINFO_FAIL 4
#define SETUP_NOT_RPI_FAIL 5


#define INPUT  1 // is really 0 for control register!
#define OUTPUT 0 // is really 1 for control register!
#define ALT0   4

static void short_wait(void)
{
    int i;

    for (i=0; i<150; i++) {    // wait 150 cycles
        asm volatile("nop");
    }
}

RPiGpio* RPiGpio::instance() {
   static RPiGpio* _instance= 0;

   if (!_instance) {
      _instance = new RPiGpio();
   }
   return _instance;
}
      
RPiGpio::RPiGpio() {
    int mem_fd;

   // available GPIO bits on RPi
   // Rev 2 P1 : 2,3,4,7,8,9,10,11,14,15,17,18,22,23,24,25,27
   //       bit31-+                                     +-bit0 
   //     mask:   0000 1011 1100 0110 1100 1111 1001 1100
   // Rev 2 P5:  (17,18,)19,20 ** 17,18 are also on P1
   //     mask:   0000 0000 0001 1110 0000 0000 0000 0000
   // Rev 3 additional: 5,6,12,13,16,(19,20,)21,26 ** 19,20 already on P5 of rev 2.0
   //     mask:   0000 0100 0011 1001 0011 0000 0110 0000
   // we provide all bits of rev 3 here
   //     mask:   0000 1111 1111 1111 1111 1111 1111 1100
   //    bits 0,1,28,29,30,31 are not available
   availableBits = 0x0ffffffc;

    // try /dev/gpiomem  - this does not require root privs
    if ((mem_fd = open("/dev/gpiomem", O_RDWR|O_SYNC)) > 0) {
        gpio_map = (uint32_t *)mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0);
        if ((uint32_t)gpio_map < 0) {
            Log::error("could not map GPIO registers");
            throw theInternalDationSignal;
        } 
    } else {
            Log::error("could not open /dev/gpiomem");
            throw theInternalDationSignal;
    }
}


void RPiGpio::useBits(int start, int width, RPiGpioMode direction, RPiGpioPud pud) {
    // simple implemention for gpio bits 2-27
    int gpio;

    for (gpio=start; gpio> start-width; gpio--)  {
        if ((availableBits & (1<<gpio)) == 0) {
            Log::error("RPiGpio: bit %d used multiple", gpio);
            throw theIllegalParamSignal;
        }
        availableBits &= ~(1<<gpio);
    }
 
    for (gpio=start; gpio> start-width; gpio--)  {
       int offset = FSEL_OFFSET + (gpio/10);
       int shift = (gpio%10)*3;
  
       int clk_offset = PULLUPDNCLK_OFFSET;
       switch (pud) {
          case NONE: 
	     *(gpio_map+PULLUPDN_OFFSET) &= ~3;
             break;
          case DOWN:
            *(gpio_map+PULLUPDN_OFFSET) = (*(gpio_map+PULLUPDN_OFFSET) & ~3)
                                          | PUD_DOWN;
            break;
          case UP: *(gpio_map+PULLUPDN_OFFSET) = (*(gpio_map+PULLUPDN_OFFSET) & ~3) | PUD_UP;

                        break;
       }
       short_wait();
       *(gpio_map+clk_offset) = 1 << (gpio%32);
       short_wait();
       *(gpio_map+PULLUPDN_OFFSET) &= ~3;
       *(gpio_map+clk_offset) = 0;

       if (direction == OUTPUT) {
           *(gpio_map+offset) = (*(gpio_map+offset) & ~(7<<shift)) | (1<<shift);
       } else {  // direction == INPUT
           *(gpio_map+offset) = (*(gpio_map+offset) & ~(7<<shift));
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

    for (int gpio=start; gpio> start-width; gpio--) {
       if (data & 0x80000000) { // value == HIGH
           maskSet |= 1<< gpio;
       } else {     // value = LOW
           maskClr |= 1<< gpio;
       }
       data <<= 1; // next bit
    }
    if (maskClr) {
       *(gpio_map+CLR_OFFSET) = maskClr;
    }
    if (maskSet) {
       *(gpio_map+SET_OFFSET) = maskSet;
    }
}

int32_t RPiGpio::readBits(int start, int width) {
   // simple implementation for gpios < 32
   int32_t value;

   value = *(gpio_map+PINLEVEL_OFFSET);
   value <<= 32-start;
   return value;
}

}

