


#ifndef HY32
#define HY32

#include <stdint.h>
#include "Hy32LowLevel.h"


/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

namespace pearlrt {
   /**
   stream dation for text output on the lcd

   The dation simulates some vt100 control sequences for positioning,
   clear  and color selection

   */
   class Hy32 : public SystemDationNB {
   private:
      Hy32LowLevel* device;
      Mutex mutex;
      int nbrOpenUserDations;
      uint16_t colorMap[8];
      int colorIndexBackground;
      int colorIndexForeground;
      int state;  // state index while decoding the esc sequence
      int p1, p2; // numeric parameters in esc-sequences

      uint16_t xmax, ymax;
      uint16_t line, col;
      int orientation;
      uint16_t charWidth, charHeight, lineSpacing;

      Hy32();

   public:
      /**
        setup the display including GPIO configuration

        \param orientation defines the layout \\
             0 = portrait, connector down \\
             1 = landscape, connector left \\
             2 = portrait, connector up \\
             3 = landscape, connector right
      */
      Hy32(int orientation);

      Hy32 * dationOpen(const char * idfValue, int openParam);

      void  dationClose(int closeParam);

      int capabilities();
      void dationRead(void * destination, size_t size);
      void dationWrite(void * destination, size_t size);
      void dationUnGetChar(const char c);

   };
};
#endif
