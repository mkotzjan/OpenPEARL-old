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

   Hy32::Hy32(int orienation) {
      if (orientation < 0 || orientation > 3) {
         Log::error("Hy32: illegal orientation: %d", orientation);
         throw theIllegalParamSignal;
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
      charWidth = device->getCharWidth();
      charHeight = device->getCharHeight();
      lineSpacing = 1;

      device->clear(colorMap[colorIndexBackground]);
      device->setFont(0);

      mutex.name("Hy32");
      nbrOpenUserDations = 0;
   }


   int Hy32::capabilities() {
      return (FORWARD | ANY | PRM | OUT);
   }

   Hy32* Hy32::dationOpen(const char * idfValue, int openParams) {
      if (openParams & (Dation::IDF | Dation::CAN)) {
         Log::error("Hy32: does not support IDF and CAN");
         throw theIllegalParamSignal;
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
         throw theIllegalParamSignal;
      }

      mutex.unlock();
   }

   void Hy32::dationRead(void * destination, size_t size) {

      Log::error("Hy32: read is not supported");
      throw theIllegalParamSignal;
   }

   void Hy32::dationWrite(void * destination, size_t size) {
      static const char csiString[] = {'\033', '[', 0};
      char chString[2];

      char * ch;

      if (nbrOpenUserDations == 0) {
         Log::error("Hy32: not opened");
         throw theIllegalParamSignal;
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
            } else {
               chString[0] = *ch;
               chString[1] = 0;
               device->text(col*charWidth, 
                            line*(charHeight+lineSpacing)+lineSpacing/2,
                            chString,
                            colorMap[colorIndexForeground],
                            colorMap[colorIndexBackground]);
               col ++;
            }

            break;

         case 1: // esc was received
            if ((*ch) == '[') {
               state = 2;
               p1 = 0;
               p2 = 0;
            } else {
               device->text(col*charWidth, 
                            line*(charHeight+lineSpacing)+lineSpacing/2,
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
printf("clear mode %d\n", p1);
                  break;

               case 'm': // color/font
                 if (p1 >= 0 && p1 <= 7) {
                     colorIndexForeground = p1;
                 } else if (p1 >= 40 && p1 <= 46 ) {
                     colorIndexBackground = p1;
                 } else {
printf("color/font %d\n", p1);
                 }
                  break;

               default:   // illegal char
                  printf("illegal esc sequence -- dump string missing\n");
                  break;
               }

               state = 0;
            }

            break;

         case 3: // CSI and p1+';' received gathering p2
            if (isdigit(*ch)) {
               p2 = 10 * p2 + (*ch) - '0';
            } else {
               switch (*ch) {
               case 'H': // set position
printf("set cursor %d,%d\n", p1,p2);
                  break;

               default:   // illegal char
                  printf("illegal esc sequence -- dump string missing\n");
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
}

