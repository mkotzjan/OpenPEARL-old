#ifndef LPC17XXUSBKEYBOARD_INCLUDED
#define LPC17XXUSBKEYBOARD_INCLUDED

#include "SystemDationNB.h"
#include "Mutex.h"

namespace pearlrt {

   /**
   \file

   \brief USB Keyboard support

   */

   /** this class provides access to a usb keyboard.

   */
   class Lpc17xxUsbKeyboard : public SystemDationNB {
   private:
      Mutex mutex;
      int nbrOpenUserDations;
      //void* kbTaskHandle;
      char unGetChar;
      int status;
      static bool doNewLineTranslation;
   public:

      static void kbHostTask(void* keyboardHidInterface);
      /**
        define the keyboard device

      */
      Lpc17xxUsbKeyboard();

      /**
      open the system dation

      \param idf must be null, since no named files aree supported here
      \param openParam must be 0, since no open params are supported here
      \returns pointer to this object as working object
      */
      Lpc17xxUsbKeyboard* dationOpen(const char * idf, int openParam);

      /**
      close the systen dation

      \param closeParam mut be 0, since no parameters are supported
      */
      void dationClose(int closeParam);

      void dationRead(void * destination, size_t size);
      void dationWrite(void * destination, size_t size);
      void dationUnGetChar(const char c);
      int capabilities();
      void translateNewLine(bool doNewLineTranslation);
   };
}


#endif

