#ifndef SYSTEMCONSOLE_INCLUDED
#define SYSTEMCONSOLE_INCLUDED

#include "Esp32UartInternal.h"

namespace pearlrt {

   /**
   \brief the SystemConsole object

   Each plattform needs a system console for the output of
   printf (recommended by retarget-mechanism) and emergency error
   messages. The concrete implementation depands on the specific plattform.
   Each specific plattform MUST implement this class.
   */
   class SystemConsole {
   private:
      SystemConsole();
      static GenericUart * console;

   public:


      /** obtain access to the system console object - create the
      console object if necessary with default parameters

      \return pointer to the system console object
      */
      static GenericUart* getInstance();

   };

}
#endif
