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

#ifndef HY32
#define HY32

#include <stdint.h>
#include "Hy32LowLevel.h"


namespace pearlrt {
   /**
   stream dation for text output on the lcd

   The dation simulates some vt100 control sequences for positioning,
   clear  and color selection. 

   Unknown ESC-sequences are ignored and a warn-message is produced
   This is also done for positioning outside the display area.

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
      uint16_t lines, cols;   // geometry of the display
      uint16_t line, col;     // current cursor position
      int orientation;
      uint16_t charWidth, charHeight, lineSpacing;
      static const uint16_t lineSpacingSmall;
      static const uint16_t lineSpacingBig;
      void adjustGeometry();
      uint16_t xOfCurrentPos();
      uint16_t yOfCurrentPos();
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
      void translateNewLine(bool doNewLineTranslation);

   };
};
#endif
