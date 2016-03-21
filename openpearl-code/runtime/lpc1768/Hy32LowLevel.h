


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

   The class is realizes as singleton, since only one display is possible
   */
   class Hy32LowLevel {
   private:
      uint16_t xmax, ymax;
      int orientation;
      uint16_t charWidth, charHeight, lineSpacing;
      int font;
      static const unsigned char * fonts[2];

      void init(int orientation);
      void setCursor(uint16_t x, uint16_t y);
      void setPoint(uint16_t x, uint16_t y, uint16_t color);
      void putSmallChar(uint16_t x, uint16_t y, char ch,
                        uint16_t chColor, uint16_t bkcolor);
      void putBigChar(uint16_t x, uint16_t y, char ch,
                      uint16_t chColor, uint16_t bkcolor);

      Hy32LowLevel();

      /**
        setup the display including GPIO configuration

        \param orientation defines the layout \\
             0 = portrait, connector down \\
             1 = landscape, connector left \\
             3 = portrait, connector up \\
             4 = landscape, connector right
      */
      Hy32LowLevel(int orientation);
      static Hy32LowLevel* instance;

   public:
      static Hy32LowLevel* getInstance(int orientation);

      int getXMax();
      int getYMax();
      int getCharWidth();
      int getCharHeight();

      uint16_t color(uint16_t red, uint16_t green, uint16_t blue);

      /**
      clear the display with the given color

      Check the macro RGB565 to create the color value with 5 bit for
         red and blue, and 6 bit for green

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

        \param fontNumber  0 = 8x12 font<br>
                           1 = 16x16 font
      */
      void setFont(int fontNumber) ;

      /**
      fill the rectangle given by (x0,y0) and (x1,y1) with the given color

      \param x0 starting x coordinate of the rectangle
      \param y0 starting y coordinate of the rectangle
      \param x1 ending x coordinate of the rectangle
      \param y1 ending y coordinate of the rectangle
      \param color the coded rgb value for the interior of the rectangle

      \note the orienation of the filling sequence is internally adjusted
        for maximum speed
      */
      void rectangle(uint16_t x0, uint16_t y0,
                     uint16_t x1, uint16_t y1, uint16_t bkColor) ;
   };
};
#endif
