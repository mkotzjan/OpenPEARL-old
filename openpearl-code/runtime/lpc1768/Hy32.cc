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
#include <cctype>

#include "chip.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "Lpc17xxUart.h"
#include "Log.h"
#include "Hy32.h"
#include "Hy32LowLevel.h"

namespace pearlrt {

   const uint16_t Hy32::lineSpacingSmall = 2;
   const uint16_t Hy32::lineSpacingBig = 2;

   Hy32::Hy32(int orientation) {
printf("Hy32-ctor\n");
      if (orientation < 0 || orientation > 3) {
         Log::error("Hy32: illegal orientation: %d", orientation);
         throw theInternalDationSignal;
      }

      device = Hy32LowLevel::getInstance(orientation);
      colorMap[0] = device->color(0, 0, 0); 		// black
      colorMap[1] = device->color(170, 0, 0); 		// red
      colorMap[2] = device->color(0, 170, 0); 		// green
      colorMap[3] = device->color(255, 255, 0); 	// yellow
      colorMap[4] = device->color(0, 0, 170); 		// blue
      colorMap[5] = device->color(170, 0, 170);	// magenta
      colorMap[6] = device->color(0, 170, 170); 	// cyan
      colorMap[7] = device->color(255, 255, 255); 	// white

      colorIndexBackground = 0;
      colorIndexForeground = 7;

      xmax = device ->getXMax();
      ymax = device ->getYMax();

      device->clear(colorMap[colorIndexBackground]);
      lineSpacing = lineSpacingSmall ;
      device->setFont(0, lineSpacing);
      adjustGeometry();

      line = 0;
      col = 0;
      state = 0;

      mutex.name("Hy32");
      nbrOpenUserDations = 0;
   }


   int Hy32::capabilities() {
      return (FORWARD | ANY | PRM | OUT);
   }

   Hy32* Hy32::dationOpen(const char * idfValue, int openParams) {
      if (openParams & (Dation::IDF | Dation::CAN)) {
         Log::error("Hy32: does not support IDF and CAN");
         throw theDationParamSignal;
      }

      mutex.lock();

      nbrOpenUserDations++;

      mutex.unlock();

      return this;
   }

   void Hy32::dationClose(int closeParams) {

      mutex.lock();

      if (nbrOpenUserDations == 0) {
         Log::error("Hy32: no dation opened");
         mutex.unlock();
         throw theCloseFailedSignal;
      }

      nbrOpenUserDations--;

      if (closeParams & Dation::CAN) {
         Log::error("Hy32: CAN not supported");
         mutex.unlock();
         throw theDationParamSignal;
      }

      mutex.unlock();
   }

   void Hy32::dationRead(void * destination, size_t size) {

      Log::error("Hy32: read is not supported");
      throw theInternalDationSignal;
   }

   uint16_t Hy32::xOfCurrentPos() {
      return col * charWidth;
   }

   uint16_t Hy32::yOfCurrentPos() {
      return line * (charHeight + lineSpacing);
   }

   void Hy32::adjustGeometry() {
      charWidth = device->getCharWidth();
      charHeight = device->getCharHeight();
      lines = ymax / (charHeight + lineSpacing);
      cols = xmax / charWidth;

      if (col >= cols) {
         col = cols - 1;
      }

      if (line >= lines) {
         line = lines - 1;
      }
   }

   void Hy32::dationWrite(void * destination, size_t size) {
      static const char csiString[] = {'\033', '[', 0};
      char chString[2];
      char * ch;
      uint16_t x0, y0; // pixel coordinates for clear

      if (nbrOpenUserDations == 0) {
         Log::error("Hy32: not opened");
         throw theDationNotOpenSignal;
      }

      mutex.lock();

      ch = (char*)destination;

      for (; size > 0; size--) {
         switch (state) {
         case 0: // normal character
            if ((*ch) == 0x1b) {
               state = 1;
            } else if ((*ch) == '\n') {
               col = 0;
               line ++;

               if (line >= lines) {
                  line = 0;
               }
            } else {
               chString[0] = *ch;
               chString[1] = 0;
               device->text(col * charWidth,
                            line * (charHeight + lineSpacing),
                            chString,
                            colorMap[colorIndexForeground],
                            colorMap[colorIndexBackground]);
               col ++;

               if (col >= cols) {
                  col = 0;
                  line ++;

                  if (line >= lines) {
                     line = 0;
                  }
               }
            }

            break;

         case 1: // esc was received
            if ((*ch) == '[') {
               state = 2;
               p1 = 0;
               p2 = 0;
            } else {
               device->text(col * charWidth,
                            line * (charHeight + lineSpacing),
                            csiString,
                            colorMap[colorIndexForeground],
                            colorMap[colorIndexBackground]);
               col += 2;
               state = 0;
            }

            break;

         case 2: // CSI received gathering p1

            // possible patterns:
            // digits + char
            // digits +';' + digits + char
            if (isdigit(*ch)) {
               p1 = 10 * p1 + (*ch) - '0';
            } else {
               switch (*ch) {
               case ';' :
                  state = 3;
                  break;

               case 'J':  // clear

                  // the bahavior of the text cursor location is not
                  // clearly defined
                  // we implement the behavior of xterm and linux terminal
                  // they leave the text cursor unchanged
                  switch (p1) {
                  case 0: // to end of screen
                     x0 = xOfCurrentPos();
                     y0 = yOfCurrentPos();

                     if (col > 0) {
                        device->rectangle(
                           x0, y0,
                           xmax, y0 + charHeight + lineSpacing - 1,
                           colorMap[colorIndexBackground]);

                        device->rectangle(
                           0, y0 + charHeight + lineSpacing,
                           xmax, ymax,
                           colorMap[colorIndexBackground]);

                     } else {
                        device->rectangle(x0, y0, xmax, ymax,
                                          colorMap[colorIndexBackground]);
                     }

                     break;

                  case 1: // to begin of screen
                     x0 = xOfCurrentPos();
                     y0 = yOfCurrentPos();

                     if (col > 0) {
                        device->rectangle(
                           0, 0, xmax, y0 - 1,
                           colorMap[colorIndexBackground]);
                        device->rectangle(
                           0, y0,
                           x0 - 1, y0 + charHeight + lineSpacing - 1,
                           colorMap[colorIndexBackground]);
                     } else {
                        device->rectangle(0, 0, xmax, y0 - 1,
                                          colorMap[colorIndexBackground]);
                     }

                     break;

                  case 2: // complete screen
                     device->clear(colorMap[colorIndexBackground]);
                     break;
                  }

                  state = 0;
                  break;

               case 'm': // color/font
                  if (p1 >= 30 && p1 <= 37) {
                     colorIndexForeground = p1 - 30;
                  } else if (p1 >= 40 && p1 <= 47) {
                     colorIndexBackground = p1 - 40;
                  } else if (p1 == 10) {
                     lineSpacing = lineSpacingSmall;
                     device->setFont(0, lineSpacing);
                     adjustGeometry();
                  } else if (p1 == 11) {
                     lineSpacing = lineSpacingBig;
                     device->setFont(1, lineSpacing);
                     adjustGeometry();
                  } else {
                     // unknown command -- ignore it
                     Log::warn("Hy32: illegal color/font: %d", p1);
                  }

                  state = 0;

                  break;

               default:   // illegal char
                  // unknown command -- ignore it
                  Log::warn("Hy32: unkwnown esc sequence (%x)", *ch);
                  state = 0;
                  break;
               }

            }

            break;

         case 3: // CSI and p1+';' received gathering p2
            if (isdigit(*ch)) {
               p2 = 10 * p2 + (*ch) - '0';
            } else {
               switch (*ch) {
               case 'H': // set position
                  if (p1 > 0 && p2 > 0 && p1 <= lines && p2 <= cols) {
                     line = p1-1;
                     col = p2-1;
                  } else {
                     // illegal coordinate -- ignore it
                     Log::warn("Hy32: coordinate (%d,%d) outside display area",
                        p1, p2);
                  }

                  state = 0;
                  break;

               default:   // illegal char
                  // unknown command -- ignore it
                  Log::warn("Hy32: unkwnown esc sequence (%x)", *ch);
                  state = 0;
                  break;
               }

               state = 0;
            }

            break;
         }

         ch ++;
      }

      mutex.unlock();
   }

   void Hy32::dationUnGetChar(const char c) {
      // do nothing, sice get is not supported --> unget is not
      // needed
   }

   void Hy32::translateNewLine(bool doNewLineTranslation) {
      // do nothing - Hy32 does support only ALPHIC access
   }
}

