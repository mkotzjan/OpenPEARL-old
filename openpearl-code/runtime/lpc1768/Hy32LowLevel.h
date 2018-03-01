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

#ifndef HY32LOWLEVEL_INCLUDED
#define HY32LOWLEVEL_INCLUDED

#include <stdint.h>

extern unsigned char SmallFont[];
extern unsigned char BigFont[];

namespace pearlrt {
   /**
   low level access routines to the hy32 display

   These methods are intended to be used by the system device drivers
   which will operate on the lcd

   The class is realized as singleton, since only one display is possible
   */
   class Hy32LowLevel {
   private:
      uint16_t xmax, ymax;
      int orientation;
      uint16_t charWidth, charHeight, lineSpacingUp, lineSpacingDown;
      int font;
      static const unsigned char * fonts[2];

      void init(int orientation);
      void setCursor(uint16_t x, uint16_t y);
      void setPoint(uint16_t x, uint16_t y, uint16_t color);
      void showLineSpacing(uint16_t x, uint16_t y,
                           uint16_t lineSpace, uint16_t bkColor);
      void putSmallChar(uint16_t x, uint16_t y, char ch,
                        uint16_t chColor, uint16_t bkcolor);
      void putBigChar(uint16_t x, uint16_t y, char ch,
                      uint16_t chColor, uint16_t bkcolor);

      Hy32LowLevel();
      Hy32LowLevel(int orientation);

      static Hy32LowLevel* instance;

   public:
      /**
        setup the display including GPIO configuration

        The display is adressed in pixel coordinates, starting with
        (0,0) for upper left and (xmax,ymax) for lower right.

        \param orientation defines the layout \\
             0 = portrait, connector down \\
             1 = landscape, connector left \\
             3 = portrait, connector up \\
             4 = landscape, connector right

        \returns pointer to the Hy32LowLevel object
      */
      static Hy32LowLevel* getInstance(int orientation);

      /**
        Read the biggest coordinate in x direction (left to right).
        The x direction is always the direction of text writing and
        depends on the selected orientation of the display.

        \returns the maximum pixel number in x direction
                  (counting starts with 0)
      */
      int getXMax();

      /**
        Read the biggest coordinate in y direction (up to down).

        \returns the maximum pixel number in y direction
                  (counting starts with 0)
      */
      int getYMax();

      /**
        read the number of pixel of a character in x direction, depending
        on the selected font. Only the pixel of the font matrix are
        regarded.

        \returns number of pixel in x-direction of the character
      */
      int getCharWidth();

      /**
        read the number of pixel of a character in y direction, depending
        on the selected font. Only the pixel of the font matrix are
        regarded.

        \returns number of pixel in y-direction of the character
      */
      int getCharHeight();

      /**
        calculate the device depending RBG value.

        The hy32 device does not support true color. The reduction
        to the available color bits is done in this function
        The display expects 5 bit for red and blue, and 6 bit for green.

        \param red intensity of red as value from 0 .. 255
        \param green intensity of green as value from 0 .. 255
        \param blue intensity of blue as value from 0 .. 255

        \returns the device depending rgb value
      */
      uint16_t color(uint16_t red, uint16_t green, uint16_t blue);

      /**
      clear the display with the given color


      \param color an 16 bit rgb-value
      */
      void clear(uint16_t color);

      /**
      draw a line from (x0,y0) to (x1,y1) in the given color

        \param x0 starting x coordinate
        \param y0 starting y coordinate
        \param x1 ending x coordinate
        \param y1 ending y coordinate
        \param color an 16 bit rgb-value
      */
      void drawLine(uint16_t x0, uint16_t y0,
                    uint16_t x1, uint16_t y1,
                    uint16_t color);

      /**
      set the given text to the location (x0,y0).
       The starting coordinate is the upper left corner of the first character.

      \param x0 starting coordinate for the text
      \param y0 starting coordinate for the text
      \param str character pointer to the null-terminated text
      \param color forground color of the text
      \param bkColor background color of the text
      */
      void text(uint16_t x0, uint16_t y0,
                const char *str,
                const uint16_t color, const uint16_t bkColor);

      /**
        select a given font

        The specified line spacing is distrubuted symetrically
        above and below of the pure text. In case of odd pixel
        number, the bigger part is below of the text line

        \param fontNumber  0 = 8x12 font<br>
                           1 = 16x16 font
        \param lineSpacing number of pixel between two lines in the current
      	  font. Must be >0 and < ymax-characterHeight
      */
      void setFont(int fontNumber, int lineSpacing) ;

      /**
      fill the rectangle given by (x0,y0) and (x1,y1) with the given color

      \param x0 starting x coordinate of the rectangle
      \param y0 starting y coordinate of the rectangle
      \param x1 ending x coordinate of the rectangle
      \param y1 ending y coordinate of the rectangle
      \param bkColor the 16 bit rgb value for the interior of the rectangle

      \note the orientation of the filling sequence is internally adjusted
        for maximum speed
      */
      void rectangle(uint16_t x0, uint16_t y0,
                     uint16_t x1, uint16_t y1, uint16_t bkColor) ;
   };
};
#endif
