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

#include "Hy32LowLevel.h"
#include <stdlib.h> // abs()

#include "chip.h"
#include "gpio_17xx_40xx.h"
#include "Signals.h"
#include "Log.h"

namespace pearlrt {

   /* LCD Interface */
#define PIN_EN		(1 << 19)
#define PIN_LE		(1 << 20)
#define PIN_DIR		(1 << 21)
#define PIN_CS      (1 << 22)
#define PIN_RS		(1 << 23)
#define PIN_WR		(1 << 24)
#define PIN_RD		(1 << 25)

#define LCD_EN(x)   ((x) ? (LPC_GPIO->SET = PIN_EN) :     \
                           (LPC_GPIO->CLR = PIN_EN));
#define LCD_LE(x)   ((x) ? (LPC_GPIO->SET = PIN_LE) :     \
                           (LPC_GPIO->CLR = PIN_LE));
#define LCD_DIR(x)  ((x) ? (LPC_GPIO->SET = PIN_DIR) :    \
                           (LPC_GPIO->CLR = PIN_DIR));
#define LCD_CS(x)   ((x) ? (LPC_GPIO->SET = PIN_CS) :     \
                           (LPC_GPIO->CLR = PIN_CS));
#define LCD_RS(x)   ((x) ? (LPC_GPIO->SET = PIN_RS) :     \
                           (LPC_GPIO->CLR = PIN_RS));
#define LCD_WR(x)   ((x) ? (LPC_GPIO->SET = PIN_WR) :     \
                           (LPC_GPIO->CLR = PIN_WR));
#define LCD_RD(x)   ((x) ? (LPC_GPIO->SET = PIN_RD) :     \
                           (LPC_GPIO->CLR = PIN_RD));

// display coordinates run alway from upper left =(0,0)
// to lower right=(239,319)
// the rotation is done in setCursor and auto fill register (ENTRY-Mode)
#define XMAX   239    // display max coordinate
#define YMAX   319
   const unsigned char *Hy32LowLevel::fonts[] = {SmallFont, BigFont};

   Hy32LowLevel * Hy32LowLevel::instance = 0;

   Hy32LowLevel * Hy32LowLevel::getInstance(int orientation) {
      if (!instance) {
         instance = new Hy32LowLevel(orientation);
      }

      return (instance);
   }

   int Hy32LowLevel::getXMax() {
      return xmax;
   }

   int Hy32LowLevel::getYMax() {
      return ymax;
   }

   int Hy32LowLevel::getCharWidth() {
      return charWidth;
   }

   int Hy32LowLevel::getCharHeight() {
      return charHeight;
   }

   static void hy32ConfigureGPIO(void) {
      /* Configure the LCD Control pins */

      /* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 */
      /* WR = P0.24 , RD = P0.25 , */
      /* DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */
      LPC_GPIO->DIR   |= 0x03f80000;
      LPC_GPIO->SET    = 0x03f80000;
   }

   static void hy32Send(uint16_t byte) {
      //LPC_GPIO2->FIODIR |= 0x0FF;          /* P2.0...P2.7 Output */
      LPC_GPIO2->DIR = 0x0ff;
      LCD_DIR(1)			    /* Interface A->B */
      LCD_EN(0)	                        /* Enable 2A->2B */
      LPC_GPIO2->CLR =  0x0ff;
      LPC_GPIO2->PIN = (byte & 0x0ff);          /* Write D0..D7 */
      LCD_LE(1)
      LCD_LE(0)				/* latch D0..D7	*/
      LPC_GPIO2->CLR &=  0x0ff;
      LPC_GPIO2->PIN = (byte >> 8) & 0x0ff;      /* Write D8..D15 */
   }

   static void wait_delay(int count) {
      volatile int xcount;

      for (xcount = 0; xcount < count; xcount++);
   }

   static uint16_t hy32Read(void) {
      uint16_t value;

      //LPC_GPIO2->FIODIR &= ~(0xFF);      /* P2.0...P2.7 Input */
      LPC_GPIO2->DIR = 0x000;
      LCD_DIR(0);		           /* Interface B->A */
      LCD_EN(0);	                   /* Enable 2B->2A */
      wait_delay(30);		           /* delay some times */
      value = LPC_GPIO2->PIN;              /* Read D8..D15 */
      LCD_EN(1);	                   /* Enable 1B->1A */
      wait_delay(30);			   /* delay some times */
      value = (value << 8) | (0x0ff & LPC_GPIO2->PIN); /* Read D0..D7 */
      LCD_DIR(1);
      return  value;
   }

   static void hy32WriteIndex(uint16_t index) {
      LCD_CS(0);
      LCD_RS(0);
      LCD_RD(1);
      hy32Send(index);
      wait_delay(22);
      LCD_WR(0);
      wait_delay(1);
      LCD_WR(1);
      LCD_CS(1);
   }

   static void hy32WriteData(uint16_t data) {
      LCD_CS(0);
      LCD_RS(1);
      hy32Send(data);
      LCD_WR(0);
      wait_delay(1);
      LCD_WR(1);
      LCD_CS(1);
   }

   static uint16_t hy32ReadData(void) {
      uint16_t value;

      LCD_CS(0);
      LCD_RS(1);
      LCD_WR(1);
      LCD_RD(0);
      value = hy32Read();

      LCD_RD(1);
      LCD_CS(1);

      return value;
   }

   static void hy32WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue) {
      /* Write 16-bit Index, then Write Reg */
      hy32WriteIndex(LCD_Reg);
      hy32WriteData(LCD_RegValue);
   }

   static uint16_t hy32ReadReg(uint16_t LCD_Reg) {
      uint16_t LCD_RAM;

      /* Write 16-bit Index (then Read Reg) */
      hy32WriteIndex(LCD_Reg);
      LCD_RAM = hy32ReadData();
      return LCD_RAM;
   }

   void Hy32LowLevel::setCursor(uint16_t col, uint16_t row) {
      switch (orientation) {
      case 0:
         hy32WriteReg(0x004e, col);       // display x
         hy32WriteReg(0x004f, row);       // display y
         break;

      case 1:
         hy32WriteReg(0x004e, row);       // display x
         hy32WriteReg(0x004f, xmax - col); // display y
         break;

      case 2:
         hy32WriteReg(0x004e, xmax - col);     // display x
         hy32WriteReg(0x004f, ymax - row);     // display y
         break;

      case 3:
         hy32WriteReg(0x004e, ymax - row);     // display x
         hy32WriteReg(0x004f, col);        // display y
         break;
      }

   }

   static void delay_ms(uint16_t ms) {
      uint16_t i, j;

      for (i = 0; i < ms; i++) {
         for (j = 0; j < 1141 /* 20000*/; j++)  /* xcount ++ */ ;
      }
   }

   Hy32LowLevel::Hy32LowLevel() {
      init(0);
   }

   Hy32LowLevel::Hy32LowLevel(int orientation) {
      init(orientation);
   }


   void Hy32LowLevel::init(int _orientation) {
      volatile	uint16_t DeviceCode;
      static const struct {
         uint16_t reg;
         uint16_t data;
      } initArray[] = {
         {0x0000, 0x0001},   // Oscillator start
         {0x0003, 0xA8A4},   // Power Control (1)
         {0x000C, 0x0000},   // Power Control (2)
         {0x000D, 0x080C},   // Power Control (3)
         {0x000E, 0x2B00},   // Power Control (4)
         {0x001E, 0x00B0},   // Power Control (5)
         {0x0001, 0x2B3F},   // Driver Output Control
         {0x0002, 0x0600},   // LCD drive AC Control
         {0x0010, 0x0000},   // Sleep mode (off)
         {0x0011, 0x6070},   // entry mode
         {0x0005, 0x0000},   // Compare Register(1)
         {0x0006, 0x0000},   // Compare Register(2)
         {0x0016, 0xEF1C},   // Horizontal Porch    (default)
         {0x0017, 0x0003},   // vertical Porch
         {0x0007, 0x0033},   // Display Control (one screen)
         {0x000B, 0x0000},   // frame cycle control
         {0x000F, 0x0000},   // get scan start position (default)
         {0x0041, 0x0000},   // Vertical Scroll Control (1) (default)
         {0x0042, 0x0000},   // Vertical Scroll Control (2) (default)
         {0x0048, 0x0000},   // First Window Start (default)
         {0x0049, 0x013F},   // First Window End (default)
         {0x004A, 0x0000},   // Second Window Start (default)
         {0x004B, 0x0000},   // Second Window End
         {0x0044, 0xEF00},   // Horizontal RAM Address Position (default)
         {0x0045, 0x0000},   // vertical RAM Address Position (default)
         {0x0046, 0x013F},   // vertical RAM Address end (default)
         {0x0030, 0x0707},   // gamma control (1)
         {0x0031, 0x0204},   // gamma control (2)
         {0x0032, 0x0204},   // gamma control (3)
         {0x0033, 0x0502},   // gamma control (4)
         {0x0034, 0x0507},   // gamma control (5)
         {0x0035, 0x0204},   // gamma control (6)
         {0x0036, 0x0204},   // gamma control (7)
         {0x0037, 0x0502},   // gamma control (8)
         {0x003A, 0x0302},   // gamma control (9)
         {0x003B, 0x0302},   // gamma control (10)
         {0x0023, 0x0000},   // RAM Write data mask (1)  (default)
         {0x0024, 0x0000},   // RAM Write data mask (2)  (default)
         {0x0025, 0x8000},   // Frame Frequency (default)
         {0x004f, 0     },   // set GDDRAM y Address (default)
         {0x004e, 0     }    // set GDDRAM x Address (default)
      };
      size_t i;

      orientation = _orientation;

      hy32ConfigureGPIO();
      delay_ms(100);
      DeviceCode = hy32ReadReg(0x0000);

      if (DeviceCode == 0x8989) {
         for (i = 0; i < sizeof(initArray) / sizeof(initArray[0]); i++) {
            hy32WriteReg(initArray[i].reg, initArray[i].data);
            delay_ms(50);
         }
      } else {
         Log::error("Hy32LowLevel: unknown display controller %x", DeviceCode);
         throw theInternalDationSignal;
      }

      switch (orientation) {
      default:
         Log::error("Hy32LowLevel: illegal orientation requested %d",
                    orientation);
         throw theInternalDationSignal;

      case 0: // portrait connector down
         xmax = 239;
         ymax = 319;
         hy32WriteReg(0x011, 0x06070);// set auto increment direction
         break;

      case 1: // landscape connector left
         xmax = 319;
         ymax = 239;
         hy32WriteReg(0x011, 0x06058);// set auto increment direction
         break;

      case 2: // portrait connector up
         xmax = 239;
         ymax = 319;
         hy32WriteReg(0x011, 0x06040);// set auto increment direction
         break;

      case 3: // landscape connector right
         xmax = 319;
         ymax = 239;
         hy32WriteReg(0x011, 0x06078);// set auto increment direction
         break;
      }

      delay_ms(50);   /* delay 50 ms for safety */
      setFont(0, 2);
   }

   uint16_t Hy32LowLevel::color(uint16_t red, uint16_t green, uint16_t blue) {
      uint16_t color;
      color = (uint16_t)(((red   >> 3) << 11) |
                         ((green >> 2) << 5) |
                         (blue  >> 3));
      return (color);
   }

   void Hy32LowLevel::clear(uint16_t Color) {
      uint32_t index;
      //hy32WriteReg(0x004e, 0);
      //hy32WriteReg(0x004f, 0);
      setCursor(0, 0);
      hy32WriteIndex(0x0022);

      for (index = 0; index < (XMAX + 1) * (YMAX + 1); index++) {
         hy32WriteData(Color);
      }
   }



#ifdef REMOVED
   static uint16_t LCD_BGR2RGB(uint16_t color) {
      uint16_t  r, g, b, rgb;

      b = (color >> 0)  & 0x1f;
      g = (color >> 5)  & 0x3f;
      r = (color >> 11) & 0x1f;

      rgb = (b << 11) + (g << 5) + (r << 0);

      return (rgb);
   }

#endif

   void Hy32LowLevel::setPoint(uint16_t x, uint16_t y, uint16_t color) {
      if (x > xmax || y > ymax) {
         Log::error("Hy32LowLevel: point out of region (%d,%d)", x, y);
         throw theInternalDationSignal;
      }

      setCursor(x, y);
      hy32WriteReg(0x0022, color);
   }

   void Hy32LowLevel::drawLine(uint16_t x0, uint16_t y0,
                               uint16_t x1, uint16_t y1,
                               uint16_t color) {
      /* bresenham algorithm from
         https://de.wikipedia.org/wiki/Bresenham-Algorithmus
      */
      int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
      int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
      int err = dx + dy, e2; /* error value e_xy */

      while (1) {
         setPoint(x0, y0, color);

         if (x0 == x1 && y0 == y1) {
            break;
         }

         e2 = 2 * err;

         if (e2 > dy) {
            err += dy;   /* e_xy+e_x > 0 */
            x0 += sx;
         }

         if (e2 < dx) {
            err += dx;   /* e_xy+e_y < 0 */
            y0 += sy;
         }
      }
   }

   void Hy32LowLevel::showLineSpacing(uint16_t x, uint16_t y,
                                      uint16_t ls, uint16_t bkColor) {
      uint16_t i, j;

      // set upper or lower lineSpacing
      for (i = 0; i < ls; i++) {
         setPoint(x, y + i, bkColor);

         for (j = 1; j < charWidth; j++) {
            hy32WriteReg(0x0022, bkColor);
         }
      }
   }

   // put one character in small character set
   // starting pos is upper left of the character box
   void Hy32LowLevel::putSmallChar(uint16_t Xpos, uint16_t Ypos, char ch,
                                   uint16_t charColor, uint16_t bkColor) {
      uint16_t i, j;
      uint16_t pixel;
      uint16_t charIsOk;
      char  tmp_char;
      charIsOk = 1;

      if (ch < 32 || ch > 127) {
         charIsOk = 0;
      }

      showLineSpacing(Xpos, Ypos, lineSpacingUp, bkColor);
      Ypos += lineSpacingUp;

      // display is set to autoincrement of write address from left to right
      // set the adress and pixel only for first pixel in a row in the
      for (i = 0; i < charHeight; i++) {
         if (charIsOk) {
            tmp_char = SmallFont[charHeight * (ch - 32) + 4 + i];
         } else {
            tmp_char = (i & 1) ? 0x55 : 0xaa;
         }

         //character
         if ((tmp_char & 0x080) == 0x080) {
            pixel = charColor;
         } else {
            pixel = bkColor;
         }

         setPoint(Xpos, Ypos + i, pixel);

         // and send the other pixels without further positioning
         // this speeds up by a factor of approximatelly 2
         for (j = 1; j < 8; j++) {
            pixel = (((tmp_char >> (7 - j)) & 0x01) == 0x01) ? charColor :
                    bkColor;
            hy32WriteReg(0x0022, pixel);
         }
      }

      Ypos += charHeight;

      showLineSpacing(Xpos, Ypos, lineSpacingDown, bkColor);
   }

// put one character
// starting pos is upper left of the character box
   void Hy32LowLevel::putBigChar(uint16_t Xpos, uint16_t Ypos, char ch,
                                 uint16_t charColor, uint16_t bkColor) {
      uint16_t i, j;
      uint16_t pixel;
      uint16_t charIsOk;
      char  tmp_char;
      charIsOk = 1;

      if (ch < 32 || ch > 127) {
         charIsOk = 0;
      }

      showLineSpacing(Xpos, Ypos, lineSpacingUp, bkColor);
      Ypos += lineSpacingUp;

      for (i = 0; i < charHeight; i++) {
         if (charIsOk) {
            tmp_char = BigFont[charHeight * (ch - 32) * 2 + 4 + 2 * i];
         } else {
            tmp_char = (i & 1) ? 0x55 : 0xaa;
         }

         // display is set to autoincrement of write address from left to right
         // set the adress and pixel only for first pixel in a row in the
         //character
         if ((tmp_char & 0x080) == 0x080) {
            pixel = charColor;
         } else {
            pixel = bkColor;
         }

         setPoint(Xpos, Ypos + i, pixel);

         // and send the other pixels without further positioning
         // this speeds up by a factor of approximatelly 2
         for (j = 1; j < 8; j++) {
            pixel = (((tmp_char >> (7 - j)) & 0x01) == 0x01) ? charColor :
                    bkColor;
            hy32WriteReg(0x0022, pixel);
         }

         // and send the next 8 bit
         if (charIsOk) {
            tmp_char = BigFont[charHeight * (ch - 32) * 2 + 4 + 2 * i + 1];
         } else {
            tmp_char = (i & 1) ? 0x55 : 0xaa;
         }

         for (j = 0; j < 8; j++) {
            pixel = (((tmp_char >> (7 - j)) & 0x01) == 0x01) ? charColor :
                    bkColor;
            hy32WriteReg(0x0022, pixel);
         }
      }

      Ypos += charHeight;

      showLineSpacing(Xpos, Ypos, lineSpacingDown, bkColor);
   }

   void Hy32LowLevel::setFont(int fontNumber, int lineSpacing) {
      if (fontNumber == 0 || fontNumber == 1) {
         font = fontNumber;
         charWidth = *fonts[font];
         charHeight = *(fonts[font] + 1);
      } else {
         Log::error("Hy32LowLevel: Illegal font number (%d)", fontNumber);
         throw theInternalDationSignal;
      }

      if (lineSpacing < 0 || lineSpacing > ymax - charHeight) {
         Log::error("Hy32LowLevel: Illegal line spacing (%d)", lineSpacing);
         throw theInternalDationSignal;
      }

      // distribute equally to top and bottom of line. If lineSpacing is
      // odd, the lower part will be 1 pixel larger
      lineSpacingUp = lineSpacing / 2;
      lineSpacingDown = (lineSpacing + 1) / 2;
   }

   // print string
   // starting pos is upper left of the first character box
   void Hy32LowLevel::text(uint16_t x, uint16_t y, const char *str,
                           const uint16_t Color, const uint16_t bkColor) {
      char TempChar;

      do {
         TempChar = *str++;

         switch (font) {
         case 0:
            putSmallChar(x, y, TempChar, Color, bkColor);
            break;

         case 1:
            putBigChar(x, y, TempChar, Color, bkColor);
            break;
         }

         if (x < xmax - charWidth) {   //  << char box width
            x += charWidth;
         } else if (y < ymax - charHeight) { // << char box height
            x = 0;
            y += charHeight;
         } else {
            x = 0;
            y = 0;
         }
      } while (*str != 0);
   }

   void Hy32LowLevel::rectangle(uint16_t x0, uint16_t y0,
                                uint16_t x1, uint16_t y1,
                                uint16_t color) {
      uint16_t temp, x;

      // enshure x0 <= x1 and y0 <= y1
      if (x0 > x1) {
         temp = x1;
         x1 = x0;
         x0 = temp;
      }

      if (y0 > y1) {
         temp = y1;
         y1 = y0;
         y0 = temp;
      }

      // perform clipping
      x0 = (x0 > xmax) ? xmax : x0;
      x1 = (x1 > xmax) ? xmax : x1;
      y0 = (y0 > ymax) ? ymax : y0;
      y1 = (y1 > ymax) ? ymax : y1;

      // perform filling to x direction like the characters are written
      for (temp = y0; temp <= y1; temp ++) {
         setPoint(x0, temp, color);

         for (x = x0 + 1; x <= x1; x++) {
            hy32WriteReg(0x0022, color);
         }
      }
   }
}

