#include "SystemConsole.h"
#include "Esp32UartInternal.h"

namespace pearlrt {
   GenericUart* SystemConsole::console = 0;

   GenericUart* SystemConsole::getInstance() {
      if (! console) {
         console =
            Esp32UartInternal::getInstance(0, 115200, 8, 1 , 'N', false);
      }

      return console;
   }
}

