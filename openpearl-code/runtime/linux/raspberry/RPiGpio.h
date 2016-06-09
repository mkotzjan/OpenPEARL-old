#ifndef RPIGPIO_INCLUDED
#define RPIGPIO_INCLUDED

#include <stdint.h>

namespace pearlrt {

   /**
   helper class for access to the gpio bits ofthe raspberry pi

   note that the broadcom gpio supports write access to output only
   with set and clear bit - thus no mutex protection is needed at this level.
   */
   class RPiGpio {
   private:
      int availableBits;
      RPiGpio();
      volatile uint32_t *gpio_map;

   public:
      /**
         supported modes of the i/o-usage

         <ul>
         <li>DIGIN provides digital input mode
         <li>DIGOUT provides digital output mode
         </ul>
      */
      enum RPiGpioMode {DIGOUT, DIGIN};
      enum RPiGpioPud {NONE, DOWN, UP};

      static RPiGpio* instance();
      /**
      set gpio direction

      \param start starting bit number (large value)
               (2 to 27 are available on RPi model 2)
      \param width number of bits to be used
      \param direction GpioMode::DIGOUT or GpioMode::DIGIN
      \param pud pull-up pull-down configuration
      */
      void useBits(int start, int width,
                   RPiGpioMode direction, RPiGpioPud pud);

      /**
      write the bits from value to the gpiobits from (start .. start-width+1)

      \param start first (leftmost) gpio bit number (large value)
      \param width number of consecutive gpio bits (2,...26)
      \param value the value to be written. The value is left adjusted
                    shifting occurs inside of this method
      */
      void writeBits(int start, int width, int32_t value);

      /**
      read the bits from the gpio bits from (start .. start-width+1)

      \param start first (leftmost) gpio bit number (large value)
      \param width number of consecutive gpio bits (1,...26)
      \returns the value to be written. The value is right adjusted
      */
      int32_t readBits(int start, int width);
   };
}
#endif
