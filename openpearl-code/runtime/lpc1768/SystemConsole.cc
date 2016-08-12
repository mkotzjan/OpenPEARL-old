#include "SystemConsole.h"
#include "Lpc17xxUartInternal.h"

namespace pearlrt {
   GenericUart* SystemConsole::console = 0;

   GenericUart* SystemConsole::getInstance() {
      if (! console) {
         console =
            Lpc17xxUartInternal::getInstance(0, 115200, 7, 1 , 'E', true);
      }

      return console;
   }
}

